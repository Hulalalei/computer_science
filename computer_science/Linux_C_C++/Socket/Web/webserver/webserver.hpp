#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP


/* 扩展：
1、命名空间
2、自动资源管理
3、设计模式

bug:
    一般崩在数据库没有对应的表
    字符串最前面一般是有\0了
    内存区域重叠
    Content-Length算错
    passwd结尾有很多\0，参与字符串比较
*/
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "./async_thread_pool/async_thread_pool.hpp"
#include "./connection_pool/connection_pool.hpp"
#include "./timer/timer.hpp"
#include "./http_conn/http_conn.hpp"
#include "./log/log.hpp"


namespace web {
const int MAX_FD = 65536;
const int MAX_EVENT_NUMBER = 10000;
const int TIMESLOT = 5;
const int SIZE = 100;

class webserver {
public:
    webserver();
    ~webserver();

	void init();
    void thread_pool();
    void sql_pool();

    void event_listen();
    void event_loop();
    // 为每一个用户设置一个计时器
    void timer(int connfd, struct sockaddr_in client_address);
    void adjust_timer(timer_node *timer);
    void deal_timer(timer_node *timer, int sockfd);

    // loop
    // 接收用户连接，并设置计时器
    bool deal_clientdata();
    bool deal_signal(bool &timeout, bool &stop_server);
    void deal_read(int sockfd);
    void deal_write(int sockfd);

public:
    // 基础
    int _port;
    std::string _root;
    int _pipefd[2];
    int _epfd;
    http_conn *users;

    // 数据库相关
    connection_pool *_connpool;

    // 线程池相关
    async_thread_pool *_threadpool;

    // 日志相关
    class log log_writer;

    // epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];
    int _serv_sock;

    // 定时器相关
    client_data *users_timer;
    class utils _util;
};


webserver::webserver() {
    users = new http_conn[MAX_FD];
    _root = std::string("./res");
    users_timer = new client_data[MAX_FD];
}

webserver::~webserver() {
    close(_epfd);
    close(_serv_sock);
    close(_pipefd[1]);
    close(_pipefd[0]);
    delete[] users;
    delete[] users_timer;
    delete _threadpool;
}

void webserver::init() {
    std::ifstream ifsJson("./webconfig.json", std::ifstream::in);
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifsJson, root);
    _port = root["server_port"].asInt();
}

void webserver::thread_pool() {
    _threadpool = new async_thread_pool();
}

void webserver::sql_pool() {
    _connpool = connection_pool::get_instance();
    users->init_mysql_result(_connpool);
}

void webserver::event_listen() {
    _serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 端口复用 以及 优雅断开连接
    int opt = 1;
    if (setsockopt(_serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("re_use error\n");
    struct linger tmp = {1, 1};
    setsockopt(_serv_sock, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    // 绑定 以及 监听
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(_port);
    if (bind(_serv_sock, (struct sockaddr *)&serv_adr, sizeof serv_adr) == -1)
        throw std::runtime_error("bind() error\n");

    if (listen(_serv_sock, SIZE) == -1)
        throw std::runtime_error("listen() error\n");

    _util.init(TIMESLOT);
    epoll_event events[MAX_EVENT_NUMBER];
    _epfd = epoll_create(SIZE);

    _util.addfd(_epfd, _serv_sock, false);
    http_conn::_epfd = _epfd;

	//UNIX域流套接字
    socketpair(PF_UNIX, SOCK_STREAM, 0, _pipefd);
    _util.set_nonblocking(_pipefd[1]);
    _util.addfd(_epfd, _pipefd[0], false);

    _util.addsig(SIGPIPE, SIG_IGN);
	// 发送signal
    _util.addsig(SIGALRM, _util.sig_handler, false);
    _util.addsig(SIGTERM, _util.sig_handler, false);
    _util.addsig(SIGTSTP, _util.sig_handler, false);

    alarm(TIMESLOT);

    utils::u_pipefd = _pipefd;
    utils::u_epfd = _epfd;
}

void webserver::event_loop() {
    bool timeout = false;
    bool stop_server = false;

    while (!stop_server) {
        int event_cnt = epoll_wait(_epfd, events, MAX_EVENT_NUMBER, -1);

        for (int i = 0; i < event_cnt; i ++) {
            int sockfd = events[i].data.fd;

            // 处理新到的客户连接，绑定计时器
            if (sockfd == _serv_sock) {
                log_writer.write(log::info, "new client");
                bool flag = deal_clientdata();
                if (false == flag)
                    continue;
            }
            else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                // 服务器端关闭连接，移除对应的定时器
                log_writer.write(log::info, "client close");
                timer_node *timer = users_timer[sockfd]._timer;
                deal_timer(timer, sockfd);
            }
            // 处理信号
            else if ((sockfd == _pipefd[0]) && (events[i].events & EPOLLIN)) {
                log_writer.write(log::info, "get a signal");
                bool flag = deal_signal(timeout, stop_server);
                if (false == flag)
                    log_writer.write(log::error, "deal_signal error");
            }
            // 处理客户连接上接收到的数据
            else if (events[i].events & EPOLLIN) {
                log_writer.write(log::info, "read data");
                deal_read(sockfd);
            }
            else if (events[i].events & EPOLLOUT) {
                log_writer.write(log::info, "write data");
                deal_write(sockfd);
            }
        }
        if (timeout) {
            log_writer.write(log::info, "timeout");
            _util.timer_handler();
            timeout = false;
        }
    }
}

bool webserver::deal_clientdata() {
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    int connfd = accept(_serv_sock, (struct sockaddr *)&client_address, &client_addrlength);
    if (connfd < 0) {
        log_writer.write(log::error, "accept error");
        return false;
    }
    if (http_conn::_users_count >= MAX_FD) {
        _util.show_error(connfd, "Internal server busy");
        return false;
    }
    timer(connfd, client_address);
    return true;
}

bool webserver::deal_signal(bool &timeout, bool &stop_server) {
    int ret = 0;
    int sig;
    char signals[1024];
    ret = recv(_pipefd[0], signals, sizeof(signals), 0);
    if (ret == -1)
    {
        return false;
    }
    else if (ret == 0)
    {
        return false;
    }
    else {
        for (int i = 0; i < ret; ++i) {
            switch (signals[i])
            {
                case SIGALRM:
                {
                    timeout = true;
                    break;
                }
                case SIGTERM:
                {
                    stop_server = true;
                    break;
                }
                case SIGTSTP:
                {
                    stop_server = true;
                    break;
                }
            }
        }
    }
    return true;
}

void webserver::deal_read(int sockfd) {
    timer_node *timer = users_timer[sockfd]._timer;
    // proactor
    if (users[sockfd].read_once()) {
        // 若监测到读事件，将该事件放入请求队列
        _threadpool->submit([&]() {
            users[sockfd].mysql_conn = _connpool->get_conn();
            users[sockfd].process();
            _connpool->release_conn(users[sockfd].mysql_conn);
        });

        if (timer) adjust_timer(timer);
    }
    else deal_timer(timer, sockfd);
}

void webserver::deal_write(int sockfd) {
    timer_node *timer = users_timer[sockfd]._timer;
    // proactor
    if (users[sockfd].write())
        if (timer)
            adjust_timer(timer);

    else deal_timer(timer, sockfd);
}

void webserver::timer(int connfd, struct sockaddr_in client_address) {
    users[connfd].init(connfd, client_address, _root);

    // 初始化client_data数据
    // 创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
    users_timer[connfd].address = client_address;
    users_timer[connfd].sockfd = connfd;

    timer_node *timer = new timer_node;
    timer->user_data = &users_timer[connfd];
    timer->func = cb_func;
    time_t cur = timer::get_tick();
    timer->expire = cur + 3 * TIMESLOT;

    users_timer[connfd]._timer = timer;
    _util.util_timer.add_timer(*timer);
}

void webserver::adjust_timer(timer_node *timer) {
    time_t cur = timer::get_tick();
    timer->expire = cur + 3 * TIMESLOT;
}

void webserver::deal_timer(timer_node *timer, int sockfd) {
    timer->func(*timer);
    if (timer) _util.util_timer.del_timer(*timer);
}

}
#endif

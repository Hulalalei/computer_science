#ifndef WEBSERVER
#define WEBSERVER


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
#include <functional>
#include <coroutine>
#include <concepts>
#include <type_traits>

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <json/json.h>
#include <mysql/mysql.h>

#include <thread_pool.hpp>
#include <connection_pool.hpp>
#include <memory_pool.hpp>
#include <http_conn.hpp>
#include <timer.hpp>
#include <minilog.hpp>
#include <debug.hpp>


namespace co_asyncweb {
    std::source_location loc;

    std::string get_filename(std::string pathname) {
        auto pos = pathname.rfind("/");
        return pathname.substr(pos + 1);
    }

    void check_error(int err, std::string express, std::string pathname, int line) {
        if (-1 == err) {
            auto filename = get_filename(pathname);
            std::cout << std::format("{} : {} ", filename, line);
            throw std::runtime_error(express);
        }
    }

    const int MAX_FD = 65536;
    const int MAX_EVENT_NUMBER = 10000;
    const int TIMESLOT = 5;
    const int SIZE = 100;
    struct Promise {
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { throw; }
        std::suspend_always yield_value(int ret) { m_val = ret; return {}; }
        void return_void() { m_val = 0; }
        std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }

        int m_val;
    };
    struct task {
        using promise_type = Promise;
        std::coroutine_handle<promise_type> m_co;
        task(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
        task(task &&) = delete;
        ~task() { m_co.destroy(); }

        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept { }
        void await_resume() const noexcept { return; }
    };

    class webserver {
    friend struct client_awaitable;
    public:
        webserver();
        ~webserver();

        void init();
        void thread_pool();
        void sql_pool();
        // 日志相关
        void log_system();

        void event_listen();
        task event_loop();
    private:
        // 为每一个用户设置一个计时器
        void timer(int connfd, struct sockaddr_in client_address);
        void adjust_timer(timeouts::timer_node *timer);
        void deal_timer(timeouts::timer_node *timer, int sockfd);

        // loop
        // 接收用户连接，并设置计时器
        // bool deal_clientdata();
        bool deal_signal(bool &timeout, bool &stop_server);
        void deal_read(int sockfd);
        void deal_write(int sockfd);

    public:
        // 基础
        int web_port;
        std::string fileroot;
        int web_pipefd[2];
        int web_epfd;
        http::http_conn *http_users;

        // 数据库相关
        connsql::connection_pool *connpool;
        std::string sql_host;
        std::string sql_user;
        std::string sql_passwd;
        std::string sql_db;
 

        // 线程池相关
        thread::thread_pool<std::function<void()> > threadpool;

        
        // epoll_event相关
        epoll_event events[MAX_EVENT_NUMBER];
        int serv_sock;

        // 定时器相关
        timeouts::client_data *users_timer;
        timeouts::utils timer_ctrl;
    };

    struct client_awaitable{
        bool await_ready() const noexcept { return lfd == datafd; }
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept { minilog::log_trace("accept_suspend..."); }
        int await_resume() const noexcept { 
            minilog::log_trace("new client");
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept(lfd, (struct sockaddr *)&client_address, &client_addrlength);
            return connfd;
        }
        client_awaitable operator co_await() { return *this; }
        int lfd, datafd;
    };

    webserver::webserver() {
        http_users = new http::http_conn[MAX_FD];
        fileroot = std::string("../res");
        users_timer = new timeouts::client_data[MAX_FD];
    }

    webserver::~webserver() {
        close(web_epfd);
        close(serv_sock);
        close(web_pipefd[1]);
        close(web_pipefd[0]);
        delete[] http_users;
        delete[] users_timer;
    }

    void webserver::init() {
        std::ifstream ifsJson("./webconfig.json", std::ifstream::in);
        Json::Reader rd;
        Json::Value jroot;
        rd.parse(ifsJson, jroot);
        web_port = jroot["server_port"].asInt();
        sql_host = jroot["host"].asString();
        sql_user = jroot["user"].asString();
        sql_passwd = jroot["passwd"].asString();
        sql_db = jroot["db"].asString();
    }

    void webserver::thread_pool() {
    }

    void webserver::sql_pool() {
        connpool = connsql::connection_pool::get_instance();
        connpool->init_conn(sql_host, sql_user, sql_passwd, sql_db);
        http_users->init_mysql_result(connpool);
    }

    // 日志相关
    void webserver::log_system() {
        minilog::set_log_level(minilog::log_level::trace);
        minilog::set_log_file("./info.log");
    }

    void webserver::event_listen() {
        serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        // 端口复用 以及 优雅断开连接
        int opt = 1;
        if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            throw std::runtime_error("re_use error\n");
        // 关闭连接时，会阻塞close，直到数据发送完毕
        // 超时时间为1s
        // 类似于心跳包
        struct linger tmp = {1, 1};
        setsockopt(serv_sock, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

        // 禁用Nagle算法，提高传输速率
        // int opt_val = 1;
        // setsockopt(_serv_sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));

        // 绑定 以及 监听
        struct sockaddr_in serv_adr;
        memset(&serv_adr, 0, sizeof serv_adr);
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_adr.sin_port = htons(web_port);
        check_error(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof serv_adr), "bind() error", loc.file_name(), loc.line());
        check_error(listen(serv_sock, SIZE), "listen() error", loc.file_name(), loc.line());

        timer_ctrl.init(TIMESLOT);
        epoll_event events[MAX_EVENT_NUMBER];
        web_epfd = epoll_create(SIZE);

        timer_ctrl.addfd(web_epfd, serv_sock, false);
        http::http_conn::http_epfd = web_epfd;

        //UNIX域流套接字
        socketpair(PF_UNIX, SOCK_STREAM, 0, web_pipefd);
        timer_ctrl.set_nonblocking(web_pipefd[1]);
        timer_ctrl.addfd(web_epfd, web_pipefd[0], false);

        timer_ctrl.addsig(SIGPIPE, SIG_IGN);
        // 发送signal
        timer_ctrl.addsig(SIGALRM, timer_ctrl.sig_handler, false);
        timer_ctrl.addsig(SIGTERM, timer_ctrl.sig_handler, false);
        timer_ctrl.addsig(SIGTSTP, timer_ctrl.sig_handler, false);
        timer_ctrl.addsig(SIGINT, timer_ctrl.sig_handler, false);

        // 预约5s后alarm一次
        alarm(TIMESLOT);

        timeouts::utils::u_pipefd = web_pipefd;
        timeouts::utils::u_epfd = web_epfd;
    }

    task webserver::event_loop() {
        bool timeout = false;
        bool stop_server = false;

        while (!stop_server) {
            int event_cnt = epoll_wait(web_epfd, events, MAX_EVENT_NUMBER, -1);

            for (int i = 0; i < event_cnt; i ++) {
                int sockfd = events[i].data.fd;

                // 处理新到的客户连接，绑定计时器
                if (sockfd == serv_sock) {
                    struct sockaddr_in client_address;
                    int connfd = co_await client_awaitable(sockfd, serv_sock);
                    if (connfd < 0) minilog::log_error("accept error");
                    if (http::http_conn::users_count >= MAX_FD) timer_ctrl.show_error(connfd, "Internal server busy");
                    timer(connfd, client_address);
                }
                else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    // 半关闭状态, 浏览器那边关闭，
                    // 服务器端关闭用户连接，移除对应的定时器
                    minilog::log_trace("client close");
                    timeouts::timer_node *timer = users_timer[sockfd].client_timer;
                    deal_timer(timer, sockfd);
                }
                // 处理信号
                else if ((sockfd == web_pipefd[0]) && (events[i].events & EPOLLIN)) {
                    minilog::log_debug("get a signal");
                    bool flag = deal_signal(timeout, stop_server);
                    if (false == flag) minilog::log_error("deal_signal error");
                }
                // 处理客户连接上接收到的数据
                else if (events[i].events & EPOLLIN) {
                    minilog::log_debug("read data");
                    deal_read(sockfd);
                }
                else if (events[i].events & EPOLLOUT) {
                    minilog::log_debug("write data");
                    deal_write(sockfd);
                }
            }
            if (timeout) {
                minilog::log_debug("timeout");
                timer_ctrl.timer_handler();
                timeout = false;
            }
        }
        co_return; 
    }

    // bool webserver::deal_clientdata() {
    //     struct sockaddr_in client_address;
    //     socklen_t client_addrlength = sizeof(client_address);
    //     int connfd = accept(_serv_sock, (struct sockaddr *)&client_address, &client_addrlength);
    //     if (connfd < 0) {
    //         minilog::log_error("accept error");
    //         return false;
    //     }
    //     if (http::http_conn::_users_count >= MAX_FD) {
    //         _util.show_error(connfd, "Internal server busy");
    //         return false;
    //     }
    //     timer(connfd, client_address);
    //     return true;
    // }

    bool webserver::deal_signal(bool &timeout, bool &stop_server) {
        int ret = 0;
        int sig;
        char signals[1024];
        ret = recv(web_pipefd[0], signals, sizeof(signals), 0);
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
                    case SIGINT:
                    {
                        stop_server = true;
                        connpool->~connection_pool();
                        break;
                    }
                }
            }
        }
        return true;
    }

    void webserver::deal_read(int sockfd) {
        timeouts::timer_node *timer = users_timer[sockfd].client_timer;
        // proactor
        if (http_users[sockfd].read_once()) {
            // 若监测到读事件，将该事件放入请求队列
            threadpool.submit([&]() {
                connsql::connRAII connraii{connpool};
                http_users[sockfd].mysql_conn = connraii.conn;
                http_users[sockfd].process();
            });

            if (timer) adjust_timer(timer);
        }
        else deal_timer(timer, sockfd);
    }

    void webserver::deal_write(int sockfd) {
        timeouts::timer_node *timer = users_timer[sockfd].client_timer;
        // proactor
        if (http_users[sockfd].write())
            if (timer) adjust_timer(timer);

        else deal_timer(timer, sockfd);
    }

    void webserver::timer(int connfd, struct sockaddr_in client_address) {
        http_users[connfd].init(connfd, client_address, fileroot);

        // 初始化client_data数据
        // 创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
        users_timer[connfd].address = client_address;
        users_timer[connfd].sockfd = connfd;

        // timer_node *timer = new timer_node;
        timeouts::timer_node *timer = new timeouts::timer_node;
        timer->user_data = &users_timer[connfd];
        timer->func = timeouts::cb_func;
        time_t cur = timeouts::timer::get_tick();
        timer->expire = cur + 3 * TIMESLOT;

        users_timer[connfd].client_timer = timer;
        timer_ctrl.util_timer.add_timer(*timer);
    }

    void webserver::adjust_timer(timeouts::timer_node *timer) {
        time_t cur = timeouts::timer::get_tick();
        timer->expire = cur + 3 * TIMESLOT;
    }

    // 关闭用户连接
    void webserver::deal_timer(timeouts::timer_node *timer, int sockfd) {
        timer->func(*timer);
        if (timer) timer_ctrl.util_timer.del_timer(*timer);
    }
}
#endif

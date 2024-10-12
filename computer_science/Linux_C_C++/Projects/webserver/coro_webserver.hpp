#include <functional>
#include <source_location>
#include <memory>
#include <iostream>
#include <format>


#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>


#include <coro_promise.hpp>
#include <thread_pool.hpp>
#include <connection_pool.hpp>
#include <memory_pool.hpp>
#include <co_timer.hpp>
#include <minilog.hpp>

namespace co_async {
    namespace web_details {
        const int MAX_FD = 65536;
        const int MAX_EVENT_NUMBER = 10000;
        const int TIMESLOT = 5;
        const int SIZE = 100;

        using Allocater = memory::default_alloc;
        static thread::thread_pool<std::function<void()> > web_threadpool;
        static connsql::connection_pool *web_connpool;
        static Allocater web_alloc;
        static timer timer_ctrl;

        int web_epfd;
        int web_pipefd[2];
        int serv_sock;
        std::string fileroot;
        http::http_conn *http_users;
        co_async::client_data *users_timer;

        int web_port;
        std::string sql_host;
        std::string sql_user;
        std::string sql_passwd;
        std::string sql_db;
        epoll_event events[MAX_EVENT_NUMBER];

        int set_nonblocking(int fd) {
            int old_option = fcntl(fd, F_GETFL);
            fcntl(fd, F_SETFL, old_option | O_NONBLOCK);
            return old_option;
        }

        void addfd(int epfd, int fd, bool one_shot) {
            epoll_event event;

            event.data.fd = fd;
            event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
            if (one_shot)
                event.events |= EPOLLONESHOT;

            epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
            set_nonblocking(fd);
        }

        void sig_handler(int sig) {
            int save_errno = errno;
            int msg = sig;
            // 发送signal
            send(web_pipefd[1], (char *)&msg, 1, 0);
            errno = save_errno;
        }

        void addsig(int sig, void(handler)(int), bool restart = true) {
            struct sigaction sa;
            memset(&sa, '\0', sizeof(sa));
            sa.sa_handler = handler;
            // 系统调用被打断后，可以自动重启这个调用
            if (restart)
                sa.sa_flags |= SA_RESTART;
            // 防止 被其他信号打断
            sigfillset(&sa.sa_mask);
            assert(sigaction(sig, &sa, NULL) != -1);
        }

        void timer_handler(timer &tm, int tmout) {
            // 任务全部执行完毕或超时，alarm一次
            // 无限alarm
            while (tm.check_timer());
            alarm(tmout);
        }

        void cb_func(const timer_node& user_node) {
            epoll_ctl(web_epfd, EPOLL_CTL_DEL, user_node.user_data->sockfd, 0);
            assert(user_node.user_data);
            close(user_node.user_data->sockfd);
        }

        void check_error(int err, std::string express) {
            if (-1 == err) minilog::log_fatal("{}", express);
        }

        void giveaway_timer(int connfd, struct sockaddr_in client_address) {
            http_users[connfd].init(connfd, client_address, fileroot);

            // 初始化client_data数据
            // 创建定时器，设置回调函数和超时时间，绑定用户数据，将定时器添加到链表中
            users_timer[connfd].address = client_address;
            users_timer[connfd].sockfd = connfd;

            // timer_node *timer = new timer_node;
            timer_node *timer = new timer_node;
            timer->user_data = &users_timer[connfd];
            timer->func = cb_func;
            time_t cur = timer::get_tick();
            timer->expire = cur + 3 * TIMESLOT;

            users_timer[connfd].client_timer = timer;
            timer_ctrl.add_timer(*timer);
        }
        
        void adjust_timer(timer_node *timer) {
            time_t cur = timer::get_tick();
            timer->expire = cur + 3 * TIMESLOT;
        }

        void deal_timer(timer_node *timer, int sockfd) {
            timer->func(*timer);
            if (timer) timer_ctrl.del_timer(*timer);
        }
    }



    using namespace web_details;
    class webserver {
    public:
        webserver();
        // Json序列化
        void init_jsonfile();
        // 日志
        void init_logsystem();
        // 线程池
        void init_threadpool();
        // 连接池
        void init_connectionpool();
        // 内存池
        void init_memorypool();
        // listen
        void eventlisten();
        // loop
        task eventloop();
        ~webserver(); 
    };


    struct client_accept {
        using promise_type = Promise;

        bool await_ready() const noexcept { return !(lfd == evfd); }
        void await_resume() const noexcept {} 
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            minilog::log_debug("accept the client");
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = 0;
            check_error(connfd = accept(lfd, (struct sockaddr *)&client_address, &client_addrlength), "accept failed");
            giveaway_timer(connfd, client_address);
        }

        client_accept operator co_await() { return *this; }
        int lfd, evfd;
    };


    struct client_read {
        using promise_type = Promise;

        bool await_ready() const noexcept { return !((ev.events & EPOLLIN) && ev.data.fd != web_pipefd[0] && ev.data.fd != serv_sock); }
        void await_resume() const noexcept {}
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            minilog::log_debug("get a read signal");
            timer_node *timer = users_timer[ev.data.fd].client_timer;
            // proactor
            if (http_users[ev.data.fd].read_once()) {
                // 若监测到读事件，将该事件放入请求队列
                web_threadpool.submit([&]() {
                    connsql::connRAII connraii{web_connpool};
                    http_users[ev.data.fd].mysql_conn = connraii.conn;
                    http_users[ev.data.fd].process();
                });

                if (timer) adjust_timer(timer);
            }
            else deal_timer(timer, ev.data.fd);
        }

        client_read operator co_await() { return *this; }
        epoll_event ev;
    };


    struct client_write {
        using promise_type = Promise;

        bool await_ready() const noexcept { return !(ev.events & EPOLLOUT); }
        void await_resume() const noexcept {}
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            minilog::log_debug("get a write signal");
            timer_node *timer = users_timer[ev.data.fd].client_timer;
            // proactor
            if (http_users[ev.data.fd].write())
                if (timer) adjust_timer(timer);

            else deal_timer(timer, ev.data.fd);
        }

        client_write operator co_await() { return *this; }
        epoll_event ev;
    };

    struct half_close {
        using promise_type = Promise;

        bool await_ready() const noexcept { return !(ev.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)); }
        void await_resume() const noexcept {}
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            minilog::log_debug("half close");
            timer_node *timer = users_timer[ev.data.fd].client_timer;
            deal_timer(timer, ev.data.fd);
        }

        half_close operator co_await() { return *this; }
        epoll_event ev;
    };

    struct catch_signal {
        using promise_type = Promise;

        bool await_ready() const noexcept { return !(ev.data.fd == web_pipefd[0]) && (ev.events & EPOLLIN); }
        void await_resume() const noexcept {} 
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            int ret = 0, sig = 0;
            char signals[1024];
            check_error(ret = recv(web_pipefd[0], signals, sizeof(signals), 0), "can not handle this signal");
            for (int i = 0; i < ret; ++ i) {
                switch (signals[i])
                {
                    case SIGALRM:
                    {
                        timeout = true;
                        break;
                    }
                    case SIGTERM:
                    {
                        stopserver = true;
                        break;
                    }
                    case SIGTSTP:
                    {
                        stopserver = true;
                        break;
                    }
                    case SIGINT:
                    {
                        stopserver = true;
                        web_connpool->~connection_pool();
                        break;
                    }
                }
            }
        }

        catch_signal operator co_await() { return *this; }
        epoll_event ev;
        bool &stopserver, &timeout;
    };

    struct timeout_conn {
        using promise_type = Promise;

        bool await_ready() const noexcept { return !timeout; }
        void await_resume() const noexcept {} 
        void await_suspend(std::coroutine_handle<> coroutine) const noexcept {
            minilog::log_debug("conn time out");
            timer_handler(timer_ctrl, TIMESLOT);
            timeout = false;
        }

        timeout_conn operator co_await() { return *this; }
        bool &timeout;
    };


    webserver::webserver() {
        http_users = new http::http_conn[MAX_FD];
        fileroot = std::string("../res");
        users_timer = new client_data[MAX_FD];
    }

    void webserver::init_jsonfile() {
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

    void webserver::init_logsystem() {
        minilog::set_log_level(minilog::log_level::info);
        minilog::set_log_file("./info.log");
    }

    void webserver::init_threadpool() {}

    void webserver::init_connectionpool() {
        web_connpool = connsql::connection_pool::get_instance();
        web_connpool->init_conn(sql_host, sql_user, sql_passwd, sql_db);
        http_users->init_mysql_result(web_connpool);
    }

    void webserver::init_memorypool() {}

    void webserver::eventlisten() {
        serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        // 端口复用 以及 优雅断开连接
        int opt = 1;
        check_error(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "reuse failed");
        
        struct linger tmp = {1, 1};
        check_error(setsockopt(serv_sock, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp)), "close linger failed");

        // 绑定 以及 监听
        struct sockaddr_in serv_adr;
        memset(&serv_adr, 0, sizeof serv_adr);
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_adr.sin_port = htons(web_port);
        check_error(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof serv_adr), "bind() error");
        check_error(listen(serv_sock, SIZE), "listen() error");

        check_error(web_epfd = epoll_create(1), "epool create failed");
    
        addfd(web_epfd, serv_sock, false);
        http::http_conn::http_epfd = web_epfd;
        check_error(socketpair(PF_UNIX, SOCK_STREAM, 0, web_pipefd), "get pipe failed");
        set_nonblocking(web_pipefd[1]);
        addfd(web_epfd, web_pipefd[0], false);

        // 发送signal
        addsig(SIGPIPE, SIG_IGN);
        addsig(SIGALRM, sig_handler, false);
        addsig(SIGTERM, sig_handler, false);
        addsig(SIGTSTP, sig_handler, false);
        addsig(SIGINT, sig_handler, false);

        // 预约5s后alarm一次
        alarm(TIMESLOT);
    }
    
    task webserver::eventloop() {
        bool stopserver = false, timeout = false;
        while (!stopserver) {
            int event_cnt = epoll_wait(web_epfd, events, MAX_EVENT_NUMBER, -1);
            for (int i = 0; i < event_cnt; ++ i) {
                int sockfd = events[i].data.fd;
                co_await client_accept(serv_sock, sockfd);
                co_await client_read(events[i]);
                co_await client_write(events[i]);
                co_await half_close(events[i]); 
                co_await catch_signal(events[i], stopserver, timeout);
                co_await timeout_conn(timeout);
            }
        }
        co_return 0;
    }

    webserver::~webserver() {
        close(web_epfd);
        close(serv_sock);
        close(web_pipefd[1]);
        close(web_pipefd[0]);
        delete[] http_users;
        delete[] users_timer;
    }
}

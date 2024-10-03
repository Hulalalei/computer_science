#ifndef TIMER
#define TIMER


#include <iostream>
#include <set>
#include <chrono>
#include <functional>
#include <ctime>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>

#include <http_conn.hpp>
#include <minilog.hpp>

namespace timeouts {
    struct client_data;
    struct timer_node_base {
        // 过期时间
        time_t expire;
        int id;
    };

    struct timer_node : timer_node_base {
        using callback = std::function<void (const timer_node&)>;
        callback func;
        client_data *user_data;
    };

    bool operator <(const timer_node_base& lhs, const timer_node_base& rhs) {
        if (lhs.expire > rhs.expire) return false;
        else if (lhs.expire < rhs.expire) return true;
        else return lhs.id < rhs.id;
    }

    class timer {
    public:
        static time_t get_tick();
        timer_node_base add_timer(timer_node &node);
        bool del_timer(timer_node_base &node);
        bool check_timer();
        time_t time_to_sleep();
        int get_id() { return ++ gid; }
    private:
        static int gid;
        std::set<timer_node, std::less<> > timer_map;
    };

    struct client_data {
        sockaddr_in address;
        int sockfd;
        timer_node *client_timer;
    };

    class utils {
    public:
        utils() {}
        ~utils() {}

        void init(int timeslot);

        int set_nonblocking(int fd);
        void addfd(int epfd, int fd, bool oneshot);

        static void sig_handler(int sig);
        void addsig(int sig, void(handler)(int), bool restart = true);
        void timer_handler();
        void show_error(int connfd, const std::string info);

    public:
        static int *u_pipefd;
        timer util_timer;
        static int u_epfd;
        int u_TIMESLOT;
    };

    void cb_func(const timer_node& user_node);


    int timer::gid = 0;

    time_t timer::get_tick() {
        minilog::log_debug("get_tick");
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    timer_node_base timer::add_timer(timer_node &node) {
        node.id = get_id();

        timer_map.insert(node);
        return static_cast<timer_node_base>(node);
    }

    bool timer::del_timer(timer_node_base &node) {
        auto iter = timer_map.find(node);
        if (iter != timer_map.end()) {
            timer_map.erase(iter);
            return true;
        }
        return false;
    }

    bool timer::check_timer() {
        minilog::log_debug("check timer");
        auto iter = timer_map.begin();
        if (iter != timer_map.end() && iter->expire <= get_tick()) {
            iter->func(*iter);
            timer_map.erase(iter);
            return true;
        }
        return false;
    }

    time_t timer::time_to_sleep() {
        auto iter = timer_map.begin();
        if (iter == timer_map.end()) return -1;
        time_t dist = iter->expire - get_tick();
        return dist > 0 ? dist : 0;
    }

    void utils::init(int timeslot) {
        u_TIMESLOT = timeslot;
    }

    int utils::set_nonblocking(int fd) {
        int old_option = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, old_option | O_NONBLOCK);
        return old_option;
    }

    void utils::addfd(int epfd, int fd, bool one_shot) {
        epoll_event event;

        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        if (one_shot)
            event.events |= EPOLLONESHOT;

        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
        set_nonblocking(fd);
    }

    void utils::sig_handler(int sig) {
        minilog::log_debug("sig handler");
        int save_errno = errno;
        int msg = sig;
        // 发送signal
        send(u_pipefd[1], (char *)&msg, 1, 0);
        errno = save_errno;
    }

    void utils::addsig(int sig, void(handler)(int), bool restart) {
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

    void utils::timer_handler() {
        minilog::log_debug("timer handler");
        // 任务全部执行完毕或超时，alarm一次
        // 无限alarm
        while (util_timer.check_timer());
        alarm(u_TIMESLOT);
    }

    void utils::show_error(int connfd, const std::string info) {
        send(connfd, &info[0], info.size(), 0);
        close(connfd);
    }

    int *utils::u_pipefd = 0;
    int utils::u_epfd = 0;


    void cb_func(const timer_node& user_node) {
        minilog::log_debug("callback");
        epoll_ctl(utils::u_epfd, EPOLL_CTL_DEL, user_node.user_data->sockfd, 0);
        assert(user_node.user_data);
        close(user_node.user_data->sockfd);
        http::http_conn::users_count --;
    }
}
#endif

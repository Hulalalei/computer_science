#include <coroutine>
#include <fstream>
#include <format>
#include <source_location>
#include <stdexcept>

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <csignal>
#include <unistd.h>
#include <json/json.h>
#include <mysql/mysql.h>

#include <debug.hpp>


void signal_handler(int signum) {
    debug(), "catch the ctrl + c signal";
    exit(0);
}

struct Promise {
    // 控制main中协程函数逻辑
    // 返回值也是可改变的: always or never
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { throw; }
    // 决定是否让出CPU权: always or never
    std::suspend_always yield_value(int ret) { m_val = ret; return {}; }
    void return_void() { m_val = 0; }
    // void return_value(int ret) { m_val = ret; }
    std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }

    int m_val;
};


struct accept_awaitable{
    bool await_ready() const noexcept { return lfd == datafd; }
    void await_suspend(std::coroutine_handle<> coroutine) const noexcept { debug(), "accept_suspend..."; }
    int await_resume() const noexcept { 
        char ip[32];
        struct sockaddr_in caddr;

        socklen_t len = static_cast<socklen_t>(sizeof(caddr));
        int cfd = accept(lfd, (struct sockaddr *)&caddr, &len);
        if (cfd == -1) std::cerr << "accept err" << std::endl;

        std::cout << "client's ip = " << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
                  << ", port = " << ntohs(caddr.sin_port) << " has join the chating-room" << std::endl;
        
        return cfd;
    }

    accept_awaitable operator co_await() { return *this; }
    int lfd, datafd;
};

struct task {
    using promise_type = Promise;
    std::coroutine_handle<promise_type> m_co;
    task(std::coroutine_handle<promise_type> coroutine): m_co(coroutine) {}
    task(task &&) = delete;
    ~task() { m_co.destroy(); }

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> coroutine) const noexcept { }
    int await_resume() const noexcept { return 10; }
};

struct EventLoop {
    int epfd;
};


task net_commute(int &epfd, int &lfd) {
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    struct epoll_event evs[1024];//传出参数
	int size = sizeof (evs) / sizeof (evs[0]);

    while (true) {
        int ret = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < ret; i++) {
			int fd = evs[i].data.fd;
			if (fd == lfd) {
				int cfd = co_await accept_awaitable(lfd, fd);

				struct epoll_event evtemp;
				evtemp.events = EPOLLIN;
				evtemp.data.fd = cfd;
				epoll_ctl (epfd, EPOLL_CTL_ADD, cfd, &evtemp);
			} else {
				// char buff[1024 * 5];
				// int rlens = recv(fd, buff, sizeof (buff), 0);
				// if (rlens > 0)
				// 	std::cout << "client " << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
				// 			  << " says: " << buff << std::endl;
				// if (rlens == 0) {
				// 	std::cout << "client" << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
				// 			  << " has cut the connection" << std::endl;
				// 	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
				// 	close (fd);
				// }
			}
		}

    }
}


void check_error(int signum, int pos) {
    if (signum == -1) {
        std::cout << std::format("num: {}, pos: {}, error\n", signum, pos);
        exit(0);
    }
}


// 127.0.0.1 : 12000
int main (int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(lfd, 1);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (12000);
	saddr.sin_addr.s_addr = INADDR_ANY;
	check_error(bind(lfd, (struct sockaddr*)&saddr, sizeof (saddr)), 2);
    check_error(listen (lfd, 128), 3);

    int epfd = epoll_create(1);
    // task t = net_commute(epfd, lfd);
    // while (!t.m_co.done()) {
    //     t.m_co.resume();
    // }
    
    std::ifstream ifsJson("./webconfig.json", std::ifstream::in);
    if (!ifsJson) throw std::runtime_error("err");
    else std::cout << std::format("ok\n");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifsJson, root);
    std::cout << root["server_port"].asInt();
    std::cout << root["host"].asString();
    std::cout << root["user"].asString();
    root["passwd"].asString();
    root["db"].asString();
    return 0;
}

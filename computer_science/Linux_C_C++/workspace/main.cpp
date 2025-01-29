#include <asm-generic/socket.h>
#include <cerrno>
#include <csignal>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <print>
#include <array>

#include <expected.hpp>
#include <io_context.hpp>
#include <callback.hpp>
#include <timer_context.hpp>



void handle_ctrl(int sig) {
    std::println("catch the ctrl + C signal");
    throw;
}

// void handle_pipe(int sig) {
//     std::println("catch the pipe");
//     throw;
// }


int main(int argc, char **argv) {
    callback<> cb;
    signal(SIGINT, handle_ctrl);
    // signal(SIGPIPE, handle_pipe);

    address_resolver resolver;
    auto adrinfo = resolver.resolve("127.0.0.1", "8080");
    int sockfd = adrinfo.create_socket();
    auto serv_addr = adrinfo.get_address();

    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof on);
    convert_error(bind(sockfd, serv_addr.m_addr, serv_addr.m_addrlen)).expect("bind");
    convert_error(listen(sockfd, SOMAXCONN)).expect("listen");


    int epfd = convert_error(epoll_create(1)).expect("epoll_create");
    std::array<struct epoll_event, 128> events;
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);




    std::println("start the server now");
    while (true) {
        int ret = convert_error(epoll_wait(epfd, events.data(), events.size(), -1))
            .expect("pwait2");
        for (int i = 0; i < ret; i ++) {
            int fd = events[i].data.fd;
            if (fd == sockfd) {
                int connfd = convert_error(accept(sockfd, nullptr, nullptr)).expect("accept");
                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            } else {
                char buf[1024];
                auto exp_read = convert_error(read(fd, buf, sizeof buf));
                if (exp_read.is_error(ECONNRESET)) continue;
                ssize_t rn = exp_read.expect("read");
                
                auto req = std::string(buf, rn);
                // std::println("receive the requset:\n{}", req);
                std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\r\nConnection: keep-alive\r\n\r\nhello, world";
                // std::println("\nmy resonse is: {}", response);
                auto exp_write = convert_error<size_t>(write(fd, response.data(), response.size()));
                if (exp_write.is_error(EPIPE)) continue;
                ssize_t wn = exp_write.expect("write");

                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                close(fd);
            }
        }
    }
    close(sockfd);
    return 0;
}

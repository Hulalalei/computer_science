#include <iostream>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>

//关于端口无法重复使用问题：套接字未关闭.
int main (void) {
	int lfd = socket (AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) std::cerr << "init err" << std::endl;


	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (10003);
	saddr.sin_addr.s_addr = INADDR_ANY;		//client's ip
	int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "bind err" << std::endl;

	ret = listen (lfd, 128);
	if (ret == -1) std::cerr << "listen err" << std::endl;

    struct pollfd fds[1024];
    for (int i = 0; i < 1024; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;
    int maxfd = 0;

    while (true) {
        ret = poll (fds, maxfd + 1, -1);
        struct sockaddr_in caddr;
        char ip[32];

        if (fds[0].revents & POLLIN) {
            std::cout << "accepting...\n";
            socklen_t len = static_cast<socklen_t>(sizeof(caddr));
            int cfd = accept(lfd, (struct sockaddr *)&caddr, &len);
            if (cfd == -1) std::cerr << "accept err" << std::endl;
            int j = 0;
            for (j = 0; j < 1024; j++) {
                if (fds[j].fd == -1) {
                    fds[j].fd = cfd;
                    break;
                }
            }
            maxfd = maxfd > j ? maxfd : j;
            std::cout << "client's ip = " << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
            << ", port = " << ntohs(caddr.sin_port) << " has join the chating-room" << std::endl;
        }

        for (int j = 0; j <= maxfd; j++) {
            if (fds[j].revents & POLLIN) {
                std::cerr << "ok\n";
                char buff[1024 * 5];
                int rlens = recv(fds[j].fd, buff, sizeof(buff), 0);
                if (rlens > 0)
                    std::cout << "client " << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
                    << " says: " << buff << std::endl;
                if (rlens == 0) {
                    std::cout << "client" << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
                    << " has cut the connection" << std::endl;
                    fds[j].fd = -1;
                    close(fds[j].fd);
                    break;
                }
                if (rlens == -1) std::cerr << "receive error\n";
                sleep (1);
            }
        }
    }
	close (lfd);
}

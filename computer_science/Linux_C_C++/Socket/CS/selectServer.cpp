#include <iostream>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <sys/select.h>

//关于端口无法重复使用问题：套接字未关闭.
int main (void) {
	int lfd = socket (AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) std::cerr << "init err" << std::endl;


	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (11006);
	saddr.sin_addr.s_addr = INADDR_ANY;		//client's ip
	int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "bind err" << std::endl;

	ret = listen (lfd, 128);
	if (ret == -1) std::cerr << "listen err" << std::endl;

    fd_set redset;
    FD_ZERO (&redset);
    FD_SET (lfd, &redset);
    int maxfd = lfd;

    while (true) {
        fd_set temp = redset;
        std::cout << "has been covered...\n";
        select (maxfd + 1, &temp, nullptr, nullptr, nullptr);//err
        std::cout << "searching...\n";
        struct sockaddr_in caddr;
        char ip[32];

        if (FD_ISSET (lfd, &temp)) {
            std::cout << "accepting...\n";
            socklen_t len = static_cast<socklen_t>(sizeof(caddr));
            int cfd = accept(lfd, (struct sockaddr *)&caddr, &len);
            if (cfd == -1) std::cerr << "accept err" << std::endl;

            std::cout << "client's ip = " << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
            << ", port = " << ntohs(caddr.sin_port) << " has join the chating-room" << std::endl;

            FD_SET (cfd, &redset);
            maxfd = maxfd > cfd ? maxfd : cfd;
        }

        for (int i = 0; i <= maxfd; i++) {
            if (i != lfd && FD_ISSET (i, &temp)) {
                std::cout << "chating...\n";
                char buff[1024 * 5];
                int rlens = recv(i, buff, sizeof(buff), 0);
                if (rlens > 0)
                    std::cout << "client " << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
                    << " says: " << buff << std::endl;
                if (rlens == 0) {
                    std::cout << "client" << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
                    << " has cut the connection" << std::endl;
                    FD_CLR(i, &redset);
                    close(i);
                    break;
                }
            }
        }

    }

	close (lfd);
}

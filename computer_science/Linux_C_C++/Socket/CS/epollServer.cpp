#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

//关于端口无法重复使用问题：套接字未关闭.
int main (void) {
	int lfd = socket (AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) std::cerr << "init err" << std::endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (10004);
	saddr.sin_addr.s_addr = INADDR_ANY;		//client's ip
	int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "bind err" << std::endl;

	ret = listen (lfd, 128);
	if (ret == -1) std::cerr << "listen err" << std::endl;

	int epfd = epoll_create (true);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	epoll_ctl (epfd, EPOLL_CTL_ADD, lfd, &ev);

	struct epoll_event evs[1024];//传出参数
	int size = sizeof (evs) / sizeof (evs[0]);
	char ip[32];
	struct sockaddr_in caddr;

	/*创建的通信或者监听fd, 均是读取操作*/
	while (true) {
		int num = epoll_wait (epfd, evs, size, -1);
		for (int i = 0; i < num; i++) {
			int fd = evs[i].data.fd;
			if (fd == lfd) {
				socklen_t len = static_cast<socklen_t>(sizeof(caddr));
				int cfd = accept(fd, (struct sockaddr *)&caddr, &len);
				if (cfd == -1) std::cerr << "accept err" << std::endl;

				std::cout << "client's ip = " << inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
						  << ", port = " << ntohs(caddr.sin_port) << " has join the chating-room" << std::endl;
				
				struct epoll_event evtemp;
				evtemp.events = EPOLLIN;
				evtemp.data.fd = cfd;
				epoll_ctl (epfd, EPOLL_CTL_ADD, cfd, &evtemp);
			} else {
				char buff[1024 * 5];
				int rlens = recv(fd, buff, sizeof (buff), 0);
				if (rlens > 0)
					std::cout << "client " << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
							  << " says: " << buff << std::endl;
				if (rlens == 0) {
					std::cout << "client" << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip))
							  << " has cut the connection" << std::endl;
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
					close (fd);
				}
			}
		}
	}
	close (lfd);
	return 0;
}

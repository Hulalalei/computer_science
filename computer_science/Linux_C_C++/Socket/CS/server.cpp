#include <iostream>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

//关于端口无法重复使用问题：套接字未关闭.
int main (void) {
	int fd = socket (AF_INET, SOCK_STREAM, 0);
	if (fd == -1) std::cerr << "init err" << std::endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (10003);
	saddr.sin_addr.s_addr = INADDR_ANY;		// os会自动选择服务器ip
	int ret = bind(fd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "bind err" << std::endl;

	ret = listen (fd, 128); // TCP三握手中，服务器设置为LISTEN状态
	if (ret == -1) std::cerr << "listen err" << std::endl;

	struct sockaddr_in caddr;
	socklen_t len = static_cast<socklen_t> (sizeof (caddr));
	int cfd = accept (fd, (struct sockaddr*)&caddr, &len);
	if (cfd == -1) std::cerr << "accept err" << std::endl;

	char ip[32];
	std::cout << "client's ip = " << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof (ip))
				<< ", port = " << ntohs (caddr.sin_port) << " has join the chating-room" << std::endl;

	while (true) {
		char buff[1024 * 5];
		int rlens = recv (cfd, buff, sizeof (buff), 0);
		if (rlens > 0) std::cout << "client " << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof (ip)) 
						<< " says: " << buff << std::endl;
		if (rlens == 0) {
			std::cout << "client" << inet_ntop (AF_INET, &caddr.sin_addr.s_addr, ip, sizeof (ip)) 
						<< " has cut the connection" << std::endl;
			break;
		}
	}

	close (fd);
	close (cfd);
}

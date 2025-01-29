#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <pthread.h>

pthread_mutex_t mutexComm;

void *getCast (void *arg) {
	int *cfd = static_cast<int*> (arg);
	while (true) {
		//接收池子中的数据
		char *buffs = new char[1024];
		pthread_mutex_lock (&mutexComm);
		int rlen = recv (*cfd, buffs, 1024, 0);
		pthread_mutex_unlock (&mutexComm);
		if (rlen == 0) {
			std::cerr << "client receive err....\n";
			break;
		}
		if (rlen == -1) std::cerr << "client receive err...\n";
		if (rlen > 0) {
			std::cout << buffs << "\n";
		}
	}
    return nullptr;
}


int main (void) {
	int cfd = socket (AF_INET, SOCK_STREAM, 0);
	if (cfd == -1) std::cerr << "init err" << std::endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (10004);
	//服务器ip与端口
	inet_pton (AF_INET, "127.0.0.1", &saddr.sin_addr.s_addr);
	int ret = connect (cfd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "connect err" << std::endl;

	pthread_t comm;
	pthread_create (&comm, nullptr, getCast, &cfd);
	while (true) {
		//发送数据
		char *buff = new char[1024];
		std::cout << "输入：" << std::endl;
		fgets (buff, sizeof (buff), stdin);
		send (cfd, buff, 1024, 0);
	}

    close (cfd);
}

#include <iostream>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <sys/select.h>
#include "ThreadPool.h"

pthread_mutex_t mutexComm;

class acceptInfo {
public:
    struct sockaddr_in* caddr;
    int lfd;
    fd_set *redset;
    int *maxfd;
};

void acceptConn (void *arg) {
    acceptInfo *acceptinfo = static_cast<acceptInfo*> (arg);
    while (true) {
        char ip[32];
        std::cout << "accepting...\n";
        socklen_t len = static_cast<socklen_t>(sizeof(*acceptinfo->caddr));
        int cfd = accept(acceptinfo->lfd, (struct sockaddr *)acceptinfo->caddr, &len);
        if (cfd == -1)
            std::cerr << "accept err" << std::endl;

        std::cout << "client's ip = " << inet_ntop(AF_INET, &acceptinfo->caddr->sin_addr.s_addr, ip, sizeof(ip))
                  << ", port = " << ntohs(acceptinfo->caddr->sin_port) << " has join the chating-room" << std::endl;

        pthread_mutex_lock(&mutexComm);
        FD_SET(cfd, acceptinfo->redset);
        std::cout << "cfd = " << cfd << '\n';
        *acceptinfo->maxfd = *acceptinfo->maxfd > cfd ? *acceptinfo->maxfd : cfd;
        pthread_mutex_unlock(&mutexComm);
    }
}

void chating (void *arg) {
    acceptInfo *acceptinfo = static_cast<acceptInfo*> (arg);

    while (true) {
        char ip[32];
        std::cout << "chating...\n";
        char buff[1024 * 5];
        
        int rlens = recv(acceptinfo->lfd, buff, sizeof(buff), 0);
        if (rlens > 0)
            std::cout << "client " << inet_ntop(AF_INET, &acceptinfo->caddr->sin_addr.s_addr, ip, sizeof(ip))
                      << " says: " << buff << std::endl;
        if (rlens == 0)
        {
            std::cout << "client" << inet_ntop(AF_INET, &acceptinfo->caddr->sin_addr.s_addr, ip, sizeof(ip))
                      << " has cut the connection" << std::endl;

            pthread_mutex_lock(&mutexComm);
            FD_CLR(acceptinfo->lfd, acceptinfo->redset);
            close(acceptinfo->lfd);
            pthread_mutex_unlock(&mutexComm);
        }
    }
}

//关于端口无法重复使用问题：套接字未关闭.
int main (void) {
	int lfd = socket (AF_INET, SOCK_STREAM, 0);
	if (lfd == -1) std::cerr << "init err" << std::endl;


	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (14022);
	saddr.sin_addr.s_addr = INADDR_ANY;		//client's ip
	int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "bind err" << std::endl;

    ret = listen (lfd, 128);
	if (ret == -1) std::cerr << "listen err" << std::endl;

    fd_set redset;
    FD_ZERO (&redset);
    FD_SET (lfd, &redset);
    int maxfd = lfd;
    ThreadPool* pool = new ThreadPool {3, 6, 9};
    
    int flag = 0;
    while (true) {
        fd_set temp = redset;
        //std::cout << "has been covered...\n";
        select (maxfd + 1, &temp, nullptr, nullptr, nullptr);//err
        //std::cout << "searching...\n";
        struct sockaddr_in caddr;

        
        if (FD_ISSET (lfd, &temp) && flag == 0) {
            flag = 1;
            acceptInfo* apf = new acceptInfo;
            apf->caddr = &caddr;
            apf->lfd = lfd;

            pthread_mutex_lock(&mutexComm);
            apf->maxfd = &maxfd;
            apf->redset = &redset;
            pthread_mutex_unlock(&mutexComm);

            pool->addTasks (TaskQueue (acceptConn, apf));
            std::cout << "add a task01...\n";
        }
        //std::cout << "one round end...\n";
        for (int i = 0; i <= maxfd; i++) {
            if (i != lfd && FD_ISSET (i, &temp)) {
                std::cout << "i = " << i << '\n';
                FD_CLR(i, &redset);
                acceptInfo *apf = new acceptInfo;
                apf->caddr = &caddr;
                apf->lfd = i;

                pthread_mutex_lock(&mutexComm);
                apf->maxfd = &maxfd;
                apf->redset = &redset;
                pthread_mutex_unlock(&mutexComm);

                pool->addTasks (TaskQueue (chating, apf));
                std::cout << "add a task02...\n";
            }
        }
    }

	close (lfd);
}

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "ThreadPoolPlus.hpp"


//聊天池
static char *msgPool = new char[1024];
static pthread_mutex_t mutexMsgPool;
static pthread_mutex_t mutexComm;
static pthread_cond_t condRead;

static std::vector<int> tub;

class PInfo {
public:
   ThreadPool* p;
   int fd;

   PInfo () {
     this->p = nullptr;
     this->fd = -1;
   }
   PInfo (ThreadPool* l, int fds) {
     this->p = l;
     this->fd = fds;
   }
};

class WorkInfo {
public:
   int cfd;
   sockaddr_in caddr;
   const char *ip;
   char *pt;

   WorkInfo () {}
   WorkInfo (int two) {
     this->cfd = two;
   }
};

void acceptConn (void *arg);
void working (void *arg);
void broadCast ();

int main (void) {
   int fd = socket (AF_INET, SOCK_STREAM, 0);
   if (fd == -1) std::cerr << "init err" << std::endl;

   struct sockaddr_in saddr;
   saddr.sin_family = AF_INET;
   saddr.sin_port = htons (11068);
   saddr.sin_addr.s_addr = INADDR_ANY;  //client's ip
   int ret = bind(fd, (struct sockaddr*)&saddr, sizeof (saddr));
   if (ret == -1) std::cerr << "bind err" << std::endl;

   ret = listen (fd, 128);
   if (ret == -1) std::cerr << "listen err" << std::endl;

   pthread_mutex_init (&mutexMsgPool, nullptr);

   ThreadPool* pool = new ThreadPool (3, 6, 9);
   PInfo* p = new PInfo (pool, fd);
   pool->addTask (acceptConn, p);
   pthread_exit (nullptr);
   delete pool;

   pthread_mutex_destroy (&mutexMsgPool);
}

void acceptConn (void *arg) {
   PInfo *t = static_cast<PInfo*> (arg);
    while (true) {
        WorkInfo* winfo = new WorkInfo;

        socklen_t len = static_cast<socklen_t> (sizeof (sockaddr_in));
        winfo->cfd = accept (t->fd, (struct sockaddr*)&winfo->caddr, &len);

        
        

        if (winfo->cfd == -1) {
             std::cerr << "accept err" << std::endl;
        }
        else {
            //存储cfd
            tub.push_back (winfo->cfd);
            t->p->addTask (working, winfo);
        }
    }
}

void working (void *arg) {
    WorkInfo* winfo = static_cast<WorkInfo*> (arg);
    char ip[32];
    const char *pt = inet_ntop (AF_INET, &winfo->caddr.sin_addr.s_addr, ip, sizeof (ip));
    int ports = ntohs (winfo->caddr.sin_port);
   
    std::cout << "client's ip = " << pt << ", port = " << ports << " has join the chating-room" << std::endl;
   
   
    //往池子中添加数据
    pthread_mutex_lock (&mutexMsgPool);
    sprintf (msgPool, "client's ip = %s , port = %d has join the chating-room\n", pt, ports);
    //广播
    broadCast ();
    pthread_mutex_unlock (&mutexMsgPool);

    while (true) {
        char *buff = new char[1024];
        int rlens = recv (winfo->cfd, buff, 1024, 0);
        if (rlens > 0) {
            // std::cout << "client's ip = " << inet_ntop (AF_INET, &winfo->caddr.sin_addr.s_addr, ip, sizeof (ip)) 
            // << " says: " << buff << std::endl;
            //往池子中添加数据
            pthread_mutex_lock (&mutexMsgPool);
            sprintf (msgPool, "client's ip = %s says: %s", pt, buff);
            //广播
            std::cerr << "广播ing\n";
            broadCast ();
            pthread_mutex_unlock (&mutexMsgPool);
        }
        if (rlens == 0) {
            break;
        }
    }
}

void broadCast () {
    for (auto &t : tub) {
        int slens = send (t, msgPool, 1024, 0);
        if (slens < 0) std::cerr << "send err...\n";
    }
}
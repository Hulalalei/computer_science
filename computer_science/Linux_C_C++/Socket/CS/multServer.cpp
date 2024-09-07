#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "ThreadPool.h"


//聊天池
static char *msgPool = new char;
static pthread_mutex_t mutexMsgPool;
static pthread_mutex_t mutexComm;
static pthread_cond_t condRead;

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
	ThreadPool* p;
	sockaddr_in caddr;
	const char *ip = new char;
	char *pt = new char;

	WorkInfo () {}
	WorkInfo (int two) {
		this->cfd = two;
	}
};

void acceptConn (void *arg);
void working (void *arg);
char *readLine (char *pt, int *count);

void receives (void *arg) {
	WorkInfo* winfo = static_cast<WorkInfo*> (arg);
	std::cout << "receive winfo-ip = " << winfo->ip << "\n";
	while (true) {
		char *buff = new char[1024 * 5];
		int rlens = recv (winfo->cfd, buff, sizeof (buff), 0);
		if (rlens > 0) {
			std::cout << "client " << winfo->ip << " says: " << buff << std::endl;

			//往池子中添加数据
			pthread_mutex_lock (&mutexMsgPool);
			strcat (msgPool, "client's ip = ");
			strcat (msgPool, winfo->ip);
			strcat (msgPool, " says: ");
			strcat (msgPool, buff);
			std::cerr << msgPool << "okok01\n";
			pthread_mutex_unlock (&mutexMsgPool);

			pthread_cond_broadcast (&condRead);
		}
		if (rlens == 0) {
			std::cout << "client wip = " << winfo->ip << " has cut the connection" << std::endl;
			break;
		}
	}
}

void sends (void *arg) {
	WorkInfo* winfo = static_cast<WorkInfo*> (arg);
	while (true) {
		//读取池子里的数据并转发到客户端
		pthread_mutex_lock (&mutexMsgPool);
		int *count = new int;
		*count = 0;
		char *box = readLine (winfo->pt, count);
		int counts = *count;
		std::cerr << "box : " << box << "\n";
		pthread_mutex_unlock (&mutexMsgPool);

		while (*count > 0) {
			int slen = send (winfo->cfd, box, counts, 0);
			sleep (1);
			if (slen == -1) std::cerr << "server send err...\n";
			if (slen >= 0) box += slen;
			*count -= slen;
		}
		std::cerr << "jump\n";

		pthread_mutex_lock (&mutexMsgPool);
		winfo->pt += counts + 1;
		pthread_cond_wait (&condRead, &mutexMsgPool);
		pthread_mutex_unlock (&mutexMsgPool);
	}
}


//关于端口无法重复使用问题：套接字未关闭.
int main (void) {
	int fd = socket (AF_INET, SOCK_STREAM, 0);
	if (fd == -1) std::cerr << "init err" << std::endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons (11051);
	saddr.sin_addr.s_addr = INADDR_ANY;		//client's ip
	int ret = bind(fd, (struct sockaddr*)&saddr, sizeof (saddr));
	if (ret == -1) std::cerr << "bind err" << std::endl;

	ret = listen (fd, 3);
	if (ret == -1) std::cerr << "listen err" << std::endl;

	pthread_mutex_init (&mutexMsgPool, nullptr);

	ThreadPool* pool = new ThreadPool (3, 6, 9);
	PInfo* p = new PInfo (pool, fd);
	pool->addTasks (TaskQueue (acceptConn, p));
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
		if (winfo->cfd == -1) std::cerr << "accept err" << std::endl;		
		winfo->p = t->p;
		t->p->addTasks (TaskQueue (working, winfo));
	}
}

void working (void *arg) {
	WorkInfo* winfo = static_cast<WorkInfo*> (arg);
	char *pt = msgPool;
	

	char ip[32];
	std::cout << "client's ip = " << inet_ntop (AF_INET, &winfo->caddr.sin_addr.s_addr, ip, sizeof (ip))
	<< ", port = " << ntohs (winfo->caddr.sin_port) << " has join the chating-room" << std::endl;
	
	winfo->ip = inet_ntop (AF_INET, &winfo->caddr.sin_addr.s_addr, ip, sizeof (ip));
	std::cerr << "work winfo-ip = " << winfo->ip << "\n";//////////////////////////////////
	winfo->pt = pt;
	//往池子中添加数据
	pthread_mutex_lock (&mutexMsgPool);
	strcat (msgPool, "client's ip = ");
	strcat (msgPool, inet_ntop (AF_INET, &winfo->caddr.sin_addr.s_addr, ip, sizeof (ip)));
	strcat (msgPool, ", port = ");
	char *temp01 = new char;
	sprintf (temp01, "%d", ntohs (winfo->caddr.sin_port));
	strcat (msgPool, temp01);
	strcat (msgPool, " has join the chating-room\n");
	pthread_mutex_unlock (&mutexMsgPool);

	winfo->p->addTasks (TaskQueue (receives, winfo));
	winfo->p->addTasks (TaskQueue (sends, winfo));
}

char *readLine (char *pt, int *count) {
	if (pt[0] == '\0') return nullptr;
	char *box = new char;
	int i = 0;
	while (pt[i] != '\n') {
		
		box[i] = pt[i];
		i++;
		*count += 1;
	}
	return box;
}

#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>
#include <map>
#include <exception>
#include <fstream>
#include <thread>

#include <unistd.h>
#include <json/json.h>
#include <mysql/mysql.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#include <cstring>
#include <cstdio>
#include <strings.h>

#include "./ThreadPool/ThreadPool.hpp"
#include "./ConnectionPool/ConnectionPool.hpp"
#include "./MemoryPool/MemoryPool.hpp"
#include "./Log/_freecplus.h"



class HttpServer {
public:
    HttpServer();
    HttpServer(const HttpServer &obj) = delete;
    HttpServer &operator=(const HttpServer &obj) = delete;
    void provide_server();
    ~HttpServer();
private:
    int m_epfd;
    int event_cnt;
    int m_port;
    int serv_sock;

    struct sockaddr_in serv_adr;
    struct epoll_event *ep_events;

    ThreadPool *thread_pool;
    CLogFile logfile;

    static auto accept_server(void *arg) -> void;
    static auto request_handler(void *arg) -> void;
    void response(int clnt_sock, char *method, char *ct, char *file_name);
    const char *content_type(char *file);

    void get_content_html(FILE *clnt_write, char *method, char *ct, char *file_name);
    void get_dir_html(FILE *clnt_write, char *dir_name);
    void post_html(FILE *clnt_write, char *method, char *ct, char *file_name);
    void options_html(FILE *clnt_write, char *method, char *ct, char *file_name);
    void error_html(FILE *clnt_write);

    void send_res_head(FILE *clnt_write, int stat_num, const char *describe, const char *ct);
    void send_res_content(FILE *clnt_write, const char *file);
};

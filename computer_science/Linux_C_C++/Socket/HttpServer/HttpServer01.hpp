#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>
#include <utility>
#include <functional>
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




class HttpServer;
class Request;
class Response;
class Content_Type;



class Info {
friend class Request;
public:
    int clnt_sock;
    HttpServer *http;
    std::shared_ptr<Request> req_ptr;
    ~Info();
};

class Request {
friend class HttpServer;
friend class Response;
friend class Info;
public:
    Request();
    Request(const Request &obj) = delete;
    Request &operator=(const Request &obj) = delete;
    ~Request();
private:
    std::shared_ptr<char> req_line;
    std::shared_ptr<char> method;
    std::shared_ptr<char> ct;
    std::shared_ptr<char> file_name;

    static void request_handler(std::shared_ptr<Info> info);
};

class Response {
friend class HttpServer;
friend class Request;
public:
    Response();
    Response(const Response &obj) = delete;
    Response &operator=(const Response &obj) = delete;
    ~Response();
private:
    void response_GET(int clnt_sock, std::shared_ptr<char> ct, std::shared_ptr<char> file_name);
    void response_POST(int clnt_sock, std::shared_ptr<char> ct, std::shared_ptr<char> file_name);
    void response_ERR(FILE *clnt_write);
    void send_res_head(FILE *clnt_write, int stat_num, const char *describe, std::shared_ptr<char> ct);
    void send_res_content(FILE *clnt_write, std::shared_ptr<char> file);
};

class Content_Type {
public:
    const char *content_type(const char *file);
};

class HttpServer {
friend class Request;
friend class Response;
public:
    HttpServer();
    HttpServer(const HttpServer &obj) = delete;
    HttpServer &operator=(const HttpServer &obj) = delete;
    ~HttpServer();

    void provide_server();
private:
    int m_serv_sock;
    int m_port;
    struct sockaddr_in m_serv_adr;

    int m_epfd;
    int m_event_cnt;
    std::shared_ptr<struct epoll_event> m_ep_events;

    CLogFile m_logInfo, m_logfile;
    std::shared_ptr<Response> m_res_ptr;
    std::shared_ptr<Content_Type> m_ct_ptr;

    static auto accept_server(void *arg) -> void;
};

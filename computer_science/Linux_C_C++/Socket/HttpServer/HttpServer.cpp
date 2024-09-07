#include "HttpServer.hpp"

#define SIZE 100

/*
    1、c++11池技术
    2、日志追踪
    3、处理GET和POST请求
    4、c++11新特性
    5、json进行配置
    6、makefile编译
    7、封装一个自动销毁的智能指针
    8、MVC架构
    9、接收和响应分为两个类封装
    10、获取对应content-type进行封装
*/

HttpServer::HttpServer() : m_port(0), m_epfd(0), event_cnt(0), serv_sock(0), 
            ep_events(nullptr), thread_pool(nullptr) {
    logfile.Open("./Res/http.log", "w+");
    logfile.Write("begin\n");

    std::ifstream ifsJson("./config.json", std::ifstream::in);
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifsJson, root);
    m_port = atoi(root["port"].asString().c_str());
    int min_size = root["min_size"].asInt();
    int max_size = root["max_size"].asInt();
    int capacity = root["capacity"].asInt();

    thread_pool = new ThreadPool(min_size, max_size, capacity);

    // 切换服务器的工作路径
    chdir("./Res");

    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int opt = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("re_use error\n");

    memset(&serv_adr, 0, sizeof serv_adr);
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(m_port);
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof serv_adr) == -1)
        throw std::runtime_error("bind() error\n");

    if (listen(serv_sock, SIZE) == -1)
        throw std::runtime_error("listen() error\n");

    m_epfd = epoll_create(SIZE);
    ep_events = new struct epoll_event[SIZE];
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(m_epfd, EPOLL_CTL_ADD, serv_sock, &event);

    ifsJson.close();
}

class Info {
public:
    HttpServer *http;
    int clnt_sock;
};

void HttpServer::provide_server() {
    while (true) {
        // serv_sock在收到消息后，才会解除阻塞
        if ((event_cnt = epoll_wait(m_epfd, ep_events, SIZE, -1)) == -1)
            throw std::runtime_error("epoll_wait() error\n");
        for (int i = 0; i < event_cnt; i ++) {
            if (ep_events[i].data.fd == serv_sock) {
                // accept未处理完，会一直有事件发生，必须阻塞，不然循环会把os线程用完
                std::thread accep(accept_server, this);
                accep.join();
            }
            else {
                class Info *info = new class Info;
                info->clnt_sock = ep_events[i].data.fd;
                info->http = this;
                thread_pool->addTask(this->request_handler, info);
            }
        }
    }
}

void HttpServer::accept_server(void *arg) {
    HttpServer *http = static_cast<HttpServer*> (arg);
    struct sockaddr_in clnt_adr;
    socklen_t adr_sz = sizeof(struct sockaddr);
    int clnt_sock = 0;
    if ((clnt_sock = accept(http->serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz)) == -1)
        throw std::runtime_error("accept() error\n");

    int flag = fcntl(clnt_sock, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(clnt_sock, F_SETFL, flag);
    struct epoll_event event;
    event.data.fd = clnt_sock;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(http->m_epfd, EPOLL_CTL_ADD, clnt_sock, &event);

    http->logfile.Write("connect request %u:%u\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
    // std::cout << "connect request " << inet_ntoa(clnt_adr.sin_addr) << ":" << ntohs(clnt_adr.sin_port) << std::endl;
}

void HttpServer::request_handler(void *arg) {
    Info *info = static_cast<Info*> (arg);
    char *req_line = new char[1024];
    char *temp = new char[1024];
    char *method = new char[10];
    char *ct = new char[15];
    char *file_name = new char[30];

    //ET读取
    int str_len = 0, offset = 0;
    while ((str_len = read(info->clnt_sock, temp, 1024)) > 0) {
        memcpy(req_line + offset, temp, str_len);
        offset += str_len;
    }
    
    // 读取请求行
    std::string buf(req_line), temps = "";
    std::istringstream reader(buf);
    getline(reader, temps), strcpy(req_line, temps.c_str());

    if (str_len == -1 && errno == EAGAIN) {
        if (strstr(req_line, "HTTP/") == nullptr) {
            epoll_ctl(info->http->m_epfd, EPOLL_CTL_DEL, info->clnt_sock, nullptr);
            close(info->clnt_sock);
            if (!req_line) delete[] req_line;
            throw std::runtime_error("req_line error\n");
        }

        // "GET /index.html HTTP/1.1"
        // std::cerr << "req_line = " << req_line << std::endl;
        sscanf(req_line, "%s %s", method, file_name);
        sscanf(file_name, "%[^?]", file_name);
        // std::cerr << "method : " << method << std::endl << "file = " << file_name << std::endl;
        info->http->logfile.Write("req_line = %s\nmethod = %s\nfile = %s\n", req_line, method, file_name);
        if (info->http->content_type(file_name) != nullptr) {
            strcpy(ct, info->http->content_type(file_name));
            info->http->logfile.Write("ct = %s\n", ct);
        }
        else {
            if (!ct) delete ct;
            ct = nullptr;
        }   

        if (!req_line) delete[] req_line;
        if (!temp) delete[] temp;

        // response http
        info->http->response(info->clnt_sock, method, ct, file_name);
        if (!info) delete info;
    }
    else if (str_len == 0) {
        epoll_ctl(info->http->m_epfd, EPOLL_CTL_DEL, info->clnt_sock, nullptr);
        close(info->clnt_sock);
        if (!req_line) delete[] req_line;
        if (!temp) delete[] temp;
        if (!method) delete[] method;
        if (!ct) delete[] ct;
        if (!file_name) delete[] file_name;
        if (!info) delete info;
    }
}

void HttpServer::response(int clnt_sock, char *method, char *ct, char *file_name) {
    FILE *clnt_write = fdopen(clnt_sock, "w");

    if (strcmp(method, "GET") == 0) {
        char *files = nullptr;
        if (strcmp(file_name, "/") == 0) files = "./";
        else files = file_name + 1;

        struct stat st;
        if (stat(files, &st) == -1) error_html(clnt_write);
        if (S_ISDIR(st.st_mode)) get_dir_html(clnt_write, files);
        else get_content_html(clnt_write, method, ct, files);

        if (!method) delete[] method;
        if (!ct) delete[] ct;
        if (!file_name) delete[] file_name;
    }
    else if (strcmp(method, "POST") == 0)
        post_html(clnt_write, method, ct, file_name);
    else if (strcmp(method, "OPTIONS") == 0)
        options_html(clnt_write, method, ct, file_name);
}

const char *HttpServer::content_type(char *file) {
    const char *dot = strrchr(file, '.');
    if (dot == nullptr)
        return "text/plain; charset=utf-8";
    else if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    else if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    else if (strcmp(dot, ".png") == 0)
        return "image/png";
    else if (strcmp(dot, ".css") == 0)
        return "text/css";
    else if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    else if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    else if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    else if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    else if (strcmp(dot, ".ico") == 0)
        return "image/vnd.microsoft.icon";
    else if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    else return nullptr;
}

void HttpServer::get_dir_html(FILE *clnt_write, char *dir_name) {
    send_res_head(clnt_write, 200, "OK", content_type(".html"));

    char buf[1024] = { 0 };
    sprintf(buf, "<html><head><title>%s</title></head><body><table>", dir_name);

    struct dirent **name_list = nullptr;
    int num = scandir(dir_name, &name_list, nullptr, alphasort);

    for (int i = 0; i <num; i ++) {
        char *name = name_list[i]->d_name;
        char sub_path[SIZE];
        sprintf(sub_path, "%s/%s", dir_name, name);

        struct stat st;
        stat(sub_path, &st);
        if (S_ISDIR(st.st_mode)) {
            sprintf(buf + strlen(buf), 
            "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>", 
            name, name, st.st_size);
        }
        else {
            sprintf(buf + strlen(buf), 
            "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", 
            name, name, st.st_size);
        }
        fputs(buf, clnt_write);
        fflush(clnt_write);
        memset(buf, 0, sizeof buf);
        delete name_list[i];
    }
    sprintf(buf, "</num;></body></html>");
    fputs(buf, clnt_write);
    fflush(clnt_write);
    delete name_list;
}

void HttpServer::get_content_html(FILE *clnt_write, char *method, char *ct, char *file_name) {
    if (ct == nullptr) error_html(clnt_write);
    else {
        send_res_head(clnt_write, 200, "OK", ct);
        send_res_content(clnt_write, file_name);
    }
}

void HttpServer::post_html(FILE *clnt_write, char *method, char *ct, char *file_name) {
    logfile.Write("receive a post request");
    // std::cout << "receive a post request" << std::endl;
    error_html(clnt_write);
}

void HttpServer::options_html(FILE *clnt_write, char *method, char *ct, char *file_name) {
    logfile.Write("receive a option request");
    // std::cout << "options" << std::endl;
    error_html(clnt_write);
}

void HttpServer::error_html(FILE *clnt_write) {
    send_res_head(clnt_write, 404, "Not Found", content_type(".html"));
    send_res_content(clnt_write, "404.html");
}

void HttpServer::send_res_head(FILE *clnt_write, int stat_num, const char *describe, const char *ct) {
    char protocol[30] = { 0 };
    char server[] = "Server: Linux Web Server\r\n";
    char cnt_len[] = "Content-length: -1\r\n";
    char cnt_type[SIZE] = { 0 };
    sprintf(protocol, "HTTP/1.1 %d %s\r\n", stat_num, describe);
    sprintf(cnt_type, "Content-type: %s\r\n\r\n", ct);

    // 发送状态行，消息头
    fputs(protocol, clnt_write);
    fputs(server, clnt_write);
    fputs(cnt_len, clnt_write);
    fputs(cnt_type, clnt_write);
    fflush(clnt_write);
}

void HttpServer::send_res_content(FILE *clnt_write, const char *file) {
    char buf[SIZE] = { 0 };
    FILE *send_file = fopen(file, "r");
    if (send_file == nullptr) {
        error_html(clnt_write);
        return;
    }
    
    // 发送消息
    while (fread(buf, sizeof(char), SIZE, send_file) > 0) {
        fwrite(buf, SIZE, 1, clnt_write);
        fflush(clnt_write);
    }
    fflush(clnt_write);
    std::cout << "send_completed\n" << std::endl;
}

HttpServer::~HttpServer() {
    if (!ep_events) delete[] ep_events;
}

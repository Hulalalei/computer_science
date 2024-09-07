#include "HttpServer01.hpp"

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
    11、多反应堆模型
*/

// 当前：单反应堆模型
HttpServer::HttpServer() : m_port(0), m_epfd(0), m_event_cnt(0), m_serv_sock(0), 
                m_ep_events(std::shared_ptr<struct epoll_event> (new struct epoll_event[SIZE])), 
                m_res_ptr(std::shared_ptr<Response> (new Response)), 
                m_ct_ptr(std::shared_ptr<Content_Type> (new Content_Type)) {
    // 日志
    m_logInfo.Open("./Res/http.log", "w+");
    m_logfile.Open("./Res/request.log", "w+");
    m_logInfo.Write("begin\n");
    m_logfile.Write("begin\n");

    // Json
    std::ifstream ifsJson("./config.json", std::ifstream::in);
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifsJson, root);
    m_port = atoi(root["port"].asString().c_str());
    int min_size = root["min_size"].asInt();
    int max_size = root["max_size"].asInt();
    int capacity = root["capacity"].asInt();
    ifsJson.close();


    // 切换服务器的工作路径
    chdir("./Res");

    // 基于TCP
    m_serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 端口复用
    int opt = 1;
    if (setsockopt(m_serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("re_use error\n");

    // 绑定 & 监听
    memset(&m_serv_adr, 0, sizeof m_serv_adr);
    m_serv_adr.sin_family = AF_INET;
    m_serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serv_adr.sin_port = htons(m_port);
    if (bind(m_serv_sock, (struct sockaddr *)&m_serv_adr, sizeof m_serv_adr) == -1)
        throw std::runtime_error("bind() error\n");
    if (listen(m_serv_sock, SIZE) == -1)
        throw std::runtime_error("listen() error\n");

    // 事件上树
    m_epfd = epoll_create(SIZE);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = m_serv_sock;
    epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_serv_sock, &event);
}

void HttpServer::provide_server() {
    while (true) {
        // m_serv_sock在收到消息后，才会解除阻塞
        if ((m_event_cnt = epoll_wait(m_epfd, m_ep_events.get(), SIZE, -1)) == -1)
            throw std::runtime_error("epoll_wait() error\n");
        for (int i = 0; i < m_event_cnt; i ++) {
            if (m_ep_events.get()[i].data.fd == m_serv_sock) {
                // accept未处理完，会一直有事件发生，必须阻塞，不然循环会把os线程用完
                std::thread accep(accept_server, this);
                accep.join();
            }
            else {
                std::shared_ptr<Info> info(new Info);
                info->clnt_sock = m_ep_events.get()[i].data.fd;
                info->http = this;
                info->req_ptr = std::shared_ptr<Request> (new Request);

                void (*ptr)(std::shared_ptr<Info>) = info->req_ptr->request_handler;
                std::thread t(ptr, info);
                t.join();
            }
        }
    }
}

void HttpServer::accept_server(void *arg) {
    HttpServer *http = static_cast<HttpServer*> (arg);
    struct sockaddr_in clnt_adr;
    socklen_t adr_sz = sizeof(struct sockaddr);
    int clnt_sock = 0;
    if ((clnt_sock = accept(http->m_serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz)) == -1)
        throw std::runtime_error("accept() error\n");
    int flag = fcntl(clnt_sock, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(clnt_sock, F_SETFL, flag);
    struct epoll_event event;
    event.data.fd = clnt_sock;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(http->m_epfd, EPOLL_CTL_ADD, clnt_sock, &event);
    http->m_logfile.Write("connect request %s:%u\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
    http->m_logInfo.Write("connect request %s:%u\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
}

HttpServer::~HttpServer() {
    
}


Info::~Info() {
    http = nullptr;
    req_ptr->req_line.reset();
    req_ptr->method.reset();
    req_ptr->ct.reset();
    req_ptr->file_name.reset();
    req_ptr.reset();
}

Request::Request(): req_line(std::shared_ptr<char> (new char[1024], [](char *p) { delete[] p; })), 
                method(std::shared_ptr<char> (new char[128], [](char *p) { delete[] p; })), 
                ct(std::shared_ptr<char> (new char[128], [](char *p) { delete[] p; })), 
                file_name(std::shared_ptr<char> (new char[128], [](char *p) { delete[] p; })) {
    
    memset(req_line.get(), 0, 1024);
    memset(method.get(), 0, 128);
    memset(ct.get(), 0, 128);
    memset(file_name.get(), 0, 128);
}

void Request::request_handler(std::shared_ptr<Info> info) {
    int clnt_sock = info->clnt_sock;
    char *req_line = info->req_ptr->req_line.get();
    char *method = info->req_ptr->method.get();
    char *ct = info->req_ptr->ct.get();
    char *file_name = info->req_ptr->file_name.get();
    char temp[1024] = { 0 };
    
    //ET读取
    int str_len = 0, offset = 0;
    while ((str_len = read(clnt_sock, temp, 1024)) > 0) {
        memcpy(req_line + offset, temp, str_len);
        offset += str_len;
    }

    if (str_len == -1 && errno == EAGAIN) {
        // 解析请求行
        info->http->m_logfile.Write("%s", req_line);
        sscanf(req_line, "%[^\r\n]", req_line);
        if (strstr(req_line, "HTTP/") == nullptr) {
            epoll_ctl(info->http->m_epfd, EPOLL_CTL_DEL, clnt_sock, nullptr), close(clnt_sock);
            info->http->m_logInfo.Write("request's format is wrong");
            return;
        }
        
        sscanf(req_line, "%s %s", method, file_name);
        sscanf(file_name, "%[^?]", file_name);
        info->http->m_logInfo.Write("req_line = %s\nmethod = %s\nfile = %s\n", req_line, method, file_name);
        
        if (info->http->m_ct_ptr.get()->content_type(file_name) != nullptr) {
            strcpy(ct, info->http->m_ct_ptr.get()->content_type(file_name));
            info->http->m_logInfo.Write("ct = %s\n", ct);
            if (strcmp(file_name, "/") == 0)
                strcpy(file_name, "index.html");
        }
        else {
            ct = nullptr;
            info->http->m_logInfo.Write("can't parse the content-type");
        }

        // response http
        if (strcmp(method, "GET") == 0)
            info->http->m_res_ptr.get()->response_GET(clnt_sock, info->req_ptr->ct, info->req_ptr->file_name);
        else if (strcmp(method, "POST") == 0)
            info->http->m_res_ptr.get()->response_POST(clnt_sock, info->req_ptr->ct, info->req_ptr->file_name);
        else info->http->m_logInfo.Write("request's format is wrong");// std::move(info->req_ptr->ct), std::move(info->req_ptr->file_name)
    }
    else if (str_len == 0) {
        epoll_ctl(info->http->m_epfd, EPOLL_CTL_DEL, clnt_sock, nullptr);
        close(clnt_sock);
    }
}

Request::~Request() {
    req_line.reset();
    method.reset();
    ct.reset();
    file_name.reset();
}




Response::Response() {
    
}

void Response::response_GET(int clnt_sock, std::shared_ptr<char> ct, std::shared_ptr<char> file_name) {
    FILE *clnt_write = fdopen(clnt_sock, "w");
    char *temp = new char[100];
    memset(temp, 0, 100);
    if (*file_name == '/') {
        strcpy(temp, file_name.get() + 1);
        strcpy(file_name.get(), temp);
    }

    if (ct.get() == nullptr) response_ERR(clnt_write);
    else {
        send_res_head(clnt_write, 200, "OK", ct);
        send_res_content(clnt_write, file_name);
    }
}

void Response::response_POST(int clnt_sock, std::shared_ptr<char> ct, std::shared_ptr<char> file_name) {

}

void Response::response_ERR(FILE *clnt_write) {
    std::cout << "404.error" << std::endl;

    std::shared_ptr<char> file(new char[30], [](char *p) { delete[] p; });
    std::shared_ptr<char> ct(new char[30], [](char *p) { delete[] p; });
    memset(file.get(), 0, 256);
    memset(ct.get(), 0, 256);

    strcpy(file.get(), "./404.html");
    strcpy(ct.get(), "text/html; charset=utf-8");
    std::cout << "file1 = " << file << " ct1 = " << ct << std::endl;
    send_res_head(clnt_write, 404, "Not Found", ct);
    send_res_content(clnt_write, file);
}

void Response::send_res_head(FILE *clnt_write, int stat_num, const char *describe, std::shared_ptr<char> ct) {
    char protocol[30] = { 0 };
    char server[] = "Server: Linux Web Server\r\n";
    char cnt_len[] = "Content-length: -1\r\n";
    char cnt_type[SIZE] = { 0 };
    sprintf(protocol, "HTTP/1.1 %d %s\r\n", stat_num, describe);
    sprintf(cnt_type, "Content-type: %s\r\n\r\n", ct.get());
    // 发送状态行，消息头
    fputs(protocol, clnt_write);
    fputs(server, clnt_write);
    fputs(cnt_len, clnt_write);
    fputs(cnt_type, clnt_write);
    fflush(clnt_write);
    std::cout << "ok\n";
}

void Response::send_res_content(FILE *clnt_write, std::shared_ptr<char> file) {
    char buf[SIZE] = { 0 };
    std::cout << "file = " << file << std::endl;
    FILE *send_file = fopen(file.get(), "r");
    if (send_file == nullptr) {
        response_ERR(clnt_write);
        return;
    }

    // 发送消息
    while (fgets(buf, SIZE, send_file) != nullptr) {
        fputs(buf, clnt_write);
        fflush(clnt_write);
    }
    fflush(clnt_write);
    std::cout << "send_completed\n" << std::endl;
}

Response::~Response() {

}




const char *Content_Type::content_type(const char *file) {
    if (strcmp(file, "/") == 0)
        return "text.html; charset=utf-8";
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
    else if (strcmp(dot, ".mp4") == 0)
        return "video/mp4";
    else if (strcmp(dot, ".bak") == 0)
        return "application/x-trash";
    else if (strcmp(dot, ".bin") == 0)
        return "application/octet-stream";
    else if (strcmp(dot, ".class") == 0)
        return "application/x-java";
    else if (strcmp(dot, ".cmake") == 0)
        return "text/x-cmake";
    else if (strcmp(dot, ".cpp") == 0)
        return "text/x-c++src";
    else if (strcmp(dot, ".cs") == 0)
        return "text/x-csharp";
    else if (strcmp(dot, ".doc") == 0)
        return "application/msword";
    else if (strcmp(dot, ".docx") == 0)
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    else if (strcmp(dot, ".gz") == 0)
        return "application/x-gzip";
    else if (strcmp(dot, ".h") == 0)
        return "text/x-chdr";
    else if (strcmp(dot, ".hpp") == 0)
        return "text/x-c++hdr";
    else if (strcmp(dot, ".jar") == 0)
        return "application/x-java-archive";
    else if (strcmp(dot, ".java") == 0)
        return "text/x-java";
    else if (strcmp(dot, ".js") == 0)
        return "application/javascript";
    else if (strcmp(dot, ".json") == 0)
        return "application/json";
    else if (strcmp(dot, ".jsonp") == 0)
        return "application/jsonp";
    else if (strcmp(dot, ".latex") == 0)
        return "text/x-tex";
    else if (strcmp(dot, ".log") == 0)
        return "text/x-log";
    else if (strcmp(dot, ".lua") == 0)
        return "text/x-lua";
    else if (strcmp(dot, ".md") == 0)
        return "application/x-genesis-rom";
    else if (strcmp(dot, ".pdf") == 0)
        return "application/pdf";
    else if (strcmp(dot, ".php") == 0)
        return "application/x-php";
    else if (strcmp(dot, ".py") == 0)
        return "text/x-python";
    else if (strcmp(dot, ".sql") == 0)
        return "text/x-sql";
    else if (strcmp(dot, ".tar.gz") == 0)
        return "application/x-compressed-tar";
    else if (strcmp(dot, ".xbl") == 0 || strcmp(dot, ".xml") == 0)
        return "application/xml";
    else if (strcmp(dot, ".xla") == 0 || strcmp(dot, ".xlc") == 0 || strcmp(dot, ".xld") == 0 || strcmp(dot, ".xll") == 0 || strcmp(dot, ".xls") == 0 || strcmp(dot, ".xlm") == 0)
        return "application/vnd.ms-excel";
    else if (strcmp(dot, ".xlsm") == 0 || strcmp(dot, ".xlsx") == 0)
        return "application/application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    else if (strcmp(dot, ".zip") == 0)
        return "application/zip";
    else return nullptr;
}

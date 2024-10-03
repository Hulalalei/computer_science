#ifndef HTTP
#define HTTP


#include <iostream>
#include <format>
#include <map>
#include <string>

#include <thread>
#include <mutex>
#include <atomic>

#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdarg.h>

#include <cstring>
#include <cstdlib>

#include <mysql/mysql.h>
#include <connection_pool.hpp>
#include <minilog.hpp>


// bug: POST请求分了两次发送
// 浏览器问题
namespace http {
    const std::string protocol_200_ok = "HTTP/1.1 200 OK\r\n";
    const std::string protocol_200_content = "200 OK";

    const std::string protocol_201_created = "HTTP/1.1 201 Created\r\n";
    const std::string protocol_201_content = "201 Created";

    const std::string protocol_202_accepted = "HTTP/1.1 202 Accepted\r\n";
    const std::string protocol_202_content = "202 Accepted";

    const std::string protocol_204_no_content = "HTTP/1.1 204 No Content\r\n";
    const std::string protocol_204_content = "204 No Content";

    const std::string protocol_301_moved_permanently = "HTTP/1.1 301 Moved Permanently\r\n";
    const std::string protocol_301_content = "301 Moved Permanently";

    const std::string protocol_400_bad_request = "HTTP/1.1 400 Bad Request\r\n";
    const std::string protocol_400_content = "400 Bad Request";

    const std::string protocol_403_forbidden = "HTTP/1.1 403 Forbidden\r\n";
    const std::string protocol_403_content = "403 Forbidden";

    const std::string protocol_404_not_found = "HTTP/1.1 404 Not Found\r\n";
    const std::string protocol_404_content = "404 Not Found";

    const std::string protocol_500_internal_server_error = "HTTP/1.1 500 Internal Server Error\r\n";
    const std::string protocol_500_content = "500 Internal Server Error";

    const std::string protocol_501_not_implemented = "HTTP/1.1 501 Not Implemented\r\n";
    const std::string protocol_501_content = "501 Not Implemented";

    const std::string protocol_502_bad_gateway = "HTTP/1.1 502 Bad Gateway\r\n";
    const std::string protocol_502_content = "502 Bad Gateway";

    const std::string protocol_503_service_unavailable = "HTTP/1.1 503 Service Unavailable\r\n";
    const std::string protocol_503_content = "503 Service Unavailable";

    const std::string server_name = "Server: Ubuntu 13\r\n";
    const std::string content_type = "Content-Type: text/html; charset=UTF-8\r\n";
    const std::string connection = "Connection: ";
    const std::string content_length = "Content-Length: ";
    const std::string blank_line = "\r\n";

    // 全局mapping
    std::map<std::string, std::string> users;


    class http_conn {
    public:
        static constexpr int FILENAME_LEN = 200;
        static constexpr int READ_BUFFER_SIZE = 2048;
        static constexpr int WRITE_BUFFER_SIZE = 2048;
        enum http_method { GET, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH };
        enum http_code { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
        enum check_state { CHECK_STATE_REQUESTLINE, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
        enum line_status { LINE_OK, LINE_BAD, LINE_OPEN };

    public:
        void init_mysql_result(connsql::connection_pool *conn_pool);
        void init(int conn_sock, const sockaddr_in &address, std::string);
        // recv读取info到buf
        bool read_once();
        // writev将写buf发送出去
        bool write();
        // 组织解析http和写到写buf
        void process();
        void close_conn(bool real_close = true);

    private:
        int set_nonblocking(int fd);
        void addfd(int epfd, int fd);
        void removefd(int epfd, int fd);
        void modfd(int epfd, int fd, int ev);
        void refresh();


        http_code process_read();
        bool process_write(http_code ret);

        http_code parse_request_line(std::string text);
        http_code parse_headers(std::string text);
        http_code parse_content(std::string text);
        http_code do_request();


        bool add_status_line(const std::string);
        bool add_headers(const std::string, const int);
        bool add_content(const std::string);
        const std::string get_contenttype(const char *file);

        void unmap();
    private:
        // 用于通信连接的sockfd
        int conn_sock;
        struct sockaddr_in addr;
        struct stat file_stat;
        struct iovec iov[2];
        int iv_count;

        std::string read_buf;
        long read_idx;
        long checked_idx;
        int start_line;
        std::string write_buf;
        std::string file_address;
        int write_idx;
        int bytes_to_send;
        int bytes_have_send;
        std::string real_file;

        check_state check_state;
        http_method method;
        std::string url;
        std::string version;
        std::string host;
        long _content_length;
        bool linger;
        int cgi;
        std::string user_passwd;

        std::string doc_root;
    public:
        static int users_count;
        static int http_epfd;
        MYSQL *mysql_conn;
    };



    int http_conn::users_count = 0;
    int http_conn::http_epfd = -1;

    // 初始化的是全局变量mapping
    void http_conn::init_mysql_result(connsql::connection_pool *conn_pool) {
        connsql::connRAII connraii{conn_pool};
        MYSQL *mysql = connraii.conn;
        minilog::log_trace("Initialize the mapping users");
        minilog::log_trace("And store the sql table");
        if (mysql_query(mysql, "SELECT username, passwd FROM user")) minilog::log_error("Query failed");
        MYSQL_RES *result = mysql_store_result(mysql);
        while (MYSQL_ROW row = mysql_fetch_row(result)) {
            std::string temp1(row[0]);
            std::string temp2(row[1]);
            users[temp1] = temp2;
        }
    }

    void http_conn::init(int connfd, const sockaddr_in &address, std::string root) {
        minilog::log_trace("Init the http conn");
        conn_sock = connfd;
        addr = address;

        addfd(http_epfd, conn_sock);
        read_buf.resize(READ_BUFFER_SIZE);
        write_buf.resize(WRITE_BUFFER_SIZE);
        users_count ++;

        doc_root = root;
        refresh();
    }

    void http_conn::refresh() {
        minilog::log_trace("refresh the http conn");
        mysql_conn = nullptr;
        bytes_to_send = 0;
        bytes_have_send = 0;
        check_state = CHECK_STATE_REQUESTLINE;
        method = GET;
        url = "";
        version = "";
        _content_length = 0;
        host = "";

        start_line = 0;
        checked_idx = 0;
        read_idx = 0;
        read_buf = "";
        read_buf.resize(READ_BUFFER_SIZE);
        write_idx = 0;
        write_buf = "";
        write_buf.resize(WRITE_BUFFER_SIZE);
        linger = false;
        cgi = 0;
    }


    bool http_conn::read_once() {
        minilog::log_info("recv http request");
        if (read_idx >= READ_BUFFER_SIZE) {
            minilog::log_warning("read buffer overflow");
            return false;
        }

        int bytes_read = 0;
        while (true) {
            bytes_read = recv(conn_sock, &read_buf[read_idx], READ_BUFFER_SIZE - read_idx, 0);
            if (bytes_read == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                minilog::log_error("recv error");
                return false;
            }
            else if (bytes_read == 0) {
                minilog::log_trace("client close the connection");
                return false;
            }
            read_idx += bytes_read;
        }
        minilog::log_info("http request: \n{}", read_buf);
        minilog::log_trace("request size: {}", read_buf.length());
        return true;
    }

    bool http_conn::write() {
        minilog::log_info("send to browser");
        int temp = 0;

        if (bytes_to_send == 0) {
            minilog::log_info("no data to send");
            modfd(http_epfd, conn_sock, EPOLLIN);
            refresh();
            return true;
        }

        while (true) {
            temp = writev(conn_sock, iov, iv_count);
            if (temp < 0) {
                if (errno == EAGAIN) {
                    modfd(http_epfd, conn_sock, EPOLLOUT);
                    minilog::log_info("send data to client");
                    return true;
                }
                unmap();
                minilog::log_error("send error");
                return false;
            }

            bytes_have_send += temp;
            bytes_to_send -= temp;
            if (bytes_have_send >= iov[0].iov_len) {
                iov[0].iov_len = 0;
                iov[1].iov_base = &file_address[bytes_have_send - write_idx];
                iov[1].iov_len = bytes_to_send;
                minilog::log_info("write_buf overflow");
            }
            else {
                iov[0].iov_base = &write_buf[bytes_have_send];
                iov[0].iov_len = iov[0].iov_len - bytes_have_send;
                minilog::log_info("write_buf underflow");
            }

            if (bytes_to_send <= 0) {
                unmap();
                modfd(http_epfd, conn_sock, EPOLLIN);

                if (linger) {
                    refresh();
                    minilog::log_trace("linger");
                    return true;
                }
                else {
                    minilog::log_info("no data to send");
                    return false;
                }
            }
        }
    }

    void http_conn::process() {
        http_code read_ret = process_read();
        if (read_ret == NO_REQUEST) {
            minilog::log_info("read http request failed");
            modfd(http_epfd, conn_sock, EPOLLIN);
            return;
        }
        bool write_ret = process_write(read_ret);
        if (!write_ret) close_conn();
        // 在此触发写事件
        modfd(http_epfd, conn_sock, EPOLLOUT);
    }

    http_conn::http_code http_conn::process_read() {
        minilog::log_info("parse http request");
        http_code ret = NO_REQUEST;

        while (true) {
            switch (check_state)
            {
                case CHECK_STATE_REQUESTLINE:
                {
                    minilog::log_info("parse request line");
                    ret = parse_request_line(read_buf);
                    if (ret == BAD_REQUEST)
                        return BAD_REQUEST;
                    break;
                }
                case CHECK_STATE_HEADER:
                {
                    minilog::log_info("parse request header");
                    ret = parse_headers(read_buf);
                    if (ret == BAD_REQUEST) return BAD_REQUEST;
                    else if (ret == GET_REQUEST) return do_request();
                    break;
                }
                case CHECK_STATE_CONTENT:
                {
                    // log_writer.write(log::info, "parse request content");
                    ret = parse_content(read_buf);
                    if (ret == GET_REQUEST) return do_request();
                    return BAD_REQUEST;
                }
                default:
                {
                    minilog::log_error("parse http request error");
                    return INTERNAL_ERROR;
                }
            }
        }
        return NO_REQUEST;
    }

    bool http_conn::process_write(http_code ret) {
        minilog::log_info("response http response");
        switch (ret)
        {
            case INTERNAL_ERROR:
            {
                minilog::log_error("internal error");
                add_status_line(protocol_500_internal_server_error);
                add_headers(".html", 35 + protocol_500_content.size());
                if (!add_content("<html><body><h1>" + protocol_500_content + "</h1></body></html>"))
                    return false;
                break;
            }
            case BAD_REQUEST:
            {
                minilog::log_error("bad request");
                add_status_line(protocol_404_not_found);
                add_headers(".html", 35 + protocol_404_content.size());
                if (!add_content("<html><body><h1>" + protocol_404_content + "</h1></body></html>"))
                    return false;
                break;
            }
            case FORBIDDEN_REQUEST:
            {
                minilog::log_error("forbidden request");
                add_status_line(protocol_403_forbidden);
                add_headers(".html", 35 + protocol_403_content.size());
                if (!add_content("<html><body><h1>" + protocol_403_content + "</h1></body></html>"))
                    return false;
                break;
            }
            case FILE_REQUEST:
            {
                minilog::log_info("file request");
                add_status_line(protocol_200_ok);
                if (file_stat.st_size != 0) {
                    minilog::log_info("write_idx: {}", write_idx);
                    if (url.size() < 1) return false;
                    add_headers(url.substr(1), file_stat.st_size);
                    iov[0].iov_base = &write_buf[0];
                    iov[0].iov_len = write_idx;
                    iov[1].iov_base = &file_address[0];
                    iov[1].iov_len = file_stat.st_size;
                    iv_count = 2;
                    bytes_to_send = write_idx + file_stat.st_size;
                    return true;
                }
                else {
                    minilog::log_error("file not found");
                    const char *ok_string = "<html><body></body></html>";
                    add_headers(".html", std::string(ok_string).size());
                    if (!add_content(ok_string)) {
                        minilog::log_error("add content error");
                        return false;
                    }
                }
            }
            default:
            {
                minilog::log_error("unknown http response");
                return false;
            }
        }
        iov[0].iov_base = &write_buf[0];
        iov[0].iov_len = write_idx;
        iv_count = 1;
        bytes_to_send = write_idx;
        return true;
    }

    http_conn::http_code http_conn::parse_request_line(std::string text) {
        if (text.find("GET") != std::string::npos) {
            method = GET;
            minilog::log_info("method = GET");
        }
        else if (text.find("POST") != std::string::npos) {
            method = POST;
            cgi = 1;
            minilog::log_info("method = POST");
        }
        else return BAD_REQUEST;


        auto st = text.find(" ", 0);
        auto end = text.find(" ", st + 1);
        if (end != std::string::npos)
            url = text.substr(st + 1, end - st - 1);
        minilog::log_info("before_url: {}", url);
        minilog::log_info("url.size: {}", url.size());

        if (text.find("HTTP/1.") == std::string::npos)
            return BAD_REQUEST;

        if (url.size() == 1)
            url += "LogIn.html";
        minilog::log_info("after url: {}", url);
        check_state = CHECK_STATE_HEADER;
        return NO_REQUEST;
    }

    http_conn::http_code http_conn::parse_headers(std::string text) {
        if (text.find("Connection:") != std::string::npos) {
            if (text.find("keep-alive")) {
                minilog::log_info("keep-alive");
                linger = true;
            }
        }
        if (text.find("Content-Length:") != std::string::npos) {
            size_t pos = text.find("Content-Length:");
            auto st = text.find("\r\n", pos + 1);
            _content_length = std::stol(text.substr(pos + 16, st - pos));
            minilog::log_info("content_length: {}", _content_length);
        }
        if (text.find("Host:") != std::string::npos) {
            size_t pos = text.find("Host:");
            auto st = text.find("\r\n", pos + 1);
            host = text.substr(pos, st - pos);
            minilog::log_info("host: {}", host);
        }
        else {
            minilog::log_warning("unrecognized header");
        }

        // 读取结束标志
        if (_content_length != 0 && cgi == 1) {
            check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }

    http_conn::http_code http_conn::parse_content(std::string text) {
        size_t st, end;
        if (text.find("username") != std::string::npos && text.find("passwd") != std::string::npos) {
            st = text.find("username");
            user_passwd = text.substr(st);
            minilog::log_info("user_passwd: {}", user_passwd);
            return GET_REQUEST;
        }
        return NO_REQUEST;
    }

    // 获取url处GET/POST文件，并将内容映射到_file_addr
    http_conn::http_code http_conn::do_request() {
        minilog::log_info("do request");
        real_file = doc_root;

        // POST请求
        if (cgi == 1 && (url.find("login") != std::string::npos)) {
            std::string username, passwd;

            auto pos = user_passwd.find("&");
            auto str_name = user_passwd.substr(0, pos);
            auto str_passwd = user_passwd.substr(pos + 1);

            auto pos_name = str_name.find("=");
            if (pos_name != std::string::npos) username = str_name.substr(pos_name + 1);
            auto pos_passwd = str_passwd.find("=");
            auto end = str_passwd.find('\0');
            if (pos_passwd != std::string::npos) passwd = str_passwd.substr(pos_passwd + 1, end - pos_passwd - 1);
            minilog::log_info("username: {}; passwd: {}", username, passwd);

            if (users.find(username) == users.end()) minilog::log_error("user not found");
            if (users[username] != passwd) minilog::log_error("passwd error");
            if (passwd.find("\r\n")) minilog::log_error("passwd error02");
            if (users.find(username) != users.end() && users[username] == passwd)
                url = "/index.html";
            else
                url = "/LogIn.html";
            minilog::log_info("map: {} {}", username, users[username]);
            minilog::log_info("post_url: {}", url);
        }
        // GET请求
        real_file += url;
        minilog::log_info("real_file: {}", real_file);

        if (stat(&real_file[0], &file_stat) < 0) { std::cout << "1"; return NO_RESOURCE; }
        if (!(file_stat.st_mode & S_IROTH)) { std::cout << "2"; return FORBIDDEN_REQUEST; }
        if (S_ISDIR(file_stat.st_mode)) { std::cout << "3"; return BAD_REQUEST; }

        int fd = open(&real_file[0], O_RDONLY);
        if (-1 == fd) return NO_RESOURCE;
        file_address = (char *)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        return FILE_REQUEST;
    }

    void http_conn::unmap() {
        if (&file_address[0]) {
            munmap(&file_address[0], file_stat.st_size);
            file_address = "";
        }
    }


    bool http_conn::add_status_line(const std::string protocol) {
        write_buf = protocol;
        write_idx += protocol.size();
        if (write_idx >= WRITE_BUFFER_SIZE) return false;
        return true;
    }

    bool http_conn::add_headers(const std::string content_type, const int content_len) {
        write_buf += server_name;
        write_idx += server_name.size();

        std::string ct = "Content-Type: " + get_contenttype(&content_type[0]) + "\r\n";
        write_buf += ct;
        write_idx += ct.size();

        linger ? write_buf += "Connection: keep-alive\r\n" : write_buf += "Connection: close\r\n";
        linger ? write_idx += 24 : write_idx += 19;

        std::string cl = content_length + std::to_string(content_len) + "\r\n";
        write_buf += cl;
        write_idx += cl.size();

        write_buf += blank_line;
        write_idx += blank_line.size();

        if (write_idx >= WRITE_BUFFER_SIZE) return false;
        return true;
    }


    bool http_conn::add_content(const std::string file) {
        write_buf += file;
        write_idx += file.size();
        if (write_idx >= WRITE_BUFFER_SIZE) return false;
        return true;
    }

    int http_conn::set_nonblocking(int fd) {
        int old_option = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, old_option | O_NONBLOCK);
        return old_option;
    }

    void http_conn::addfd(int epfd, int fd) {
        epoll_event event;
        event.data.fd = fd;
        set_nonblocking(fd);
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    }

    void http_conn::removefd(int epfd, int fd) {
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0);
        close(fd);
    }

    void http_conn::modfd(int epfd, int fd, int ev) {
        epoll_event event;
        event.data.fd = fd;
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
    }

    void http_conn::close_conn(bool real_close) {
        if (real_close && (conn_sock != -1)) {
            minilog::log_trace("close fd: {}", conn_sock);
            removefd(http_epfd, conn_sock);
            conn_sock = -1;
            users_count --;
        }
    }

    const std::string http_conn::get_contenttype(const char *file) {
        if (strcmp(file, "/") == 0)
            return std::string("text.html; charset=utf-8");
        const char *dot = strrchr(file, '.');
        if (dot == nullptr)
            return std::string("text/plain; charset=utf-8");
        else if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
            return std::string("text/html; charset=utf-8");
        else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
            return std::string("image/jpeg");
        else if (strcmp(dot, ".gif") == 0)
            return std::string("image/gif");
        else if (strcmp(dot, ".png") == 0)
            return std::string("image/png");
        else if (strcmp(dot, ".css") == 0)
            return std::string("text/css");
        else if (strcmp(dot, ".au") == 0)
            return std::string("audio/basic");
        else if (strcmp(dot, ".wav") == 0)
            return std::string("audio/wav");
        else if (strcmp(dot, ".avi") == 0)
            return std::string("video/x-msvideo");
        else if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
            return std::string("video/quicktime");
        else if (strcmp(dot, ".ico") == 0)
            return std::string("image/vnd.microsoft.icon");
        else if (strcmp(dot, ".mp3") == 0)
            return std::string("audio/mpeg");
        else if (strcmp(dot, ".mp4") == 0)
            return std::string("video/mp4");
        else if (strcmp(dot, ".bak") == 0)
            return std::string("application/x-trash");
        else if (strcmp(dot, ".bin") == 0)
            return std::string("application/octet-stream");
        else if (strcmp(dot, ".class") == 0)
            return std::string("application/x-java");
        else if (strcmp(dot, ".cmake") == 0)
            return std::string("text/x-cmake");
        else if (strcmp(dot, ".cpp") == 0)
            return std::string("text/x-c++src");
        else if (strcmp(dot, ".cs") == 0)
            return std::string("text/x-csharp");
        else if (strcmp(dot, ".doc") == 0)
            return std::string("application/msword");
        else if (strcmp(dot, ".docx") == 0)
            return std::string("application/vnd.openxmlformats-officedocument.wordprocessingml.document");
        else if (strcmp(dot, ".gz") == 0)
            return std::string("application/x-gzip");
        else if (strcmp(dot, ".h") == 0)
            return std::string("text/x-chdr");
        else if (strcmp(dot, ".hpp") == 0)
            return std::string("text/x-c++hdr");
        else if (strcmp(dot, ".jar") == 0)
            return std::string("application/x-java-archive");
        else if (strcmp(dot, ".java") == 0)
            return std::string("text/x-java");
        else if (strcmp(dot, ".js") == 0)
            return std::string("application/javascript");
        else if (strcmp(dot, ".json") == 0)
            return std::string("application/json");
        else if (strcmp(dot, ".jsonp") == 0)
            return std::string("application/jsonp");
        else if (strcmp(dot, ".latex") == 0)
            return std::string("text/x-tex");
        else if (strcmp(dot, ".log") == 0)
            return std::string("text/x-log");
        else if (strcmp(dot, ".lua") == 0)
            return std::string("text/x-lua");
        else if (strcmp(dot, ".md") == 0)
            return std::string("application/x-genesis-rom");
        else if (strcmp(dot, ".pdf") == 0)
            return std::string("application/pdf");
        else if (strcmp(dot, ".php") == 0)
            return std::string("application/x-php");
        else if (strcmp(dot, ".py") == 0)
            return std::string("text/x-python");
        else if (strcmp(dot, ".sql") == 0)
            return std::string("text/x-sql");
        else if (strcmp(dot, ".tar.gz") == 0)
            return std::string("application/x-compressed-tar");
        else if (strcmp(dot, ".xbl") == 0 || strcmp(dot, ".xml") == 0)
            return std::string("application/xml");
        else if (strcmp(dot, ".xla") == 0 || strcmp(dot, ".xlc") == 0 || strcmp(dot, ".xld") == 0 || strcmp(dot, ".xll") == 0 || strcmp(dot, ".xls") == 0 || strcmp(dot, ".xlm") == 0)
            return std::string("application/vnd.ms-excel");
        else if (strcmp(dot, ".xlsm") == 0 || strcmp(dot, ".xlsx") == 0)
            return std::string("application/application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
        else if (strcmp(dot, ".zip") == 0)
            return std::string("application/zip");
        else if (strcmp(dot, ".ttf") == 0)
            return std::string("application/x-font-ttf");
        else if (strcmp(dot, ".woff2") == 0)
            return std::string("font/woff2");
        else if (strcmp(dot, ".webp") == 0)
            return std::string("image/webp");
        else return std::string("");
    }

}
#endif

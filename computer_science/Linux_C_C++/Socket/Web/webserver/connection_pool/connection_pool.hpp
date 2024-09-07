#ifndef CONNECTION_POOL_HPP
#define CONNECTION_POOL_HPP


#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <fstream>
#include <chrono>

#include <mysql/mysql.h>
#include <json/json.h>
#include "../threadsafe_queue/threadsafe_queue.hpp"
#include "../log/log.hpp"

namespace web {
class connection_pool {
public:
    connection_pool();
    static connection_pool *get_instance();
    MYSQL *get_conn();
    bool release_conn(MYSQL *conn);
    ~connection_pool();
private:
    threadsafe_queue<MYSQL*> conn_queue;
    static constexpr int conn_count = 2;
    void init_conn(std::string host, std::string user, std::string passwd, std::string db, unsigned int port = 3306);
    void destroy_conn();
private:
    std::string host;
    std::string user;
    std::string passwd;
    std::string db;
    unsigned int port;
    class log log_writer;
};



connection_pool::connection_pool() {
    std::ifstream ifsJson("./config.json", std::ifstream::in);
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifsJson, root);
    host = root["host"].asString();
    user = root["user"].asString();
    passwd = root["passwd"].asString();
    db = root["db"].asString();
    port = root["port"].asInt();

    init_conn(host, user, passwd, db, port);
}


connection_pool *connection_pool::get_instance() {
    static connection_pool conn_pool;
    return &conn_pool;
}

MYSQL *connection_pool::get_conn() {
    auto conn = conn_queue.try_pop();
    while (conn == nullptr) {
        log_writer.write(log::error, "no conn available");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        conn = conn_queue.wait_and_pop();
    }
    log_writer.write(log::info, "create a conn");
    return *conn;
}

bool connection_pool::release_conn(MYSQL *conn) {
    conn_queue.push(conn);
    conn = nullptr;
    log_writer.write(log::info, "release a conn");
    return true;
}


void connection_pool::init_conn(std::string host, std::string user, std::string passwd, std::string db, unsigned int port) {
    std::cout << "conn\n";
    for (int i = 0; i < conn_count; i ++) {
        MYSQL *conn = nullptr;
        if ((conn = mysql_init(nullptr)) == nullptr) {
            std::cout << "err_init\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            log_writer.write(log::error, "mysql_init error");
        }
        else
            log_writer.write(log::info, "mysql_init success");

        if ((conn = mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0)) == nullptr) {
            std::cout << "conn_success\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            log_writer.write(log::error, "mysql_real_connect error");
        }
        else
            log_writer.write(log::info, "mysql_real_connect success");
        conn_queue.push(conn);
    }
    std::cout << "break\n";
}


void connection_pool::destroy_conn() {
    MYSQL *conn = nullptr;


    while (!conn_queue.empty()) {
        auto data = conn_queue.try_pop();
        if (data) conn = *data;
        if (conn) {
            mysql_close(conn);
            conn = nullptr;
        }
    }
}

connection_pool::~connection_pool() {
    destroy_conn();
}

}

#endif

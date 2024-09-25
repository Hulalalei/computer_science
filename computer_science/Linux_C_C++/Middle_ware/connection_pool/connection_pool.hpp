#ifndef CONNECTION_POOL
#define CONNECTION_POOL

#include <iostream>
#include <thread>
#include <memory>
#include <fstream>
#include <chrono>
#include <stdexcept>

#include <mysql/mysql.h>
#include <lock_free_queue.hpp>


// usage :
// auto pool = connect::connection_pool::get_instance("127.0.0.1", "root", "20040618HSF", "web_user");
// connect::connRAII conn{pool};

namespace connect {
    class connection_pool {
    public:
        connection_pool(std::string _host, std::string _user, std::string _passwd, std::string _db, unsigned int _port);
        static connection_pool *get_instance(std::string host, std::string user, std::string passwd, std::string db, unsigned int port = 3306);
        MYSQL *get_conn();
        bool release_conn(MYSQL *conn);
        ~connection_pool();
    private:
        lockfree::lock_free_queue<MYSQL*, false> conn_queue;
        static constexpr int conn_count = 8;
        void init_conn(std::string host, std::string user, std::string passwd, std::string db, unsigned int port);
        void destroy_conn();
    private:
        std::string host;
        std::string user;
        std::string passwd;
        std::string db;
        unsigned int port;
    };


    struct connRAII {
        connRAII(connection_pool *_pool): conn_pool(_pool) {
            conn = conn_pool->get_conn();
        }
        ~connRAII() {
            conn_pool->release_conn(conn);
        }
    private:
        MYSQL *conn;
        connection_pool *conn_pool;
    };


    connection_pool::connection_pool(std::string _host, std::string _user, std::string _passwd, std::string _db, unsigned int _port): 
                                                                    host(_host), user(_user), passwd(_passwd), db(_db), port(_port) {
        init_conn(host, user, passwd, db, port);
    }


    connection_pool *connection_pool::get_instance(std::string host, std::string user, std::string passwd, std::string db, unsigned int port) {
        static connection_pool conn_pool(host, user, passwd, db, port);
        return &conn_pool;
    }


    void connection_pool::init_conn(std::string host, std::string user, std::string passwd, std::string db, unsigned int port) {
        for (int i = 0; i < conn_count; i ++) {
            MYSQL *conn = nullptr;
            if ((conn = mysql_init(nullptr)) == nullptr) 
                throw std::runtime_error("init_error...\n");
            
            if ((conn = mysql_real_connect(conn, &host[0], &user[0], &passwd[0], &db[0], port, nullptr, 0)) == nullptr)
                throw std::runtime_error("init_error...\n");
            
            conn_queue.push(conn);
        }
    }


    MYSQL *connection_pool::get_conn() {
        auto conn_opt = conn_queue.pop();
        while (!conn_opt.has_value()) {
            conn_opt = conn_queue.pop();
            std::cout << "waiting for connection...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << "get a conn\n";
        return conn_opt.value();
    }


    bool connection_pool::release_conn(MYSQL *conn) {
        conn_queue.push(conn);
        std::cout << "release a conn!\n";
        return true;
    }


    void connection_pool::destroy_conn() {
        auto opt = conn_queue.pop();
        while (opt.has_value()) {
            mysql_close(opt.value());
            std::cout << "close conn\n";
            opt = conn_queue.pop();
        }
    }


    connection_pool::~connection_pool() {
        destroy_conn();
    }
}

#endif

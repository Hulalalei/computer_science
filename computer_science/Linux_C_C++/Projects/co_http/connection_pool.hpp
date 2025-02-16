#ifndef CONNECTION_POOL
#define CONNECTION_POOL

#include <thread>
#include <chrono>

#include <mysql/mysql.h>
#include "lock_free_queue.hpp"
#include "minilog.hpp"


// usage :
// auto pool = connect::connection_pool::get_instance("127.0.0.1", "root", "20040618HSF", "web_user");
// connect::connRAII conn{pool};

namespace connsql {
    class connection_pool {
    public:
        connection_pool();
        static connection_pool *get_instance();
        void init_conn(std::string host, std::string user, std::string passwd, std::string db, unsigned int port = 3306);
        MYSQL *get_conn();
        bool release_conn(MYSQL *conn);
        ~connection_pool();
    private:
        lockfree::lock_free_queue<MYSQL*> conn_queue;
        static constexpr int conn_count = 20;
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
        MYSQL *conn;
    private:
        connection_pool *conn_pool;
    };


    connection_pool::connection_pool() {
    }


    connection_pool *connection_pool::get_instance() {
        static connection_pool conn_pool;
        return &conn_pool;
    }


    void connection_pool::init_conn(std::string host, std::string user, std::string passwd, std::string db, unsigned int port) {
        for (int i = 0; i < conn_count; i ++) {
            MYSQL *conn = nullptr;
            conn = mysql_init(nullptr);
            if (!conn) { minilog::log_fatal("init_failed...please check mysql connection!"); throw; }
            conn = mysql_real_connect(conn, &host[0], &user[0], &passwd[0], &db[0], port, nullptr, 0);
            if (!conn) { minilog::log_fatal("connect_failed...please check mysql connection!"); throw; }
            
            conn_queue.push(conn);
        }
    }


    MYSQL *connection_pool::get_conn() {
        auto conn_opt = conn_queue.pop();
        while (!conn_opt.has_value()) {
            conn_opt = conn_queue.pop();
            minilog::log_trace("waiting for connsql");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        minilog::log_trace("get a connsql");
        return conn_opt.value();
    }


    bool connection_pool::release_conn(MYSQL *conn) {
        conn_queue.push(conn);
        minilog::log_trace("release a connsql");
        return true;
    }


    void connection_pool::destroy_conn() {
        auto opt = conn_queue.pop();
        while (opt.has_value()) {
            mysql_close(opt.value());
            minilog::log_info("close a connsql");
            opt = conn_queue.pop();
        }
    }


    connection_pool::~connection_pool() {
        destroy_conn();
    }
}

#endif

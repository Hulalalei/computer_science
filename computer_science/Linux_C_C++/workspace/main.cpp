#include <iostream>
#include <type_traits>
#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>

#include <json/json.h>
#include <mysql/mysql.h>

// #include <queue.hpp>
#include <lock_free_queue.hpp>
// #include "lock_free_queue.hpp"



std::mutex mt;
int k = 0, j = 0;
void speak() {
    std::lock_guard<std::mutex> gd(mt);
    std::cout << "k = " << k << "\n";
    ++ k;
}

struct node {
    int t;
};
struct Info{
    int age;
    node *ptr;
};

void release_sql(MYSQL* conn) {
    std::cout << "release ptr: " << conn << "\n";
    mysql_close(conn);
    conn = nullptr;
}

void test() {
    lockfree::lock_free_queue<int, false> q;    
    // std::thread t1([&]() {
    //     for (int i = 1; i <= 10; i ++) {
    //         MYSQL *conn = nullptr;
    //         conn = mysql_init(nullptr);
    //         if (conn == nullptr) perror("init_err\n");
    //         conn = mysql_real_connect(conn, "127.0.0.1", "root", "20040618HSF", "web_user", 3306, nullptr, 0);
    //         if (conn == nullptr) perror("conn_err\n");
    //         q.push(conn);
    //     }
    // });
    // t1.join();
    // std::thread t2([&]() {
    //     for (int i = 0; i < 10000; i ++) {
    //         auto res = q.pop();
    //         if (res.has_value()) {
    //             std::cout << "ptr: " << res.value() << "\n";
    //             release_sql(res.value());
    //         }
    //     }
    // });
    // t2.join();


    std::thread t3([&]() {
        for (int i = 0; i < 10000; i ++) {
            q.push(i);
        }
    });
    std::thread t4([&]() {
        for (int i = 0; i < 10000; i ++) {
            q.push(i);
        }
    });
    std::thread t5([&]() {
        for (int i = 0; i < 10000; i ++) {
            q.push(i);
        }
    });
    std::thread t6([&]() {
        for (int i = 0; i < 80000; i ++) {
            q.pop();
        }
    });
    std::thread t7([&]() {
        for (int i = 0; i < 80000; i ++) {
            q.pop();
        }
    });
    std::thread t8([&]() {
        for (int i = 0; i < 80000; i ++) {
            q.pop();
        }
    });
    std::thread t9([&]() {
        for (int i = 0; i < 80000; i ++) {
            q.pop();
        }
    });
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
}


int main(int argc, char **argv) {
    auto t1 = std::chrono::steady_clock::now();
    test();
    auto t2 = std::chrono::steady_clock::now();
    std::cout << "times: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1) << "\n";
    return 0;
}

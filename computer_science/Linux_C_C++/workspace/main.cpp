#include <iostream>
#include <type_traits>
#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

#include <json/json.h>
#include <mysql/mysql.h>

// #include <queue.hpp>
#include <lock_free_queue.hpp>



std::mutex mt;
int k = 0, j = 0;
void speak() {
    std::lock_guard<std::mutex> gd(mt);
    std::cout << "k = " << k << "\n";
    ++ k;
}

struct Info {
    int age = 0;
};

void test() {
    Info info;
    int b = 10;
    int *a = &b;
    lockfree::lock_free_queue<MYSQL*, true> q;    

    std::thread t1([&]() {
        for (int i = 1; i <= 10; i ++) {
            MYSQL *conn = nullptr;
            conn = mysql_init(nullptr);
            if (conn == nullptr) perror("init_err\n");
            conn = mysql_real_connect(conn, "127.0.0.1", "root", "20040618HSF", "web_user", 3306, nullptr, 0);
            if (conn == nullptr) perror("conn_err\n");
            q.push(conn);
        }
    });
    t1.join();
    // std::thread t2([&]() {
    //     for (int i = 0; i < 1000; i ++) {
    //         q.push(a);
    //     }
    // });
    // std::thread t3([&]() {
    //     for (int i = 0; i < 1000; i ++) {
    //         q.push(a);
    //     }
    // });
    // std::thread t4([&]() {
    //     for (int i = 0; i < 1000; i ++) {
    //         q.push(a);
    //     }
    // });
    // std::thread t5([&]() {
    //     for (int i = 0; i < 1000; i ++) {
    //         q.push(a);
    //     }
    // });
    // std::thread t6([&]() {
    //     for (int i = 0; i < 40000; i ++) {
    //         auto opt = q.pop();
    //     }
    // });
    // std::thread t7([&]() {
    //     for (int i = 0; i < 40000; i ++) {
    //         auto opt = q.pop();
    //     }
    // });
    // std::thread t8([&]() {
    //     for (int i = 0; i < 40000; i ++) {
    //         auto opt = q.pop();
    //     }
    // });
    // std::thread t9([&]() {
    //     for (int i = 0; i < 40000; i ++) {
    //         auto opt = q.pop();
    //     }
    // });
    // t2.join();
    // t3.join();
    // t4.join();
    // t5.join();
    // t6.join();
    // t7.join();
    // t8.join();
    // t9.join();
}

int main(int argc, char **argv) {
    test();
    std::cout << "k = " << k << " " << "j = " << j << "\n";
    return 0;
}

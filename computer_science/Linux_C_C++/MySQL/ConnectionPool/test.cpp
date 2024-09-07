#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <pthread.h>
#include "ConnectionPool.hpp"


void *execute(void *arg) {
    ConnectionPool *pool = static_cast<ConnectionPool*> (arg);
    for (int i = 0; i != 5000; i ++) {
        std::shared_ptr<MysqlConn> conn = pool->getConnection();
        char sql[1024] = { 0 };
        sprintf(sql, "insert into test values(%d, 25, 'man', 'Tom')", i);
        conn->update(sql);
    }
}

int main() {
    pthread_t t1, t2, t3, t4, t5;
    ConnectionPool *pool = ConnectionPool::getConnectionPool();
    pthread_create(&t1, nullptr, execute, pool);
    pthread_create(&t2, nullptr, execute, pool);
    pthread_create(&t3, nullptr, execute, pool);
    pthread_create(&t4, nullptr, execute, pool);
    pthread_create(&t5, nullptr, execute, pool);
    sleep(30);
    std::cout << "done..." << std::endl;
    return 0;

}
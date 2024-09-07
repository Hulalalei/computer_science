#include <iostream>
#include <memory>
#include <fstream>
#include <stdexcept>
#include <queue>
#include <unistd.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <json/json.h>
#include "MysqlConn.hpp"


// 单例模式
class ConnectionPool {
public:
    static ConnectionPool *getConnectionPool();
    std::shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();
    void addConnection();

    int m_maxNum;
    int m_minNum;
    int m_timeout;
    int m_maxIdleTime;
    pthread_mutex_t poolMutex;
    pthread_cond_t isEmpty, isFull;
    std::queue<MysqlConn*> m_connectionQ;
private:
    ConnectionPool();
    ConnectionPool(const ConnectionPool &) = default;
    ConnectionPool &operator=(const ConnectionPool &) = default;
    static void *produceConnection(void *arg);
    static void *recycleConnection(void *arg);
    bool parseJsonFile();
    
    
    pthread_t produceConn;
    pthread_t recycleConn;

    std::string m_ip;
	std::string m_user;
	std::string m_passwd;
	std::string m_dbName;
	unsigned short m_port;
};

#include "ConnectionPool.hpp"


ConnectionPool *ConnectionPool::getConnectionPool() {
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::parseJsonFile() {
    std::ifstream ifs("dbconf.json", std::ifstream::in);
    Json::Reader rd; Json::Value root;
    rd.parse(ifs, root);

    // 如果root是一个对象
    if (root.isObject()) {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minNum = root["minSize"].asInt();
        m_maxNum = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();

        return true;
    }
    return false;
}

void ConnectionPool::addConnection() {
    MysqlConn *conn = new MysqlConn;
    conn->connection(m_ip, m_user, m_passwd, m_dbName, m_port);
    conn->refreshAliveTime();
    m_connectionQ.push(conn);
}

void *ConnectionPool::produceConnection(void *arg) {
    ConnectionPool *pool = static_cast<ConnectionPool*> (arg);
    while (true) {
        // 外界请求连接数很少时，阻塞
        pthread_mutex_lock(&pool->poolMutex);
        std::cout << "producer get lock\n";
        while (pool->m_connectionQ.size() <= pool->m_minNum) {
            pthread_cond_wait(&pool->isFull, &pool->poolMutex);
            std::cout << "Qsize = " << pool->m_connectionQ.size() << " min = " << pool->m_minNum << "\n";
        }
        pool->addConnection();
        std::cout << "add a connection\n";
        pthread_mutex_unlock(&pool->poolMutex);
        pthread_cond_broadcast(&pool->isEmpty);
    }
}

void *ConnectionPool::recycleConnection(void *arg) {
    ConnectionPool *pool = static_cast<ConnectionPool*> (arg);
    while (true) {
        sleep(300);
        std::cout << "try to catch clock...\n";
        pthread_mutex_lock(&pool->poolMutex);
        std::cout << "recycle get lock\n";
        while (pool->m_connectionQ.size() >= pool->m_minNum) {
            MysqlConn *conn = pool->m_connectionQ.front();
            if (conn->getAliveTime() > pool->m_maxIdleTime) {
                pool->m_connectionQ.pop();
                pthread_cond_broadcast(&pool->isFull);
                delete conn;
                std::cout << "recycle a connection\n";
            }
            else break;
        }
        pthread_mutex_unlock(&pool->poolMutex);
    }
}

ConnectionPool::ConnectionPool() {
    if (!parseJsonFile()) throw std::runtime_error("parse json file error...\n");
    for (int i = 0; i < m_minNum; i ++) {
        addConnection();
    }
    pthread_mutex_init(&poolMutex, nullptr);
    pthread_cond_init(&isFull, nullptr);
    pthread_cond_init(&isEmpty, nullptr);
    pthread_create(&produceConn, nullptr, this->produceConnection, this);
    pthread_create(&recycleConn, nullptr, this->recycleConnection, this);

    pthread_detach(produceConn);
    pthread_detach(recycleConn);
}

// 队列里取一个链接下来
std::shared_ptr<MysqlConn> ConnectionPool::getConnection() {
    while (true) {
        pthread_mutex_lock(&poolMutex);
        while (m_connectionQ.empty()) pthread_cond_wait(&isEmpty, &poolMutex);
        // 链接用完再还回去，conn就是指向那块已经分配的内存
        std::shared_ptr<MysqlConn> connptr(m_connectionQ.front(), [this](MysqlConn *conn) {
            std::cout << "ptr get lock...\n";
            conn->refreshAliveTime();
            m_connectionQ.push(conn);
            });
        m_connectionQ.pop();
        pthread_mutex_unlock(&poolMutex);
        pthread_cond_broadcast(&isFull);
        return connptr;
    }
}

ConnectionPool::~ConnectionPool() {
    while (!m_connectionQ.empty()) {
        MysqlConn *conn = m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }
}

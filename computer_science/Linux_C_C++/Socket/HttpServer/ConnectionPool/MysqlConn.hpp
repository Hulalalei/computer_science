#include <mysql/mysql.h>
#include <chrono>
#include <iostream>

// 数据库API封装
class MysqlConn {
public:
    // 数据库初始化
    MysqlConn();

    // 数据库连接
    bool connection(std::string m_ip, std::string m_user, 
        std::string m_passwd, std::string m_dbName, unsigned short m_port);
    
    // 数据库更新
    bool update(std::string sql);

    // 数据库查询
    bool query(std::string sql);

    // 遍历结果集
    bool next();

    // 获取结果集字段值
    std::string value(int index);

    // 事务操作
    bool transaction();

    // 提交事务
    bool commit();

    // 事务回滚
    bool rollback();

    // 刷新起始空闲时间点
    void refreshAliveTime();

    // 计算连接存货的总时长
    long long getAliveTime();

    // 释放数据库连接
    ~MysqlConn();
private:
    void freeResult();
    MYSQL * m_conn;
    MYSQL_RES *m_result;
    MYSQL_ROW m_row;
    // chrono库下的一个时间点
    std::chrono::steady_clock::time_point m_alivetime;
};

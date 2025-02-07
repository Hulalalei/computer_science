#include <format>
#include <print>
#include <json/json.h>
#include <mysql/mysql.h>
#include <coro_webserver.hpp>
// #include <webserver.hpp>

int main(int argc, char **argv) {
    co_async::webserver server;

    // 日志系统
    server.init_logsystem();
    // 解析Json
    server.init_jsonfile();
	// 数据库
	server.init_connectionpool();
    // 线程池
	server.init_threadpool();
    // 事件监听
	server.eventlisten();
    // 事件回环
    co_async::task t = server.eventloop();
    while (!t.m_co.done()) {
        t.m_co.resume();
    }

 //    co_asyncweb::webserver server;
 //    server.init();
 //    // 日志系统
 //    server.log_system();
	// // 数据库
	// server.sql_pool();
 //    // 线程池
	// server.thread_pool();
 //    // 事件监听
	// server.event_listen();
 //    // 事件回环
 //    co_asyncweb::task t = server.event_loop();
 //    while (!t.m_co.done()) t.m_co.resume();
	return 0;
}

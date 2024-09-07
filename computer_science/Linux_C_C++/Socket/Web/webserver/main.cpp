#include <iostream>
#include <arpa/inet.h>
#include <atomic>
#include <thread>
#include <cassert>
#include "webserver.hpp"


int main(int argc, char **argv) {
    web::webserver server;
    std::cout << "ok1\n";
    server.init();

	//数据库
    std::cout << "ok1\n";
	server.sql_pool();

    std::cout << "ok1\n";
	server.thread_pool();

    std::cout << "ok1\n";
	server.event_listen();

    std::cout << "ok1\n";
	server.event_loop();
	return 0;
}

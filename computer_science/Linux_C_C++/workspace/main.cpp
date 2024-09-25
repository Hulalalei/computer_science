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

#include <lock_free_queue.hpp>
#include <connection_pool.hpp>
#include <thread_pool.hpp>
#include <memory_pool.hpp>
#include <minilog.hpp>


void test() {
    memory::memory_pool pool;
    memory::mem_allocate allc(&pool);
}


int main(int argc, char **argv) {
    test();
    return 0;
}

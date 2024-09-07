#include <iostream>
#include <cstring>
#include "MemoryPool.hpp"


int main(int argc, char *argv[]) {
    MemoryPool *pool = new MemoryPool;
    void *p1 = pool->_malloc(32);
    
    std::cout << cur_memnum;

    delete pool;
    return 0;
}
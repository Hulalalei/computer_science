#include <iostream>
#include <cstring>
#include <stdexcept>
#include <pthread.h>
#include <unistd.h>

const int SIZE = 2;
static int cur_memnum = 1;

class MemoryPool {
public:
    MemoryPool();
    void *_malloc(size_t size);
    void _free(void *ptr);
    ~MemoryPool();
private:
    MemoryPool(const MemoryPool &obj) = delete;
    MemoryPool &operator=(const MemoryPool &obj) = delete;
    static void *memAllocate(void *arg);
    void *cur_ptr;
    void *mem_ptr[SIZE];
    const int block_size;
    int free_count;
    bool flag;
    pthread_t manager;
    pthread_mutex_t memMutex;
};

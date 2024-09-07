#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <chrono>
#include <stdexcept>

const int SIZE = 2;
static int cur_memnum = 1;

class memory_pool {
public:
    memory_pool();
    void *_new(size_t size);
    void _delete(void *ptr);
    ~memory_pool();
private:
    memory_pool(const memory_pool &obj) = default;
    memory_pool &operator=(const memory_pool &obj) = default;
    static void *memAllocate(void *arg);
    void *cur_ptr;
    void *mem_ptr[SIZE];
    const int block_size;
    int free_count;
    bool flag;
    std::thread manager;
    std::mutex memMutex;
};


const int PAGE_SIZE = 0x1000, BLOCK_SIZE = 32;


void *memory_pool::memAllocate(void *arg) {
    memory_pool *pool = static_cast<memory_pool*> (arg);
    while (!pool->flag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(pool->memMutex);
        std::cout << "manager get lock...\n";


        if (pool->free_count == 0 && cur_memnum < SIZE) {
            std::cout << "begin to create memory...\n";
            pool->mem_ptr[++ cur_memnum - 1] = malloc(PAGE_SIZE);
            pool->cur_ptr = pool->mem_ptr[cur_memnum - 1];
            pool->free_count = PAGE_SIZE / pool->block_size;

            char *ptr = static_cast<char*> (pool->mem_ptr[cur_memnum - 1]);


            for (size_t i = 0; i < pool->free_count; i ++) {
                *(char **)ptr = static_cast<char*> (ptr + pool->block_size);
                ptr += pool->block_size;
            }
            *(char **)ptr = nullptr;
        }


        else if (pool->free_count == 0 && cur_memnum == SIZE)
            throw std::runtime_error("memory is already drained...\n");
    }
}

memory_pool::memory_pool(): cur_ptr(nullptr), flag(false),
                            block_size(BLOCK_SIZE), free_count(PAGE_SIZE / BLOCK_SIZE), manager(memAllocate, this) {
    memset(mem_ptr, 0, sizeof(mem_ptr));
    mem_ptr[cur_memnum - 1] = malloc(PAGE_SIZE);
    cur_ptr = mem_ptr[cur_memnum - 1];


    char *ptr = static_cast<char*> (mem_ptr[cur_memnum - 1]);


    for (size_t i = 0; i < free_count; i ++) {
        *(char **)ptr = static_cast<char*> (ptr + block_size);
        ptr += block_size;
    }
    *(char **)ptr = nullptr;
    manager.detach();
}

void *memory_pool::_new(size_t size) {
    std::lock_guard<std::mutex> lock(memMutex);
    std::cout << "malloc get lock...\n";
    std::cout << free_count << "\n";
    if (block_size < size || free_count == 0) {
        std::cout << "malloc err...trying to get memory...\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return nullptr;
    }

    void *ptr = cur_ptr;
    cur_ptr   = *(char **)ptr;
    free_count --;
    return ptr;
}

void memory_pool::_delete(void *ptr) {

    std::lock_guard<std::mutex> lock(memMutex);
    std::cout << "free get lock...\n";
    *(char **)ptr = static_cast<char*> (cur_ptr);
    cur_ptr       = ptr;
    free_count ++;
}

memory_pool::~memory_pool() {
    flag = true;

    cur_ptr = nullptr;
    for (size_t i = 0; i < SIZE; i ++) {
        if (mem_ptr[i] != nullptr) {
            free(mem_ptr[i]);
            mem_ptr[i] = nullptr;
        }
    }
}
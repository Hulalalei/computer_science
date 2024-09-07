#include "MemoryPool.hpp"


const int PAGE_SIZE = 0x1000, BLOCK_SIZE = 32;

void *MemoryPool::memAllocate(void *arg) {
    MemoryPool *pool = static_cast<MemoryPool*> (arg);
    while (!pool->flag) {
        sleep(2);
        pthread_mutex_lock(&pool->memMutex);
        std::cout << "manager get lock...\n";
        if (pool->free_count == 0 && cur_memnum < SIZE) {
            std::cout << "begin to create memory...\n";
            pool->mem_ptr[++ cur_memnum - 1] = malloc(PAGE_SIZE);
            pool->cur_ptr = pool->mem_ptr[cur_memnum - 1];
            pool->free_count = PAGE_SIZE / pool->block_size;
            // void *指针不能进行算术运算
            char *ptr = static_cast<char*> (pool->mem_ptr[cur_memnum - 1]);
            
            // 形成内存块链表
            for (size_t i = 0; i < pool->free_count; i ++) {
                *(char **)ptr = static_cast<char*> (ptr + pool->block_size);
                ptr += pool->block_size;
            }
            *(char **)ptr = nullptr;
        }
        else if (pool->free_count == 0 && cur_memnum == SIZE)
            throw std::runtime_error("memory is already drained...\n");
        pthread_mutex_unlock(&pool->memMutex);
    }
}

MemoryPool::MemoryPool(): cur_ptr(nullptr), flag(false), 
                            block_size(BLOCK_SIZE), free_count(PAGE_SIZE / BLOCK_SIZE) {
    memset(mem_ptr, 0, sizeof(mem_ptr));
    mem_ptr[cur_memnum - 1] = malloc(PAGE_SIZE);
    cur_ptr = mem_ptr[cur_memnum - 1];

    // void *指针不能进行算术运算
    char *ptr = static_cast<char*> (mem_ptr[cur_memnum - 1]);

    // 形成内存块链表
    for (size_t i = 0; i < free_count; i ++) {
        *(char **)ptr = static_cast<char*> (ptr + block_size);
        ptr += block_size;
    }
    *(char **)ptr = nullptr;

    pthread_mutex_init(&memMutex, nullptr);
    pthread_create(&manager, nullptr, memAllocate, this);
}

void *MemoryPool::_malloc(size_t size) {
    pthread_mutex_lock(&memMutex);
    std::cout << "malloc get lock...\n";
    std::cout << free_count << "\n";
    if (block_size < size || free_count == 0) {
        std::cout << "malloc err...trying to get memory...\n";
        pthread_mutex_unlock(&memMutex);
        sleep(3);
        return nullptr;
    }

    void *ptr = cur_ptr;
    cur_ptr   = *(char **)ptr;  // 指向下一个链块
    free_count --;
    pthread_mutex_unlock(&memMutex);
    return ptr;
}

void MemoryPool::_free(void *ptr) {
    // 头插
    pthread_mutex_lock(&memMutex);
    std::cout << "free get lock...\n";
    *(char **)ptr = static_cast<char*> (cur_ptr);
    cur_ptr       = ptr;
    free_count ++;
    pthread_mutex_unlock(&memMutex);
}

MemoryPool::~MemoryPool() {
    flag = true;
    pthread_join(manager, nullptr);
    pthread_mutex_destroy(&memMutex);
    
    cur_ptr = nullptr;
    for (size_t i = 0; i < SIZE; i ++) {
        if (mem_ptr[i] != nullptr) {
            free(mem_ptr[i]);
            mem_ptr[i] = nullptr;
        }
    }
}
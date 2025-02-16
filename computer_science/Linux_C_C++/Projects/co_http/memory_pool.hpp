#ifndef MEMORY_POOL
#define MEMORY_POOL

#include <mutex>
#include <variant>

#include <jemalloc/jemalloc.h>
#include "minilog.hpp"


namespace memory {
    // 第一级配置器
    class default_alloc {
    public:
        template <class T, class... Args> static T *new_obj(Args&&... args) {
            T *res =  (T*)je_malloc(sizeof(T));
            new (res) T(args...);
            return res;
        }
        template <class T> static void delete_obj(T *ptr) {
            je_free(ptr);
        }
    };



    // 第二级配置器
    constexpr int align = 8;
    constexpr int max_bytes = 128;
    constexpr int nfreelists = max_bytes / align;
    std::mutex mt;
    
    class memory_pool {
    private:
        static size_t round_up(size_t bytes) {
            return ( (bytes + align - 1) & ~ (align - 1));
        }
    private:
        union obj {
            union obj *free_list_link;
            char client_data[1];
        };
    private:
        static obj *volatile free_list[nfreelists];
        static size_t freelist_index(size_t bytes) {
            return ( (bytes + align - 1) / align - 1 );
        }
        static void *refill(size_t n);
        static char *chunk_alloc(size_t size, int &nobjs);

        static char *start_free;
        static char *end_free;
        static size_t heap_size;
    public:
        static void *allocate(size_t n);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
        template <class T, class... Args> static T *new_obj(Args&&... args);
        template <class T> static void delete_obj(T*);
    };



    char *memory_pool::start_free = 0;
    char *memory_pool::end_free = 0;
    size_t memory_pool::heap_size = 0;
    memory_pool::obj *volatile memory_pool::free_list[nfreelists] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


    template <class T, class... Args>
    T *memory_pool::new_obj(Args&&... args) {
        T *ptr = static_cast<T*>(allocate(sizeof(T)));
        new (ptr) T(std::forward<Args>(args)...);
        return ptr;
    }


    template <class T>
    void memory_pool::delete_obj(T *p) {
        if (p != nullptr) {
            p->~T();
            deallocate(p, sizeof(T));
        }
    }


    void *memory_pool::allocate(size_t n) {
        obj *volatile * my_free_list;
        obj *result;

        if (n > (size_t)max_bytes) {
            minilog::log_warning("too large!");        
            return (je_malloc(n));
        }

        my_free_list = free_list + freelist_index(n);

        std::lock_guard<std::mutex> guard(mt);
        result = *my_free_list;

        if (result == 0) {
            void *r = refill(round_up(n));
            return r;
        }

        *my_free_list = result->free_list_link;
        return (result);
    }


    void memory_pool::deallocate(void *p, size_t n) {
        obj *q = (obj *)p;
        obj * volatile *my_free_list;
        if (n > (size_t)max_bytes) {
            je_free(p);
            return;
        }

        my_free_list = free_list + freelist_index(n);
        std::lock_guard<std::mutex> guard(mt);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }


    void *memory_pool::refill(size_t n) {
        int nobjs = 20;
        char *chunk = chunk_alloc(n, nobjs);
        obj * volatile * my_free_list;
        obj *result;
        obj *current_obj, *next_obj;
        int i = 0;

        if (1 == nobjs) return (chunk);

        my_free_list = free_list + freelist_index(n);
        result = (obj*)chunk;
        *my_free_list = next_obj = (obj*)(chunk + n);
        for (i = 1; ; i ++) {
            current_obj = next_obj;
            next_obj = (obj*)((char*)next_obj + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = 0;
                break;
            }
            else {
                current_obj->free_list_link = next_obj;
            }
        }
        return (result);
    }

 
    char *memory_pool::chunk_alloc(size_t size, int &nobjs) {
        minilog::log_trace("unused memory is empty, get a chunk from memory pool");
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        if (bytes_left >= total_bytes) {
            minilog::log_trace("memroy pool is enough");
            result = start_free;
            start_free += total_bytes;
            return (result);
        }
        else if (bytes_left >= size) {
            minilog::log_warning("memory pool is not enough, but can only supply a block memory");
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return (result);
        }
        else {
            minilog::log_warning("get os memory");
            size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
            if (bytes_left > 0) {
                obj * volatile *my_free_list = free_list + freelist_index(bytes_left);
                ((obj*)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }

            start_free = (char*)je_malloc(bytes_to_get);
            if (0 == start_free) {
                int i = 0;
                obj * volatile *my_free_list, *p;

                // 遍历size大小往上的块的链表
                for (i = size; i <= max_bytes; i += align) {
                    my_free_list = free_list + freelist_index(i);
                    p = *my_free_list;

                    if (0 != p) {
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        return (chunk_alloc(size, nobjs));
                    }
                }
                minilog::log_fatal("os memory is empty, program will be termination!");
                end_free = 0;
                start_free = (char*)je_malloc(bytes_to_get);
            }

            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }
}
#endif

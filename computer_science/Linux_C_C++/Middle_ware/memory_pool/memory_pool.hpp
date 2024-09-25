#include <stdexcept>



namespace memory {
    class memory_pool {
        char buf[65535];
    };


    struct block {
        block(){}
        block(int bg, int ed): begin_idx(bg), end_idx(ed), free(true) {}
        int begin_idx;
        int end_idx;
        bool free;
    };
    struct block_ptr {
        block_ptr(){}
        block_ptr(block _bk): bk(_bk) {}
        block bk;
        block next;
    };


    // template <class T>
    class mem_allocate {
    private:
        memory_pool *pool;
        // 最小单位：1字长
        int minblock_size = 64;
        // 索引表
        block_ptr tab_ptr[16];
        block_ptr node_64, node_128, node_256, node_512, node_1024;
        // 当前空闲指针
        int cur_free;
    public:
        mem_allocate(memory_pool *_pool): pool(_pool), cur_free(0) {
            block b;
            node_64(b);
            // tab_ptr[1] = node_64;
            // cur_free += 64;
            // node_128(block(cur_free, cur_free + 128));
            // tab_ptr[2] = node_128;
            // cur_free += 128;
            // node_256(block(cur_free, cur_free + 256));
            // tab_ptr[3] = node_256;
            // cur_free += 256;
            // node_512(block(cur_free, cur_free + 512));
            // tab_ptr[4] = node_512;
            // cur_free += 512;
            // node_1024(block(cur_free, cur_free + 1024));
            // tab_ptr[5] = node_1024;
            // cur_free += 1024;
        }

        int allocate(size_t n) {
            // if (n > 1024) throw std::runtime_error("bad allocate!");
            // if (n <= 64) {
            //     int bg_idx = 0;
            //     if (!tab_ptr[0].bk.free) {
            //         block_ptr node(block(cur_free, cur_free + 64));
            //         node.next = tab_ptr;
            //         tab_ptr = node;
            //         bg_idx = tab_ptr[0].bk.begin_idx;
            //     }
            //     else bg_idx = tab_ptr[0].bk.begin_idx;
            //     return bg_idx;
            // }
        }
        
        void deallocate() {
            
        }
    };
}

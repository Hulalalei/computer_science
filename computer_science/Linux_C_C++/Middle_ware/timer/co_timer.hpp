#ifndef COTIMER
#define COTIMER
#include <coro_promise.hpp>
#include <functional>
#include <iostream>
#include <chrono>
#include <set>
#include <ctime>

#include <http_conn.hpp>
#include <minilog.hpp>

namespace co_async {
    struct client_data;
    struct timer_node_base {
        // 过期时间
        time_t expire;
        int id;
    };

    struct timer_node : timer_node_base {
        using callback = std::function<void (const timer_node&)>;
        callback func;
        client_data *user_data;
    };

    bool operator <(const timer_node_base& lhs, const timer_node_base& rhs) {
        if (lhs.expire > rhs.expire) return false;
        else if (lhs.expire < rhs.expire) return true;
        else return lhs.id < rhs.id;
    }

    class timer {
    public:
        static time_t get_tick();
        timer_node_base add_timer(timer_node &node);
        bool del_timer(timer_node_base &node);
        bool check_timer();
        time_t time_to_sleep();
        int get_id() { return ++ gid; }
    private:
        static int gid;
        std::set<timer_node, std::less<> > timer_map;
    };


    struct client_data {
        sockaddr_in address;
        int sockfd;
        timer_node *client_timer;
    };

    int timer::gid = 0;

    time_t timer::get_tick() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    timer_node_base timer::add_timer(timer_node &node) {
        node.id = get_id();
        timer_map.insert(node);
        return static_cast<timer_node_base>(node);
    }

    bool timer::del_timer(timer_node_base &node) {
        auto iter = timer_map.find(node);
        if (iter != timer_map.end()) {
            timer_map.erase(iter);
            return true;
        }
        return false;
    }

    bool timer::check_timer() {
        auto iter = timer_map.begin();
        if (iter != timer_map.end() && iter->expire <= get_tick()) {
            iter->func(*iter);
            timer_map.erase(iter);
            return true;
        }
        return false;
    }

    time_t timer::time_to_sleep() {
        auto iter = timer_map.begin();
        if (iter == timer_map.end()) return -1;
        time_t dist = iter->expire - get_tick();
        return dist > 0 ? dist : 0;
    }
}
#endif

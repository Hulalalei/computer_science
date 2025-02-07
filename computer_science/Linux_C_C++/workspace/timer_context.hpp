#pragma once
#include "stop_source.hpp"
#include <chrono>
#include <map>
#include <callback.hpp>


struct timer_context {
    using TIME_POINT = std::chrono::steady_clock::time_point;
    using DURATION = std::chrono::steady_clock::duration;

    struct __timer_entry {
        // 回调
        callback<> m_cb;
        stop_source m_stop;
    };
    std::multimap<TIME_POINT, __timer_entry> m_timer_heap;

    timer_context() = default;
    timer_context(timer_context &&) = delete;


    // 默认stop是不设置回调
    void set_timeout(DURATION dt, callback<> cb, stop_source stop = {}) {
        auto expired_time = std::chrono::steady_clock::now() + dt;
        auto it = m_timer_heap.insert({expired_time, __timer_entry{std::move(cb), stop}});
        // 回调：设置定时器里的回调
        stop.set_stop_callback([this, it] {
            auto cb = std::move(it->second.m_cb);
            m_timer_heap.erase(it);
            cb();
        });
    }

    DURATION duration_to_next_timer() {
        while (!m_timer_heap.empty()) {
            auto it = m_timer_heap.begin();
            auto now = std::chrono::steady_clock::now();
            // 所有到期的定时器，执行回调
            if (it->first <= now) {
                it->second.m_stop.clear_stop_callback();
                auto cb = std::move(it->second.m_cb);
                m_timer_heap.erase(it);
                cb();
            } else {
                return it->first - now;
            }
        }
        return std::chrono::nanoseconds(-1);
    }

    bool is_empty() const {
        return m_timer_heap.empty();
    }
};

#pragma once

#include <coroutine>
#include <memory>

struct stop_source {
    struct _control_block {
        bool m_stop = false;
        std::coroutine_handle<> m_handle;
    };

    std::shared_ptr<_control_block> m_control;

    stop_source() = default;

    explicit stop_source(std::in_place_t)
        : m_control(std::make_shared<_control_block>()) {}

    static stop_source make() {
        return stop_source(std::in_place);
    }

    bool stop_requested() const noexcept {
        return m_control && m_control->m_stop;
    }

    bool stop_possible() const noexcept {
        return m_control != nullptr;
    }

    void request_stop() const {
        if (!m_control) {
            return;
        }
        m_control->m_stop = true;
        if (m_control->m_handle) {
            m_control->m_handle.resume();
            m_control->m_handle = nullptr;
        }
    }

    void set_stop_callback(std::coroutine_handle<> handle) const noexcept {
        if (!m_control) {
            return;
        }
        m_control->m_handle = handle;
    }

    void clear_stop_callback() const noexcept {
        if (!m_control) {
            return;
        }
        m_control->m_handle = nullptr;
    }
};

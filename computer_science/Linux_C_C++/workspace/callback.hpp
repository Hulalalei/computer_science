#pragma once

#include <memory>
#include <utility>
#include <type_traits>
#include <cassert>


inline constexpr struct multishot_call_t {
    explicit multishot_call_t() = default;
} multishot_call;


template <class ...Args>
struct callback {
    struct __callback_base {
        virtual void __call(Args ...args) = 0;
        virtual ~__callback_base() = default;
    };

    // 提供一个__callback_base接口，剩下模板继承
    template <class F>
    struct __callback_impl final : __callback_base {
        F m_func;

        // 泛型构造: m_func = other.func
        template <class... Ts, class = std::enable_if_t<std::is_constructible_v<F, Ts...>>>
        __callback_impl(Ts &&...ts): m_func(std::forward<Ts>(ts)...) {}

        void __call(Args ...args) override {
            m_func(std::forward<Args>(args)...);
        }
    };

    std::unique_ptr<__callback_base> m_base;


    template <class F, class = std::enable_if_t<std::is_invocable_v<F, Args...> && !std::is_same_v<std::decay_t<F>, callback>>>
    callback(F &&f): m_base(std::make_unique<__callback_impl<std::decay_t<F>>>(std::forward<F>(f))) {}

    callback() = default;
    callback(std::nullptr_t) noexcept {}
    callback(callback &) = delete;
    callback &operator=(callback &) = delete;
    callback(callback &&) = default;
    callback &operator=(callback &&) = default;

    void operator()(Args ...args) {
        assert(m_base);
        m_base->__call(std::forward<Args>(args)...);
        m_base = nullptr;
    }

    void operator()(multishot_call_t, Args ...args) const {
        assert(m_base);
        m_base->__call(std::forward<Args>(args)...);
    }

    void *getaddress() const noexcept {
        return static_cast<void *>(m_base.get());
    }

    void *leak_address() const {
        return static_cast<void *>(m_base.release());
    }

    static callback from_address(void *addr) noexcept {
        callback cb;
        cb.m_base = std::unique_ptr<__callback_base>(static_cast<__callback_base *>(addr));
        return cb;
    }

    explicit operator bool() const noexcept {
        return m_base != nullptr;
    }
};

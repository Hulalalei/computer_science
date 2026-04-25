#include <print>
#include <coroutine>

template <class T = void> struct NonVoidHelper {
    using Type = T;
};

template <> struct NonVoidHelper<void> {
    using Type = NonVoidHelper;

    explicit NonVoidHelper() = default;
};


template <class A>
concept Awaiter = requires(A a, std::coroutine_handle<> h) {
    { a.await_ready() };
    { a.await_suspend(h) };
    { a.await_resume() };
};

template <class A>
concept Awaitable = Awaiter<A> || requires(A a) {
    { a.operator co_await() } -> Awaiter;
};

template <class A> struct AwaitableTraits;

template <Awaiter A> struct AwaitableTraits<A> {
    using RetType = decltype(std::declval<A>().await_resume());
    using NonVoidRetType = NonVoidHelper<RetType>::Type;
};

template <class A>
    requires(!Awaiter<A> && Awaitable<A>)
struct AwaitableTraits<A>
    : AwaitableTraits<decltype(std::declval<A>().operator co_await())> {};

int main() {
}

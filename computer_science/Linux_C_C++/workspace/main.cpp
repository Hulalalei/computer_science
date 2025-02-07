#include <asm-generic/socket.h>
#include <cerrno>
#include <csignal>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <print>

#include <expected.hpp>
#include <io_context.hpp>
#include <callback.hpp>
#include <stop_source.hpp>
#include <timer_context.hpp>


async_file m_conn;
void do_read();

void handle_ctrl(int sig) {
    std::println("catch the ctrl + C signal");
    throw;
}

char buffer01[] = "HTTP/1.1 200 OK\r\nServer: co_http\r\nContent-type: text/html;charset=utf-8\r\nConnection: keep-alive\r\nContent-length: 5\r\n\r\nhello";
std::span<char> buf_write{buffer01};
void do_write() {
    // std::println("my response: \n{}", std::string_view{buf_write.data(), buf_write.size()});

    return m_conn.async_write(buf_write, [](expected<size_t> ret) {
        if (ret.error()) {
            // std::println("read error");
            return;
        }
        auto n = ret. value();
        if (n == buf_write.size()) {
            // std::println("write done!");
            return do_read();
        }
        return do_write();
    });
}


char buffer02[1024];
std::span<char> buf_read{buffer02};
void do_read() {
    stop_source stop_io(std::in_place);
    stop_source stop_timer(std::in_place);
    // 超时后执行io stop
    io_context::get().set_timeout(std::chrono::seconds(10), [stop_io] {
        stop_io.request_stop();
    }, stop_timer);

    return m_conn.async_read(buf_read, [stop_timer] (expected<size_t> ret) {
        // read失败后执行此回调
        // 如果io_stop停止了，就执行timer_stop回调
        stop_timer.request_stop();
        if (ret.error()) {
            // std::println("read error, give up the connection: {}", strerror(-ret.error()));
            return;
        }

        size_t n = ret.value();
        if (n == 0) {
            // std::println("client side has cut the connect");
            return;
        }
        // std::println("has read the info: \n{}", std::string_view{buf_read.data(), 200});
        do_write();

    }, stop_io);
}

address_resolver::address t{};
void do_accept(async_file &web) {
    return web.async_accept(t, [&web](expected<int> ret) {
        auto connfd = ret.expect("accept");
        m_conn = async_file{connfd};
        do_read();
        return do_accept(web);
    });
}


int main(int argc, char **argv) {
    signal(SIGINT, handle_ctrl);

    io_context icx{};
    address_resolver resolver;
    auto adrinfo = resolver.resolve("127.0.0.1", "8080");
    auto async_web = async_file::async_bind(adrinfo);
    do_accept(async_web);

    std::println("start the server now");
    icx.join();

    return 0;
}

#include "io_context.hpp"
#include "http_server.hpp"
#include "file_utils.hpp"
#include "minilog.hpp"
#include <csignal>
#include <print>



void server() {
    io_context ctx;
    auto server = http_server::make();
    chdir("../res/");
    // request中加入flag判断，是请求路由还是文件
    server->get_router().route("/", [](http_server::http_request &request) {
        std::string response, content_type;
        content_type = file_get_webtype(request.url);

        if (request.body.empty()) {
        // GET
            // 判断是否 是默认路由
            if (request.url == "/") { 
                content_type = file_get_webtype(".html");
                response = file_get_content("LogIn.html"); 
            } else {
                response = file_get_content(request.url.substr(1));
            }
        } else {
        // POST
            minilog::log_debug("POST request: {}", request.body);
            auto maps = extract_post(request.body);
            minilog::log_debug("key: {}, value: {}", maps.first, maps.second);

            // 连接池判断

            response = file_get_content("index.html");
        }
        request.write_response(200, response, content_type);
    });
    std::println("正在监听：http://127.0.0.1:8080");
    server->do_start("127.0.0.1", "8080");
    // server->do_start("192.168.43.248", "8080");

    ctx.join();
}

int main() {
    // try {
        server();
    // } catch (std::system_error const &e) {
    //     fmt::println("{} ({}/{})", e.what(), e.code().category().name(),
    //                  e.code().value());
    // }
    return 0;
}

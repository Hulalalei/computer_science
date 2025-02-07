#include "io_context.hpp"
#include "http_server.hpp"
#include "file_utils.hpp"

void server() {
    io_context ctx;
    auto server = http_server::make();
    chdir("../res/");
    server->get_router().route("/", [](http_server::http_request &request) {
        std::string response;
        if (request.body.empty()) {
            response = file_get_content("LogIn.html");
        } else {
            response = "你好，你的请求是: [" + request.body
                + "]，共 " + std::to_string(request.body.size())
                + " 字节";
        }
        request.write_response(200, response, "text/html");
    });
    server->get_router().route("/style.css", [](http_server::http_request &request) {
        std::string response;
        if (request.body.empty()) {
            response = file_get_content("style.css");
        } else {
            response = "你好，你的请求是: [" + request.body
                + "]，共 " + std::to_string(request.body.size())
                + " 字节";
        }
        request.write_response(200, response, "text/css");
    });
    server->get_router().route("/login.cgi", [](http_server::http_request &request) {
        std::string response = "hello world";
        if (request.body.empty()) {
            response = file_get_content("style.css");
        } else {
            response = "你好，你的请求是: [" + request.body
                + "]，共 " + std::to_string(request.body.size())
                + " 字节";
        }
        request.write_response(200, response);
    });
    // fmt::println("正在监听：http://127.0.0.1:8080");
    server->do_start("127.0.0.1", "8080");

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

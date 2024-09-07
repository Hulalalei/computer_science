#include <iostream>
#include <unistd.h>
#include "HttpServer01.hpp"


int main(int argc, char **argv) {
    HttpServer *http(new HttpServer);
    http->provide_server();
    
    return 0;
}
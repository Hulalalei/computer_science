#include <iostream>
#include <fstream>
#include <string>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <json/json.h>



bool init(std::string &addr, int &port, struct iovec *vec) {
    std::ifstream ifs("./conf.json", std::ios::in);

    Json::Reader rd; Json::Value root;
    rd.parse(ifs, root);
    addr = root["addr"].asString();
    port = root["port"].asInt();
    std::cout << addr << ":" << port << "\n";
    return true;

    // auto t3 = root["UIR"].asString();
    // vec[3].iov_base = &t3[0];
    // vec[3].iov_len = root["uir_idx"].asInt();
    //
    // auto t5 = root["Accept"].asString();
    // vec[4].iov_base = &t5[0];
    // vec[4].iov_len = root["acc_idx"].asInt();
    //
    // auto t6 = root["Accept-Encoding"].asString();
    // vec[5].iov_base = &t6[0];
    // vec[5].iov_len = root["encode_idx"].asInt();
    //
    // auto t7 = root["Accept-Language"].asString();
    // vec[6].iov_base = &t7[0];
    // vec[6].iov_len = root["lan_idx"].asInt();

}


void conn(int &sock, struct sockaddr_in &serv_addr, std::string &addr, int &port) {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        std::cout << "conn err\n";
}

void request(int &sock, struct iovec *vec, std::string &buf) {
    writev(sock, vec, 4);
    std::cout << "send ok\n";
    while (recv(sock, &buf[0], buf.size(), 0) != 0)
        std::cout << buf << "\n";
    std::cout << "receive ok\n";
}

// requests
int main() {
    std::string address;
    int port = 0;
    struct iovec vec[4];
    init(address, port, vec);

    std::ifstream ifs("./conf.json", std::ios::in);
    Json::Reader rd; Json::Value root;
    rd.parse(ifs, root);
    auto t0 = root["GET"].asString();
    vec[0].iov_base = &t0[0];
    vec[0].iov_len = root["get_idx"].asInt();
    auto t1 = root["Host"].asString();
    vec[1].iov_base = &t1[0];
    vec[1].iov_len = root["host_idx"].asInt();
    auto t2 = root["Connection"].asString();
    vec[2].iov_base = &t2[0];
    vec[2].iov_len = root["conn_idx"].asInt();
    char buf[4] = "\r\n";
    vec[3].iov_base = buf;
    vec[3].iov_len = 2;
    ifs.close();

    std::cout << t0 << t1  << t2 << "\n";

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) std::cout << "sock err\n"; 
    struct sockaddr_in serv_addr;
    std::string buffer;
    buffer.resize(20480);
    memset(&serv_addr, 0, sizeof(serv_addr));

    conn(sock, serv_addr, address, port);
    request(sock, vec, buffer);

    return 0;
}

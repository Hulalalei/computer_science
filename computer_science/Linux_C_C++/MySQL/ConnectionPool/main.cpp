#include <iostream>
#include <mysql/mysql.h>

int main(int argc, char *argv[]) {
    MYSQL *mysql = mysql_init(nullptr);
    MYSQL *ptr =  mysql_real_connect(mysql, "localhost", "root", "20040618", "root", 3306, nullptr, 0);
    if (ptr == nullptr) std::cout << "err\n";
    return 0;
}
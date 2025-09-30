#include <iostream>
#include <unistd.h>

#define BUFFSIZE 4096

int main(int argc, char **argv) {
    std::cout << "argc: " << argc << std::endl;
    int n{};
    char buf[BUFFSIZE]{};

    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, BUFFSIZE) != n)
            std::cout << "write error\n";
    }
    if (n < 0)
        std::cout << "read error\n";
    exit(0);
}

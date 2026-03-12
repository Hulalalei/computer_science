#include <string>
#include <thread>
#include <unistd.h>


class RDT_protocol {
    struct transfer_data {
        int type;
        int seq; // start with 1
        int len;
        std::string data;
    };

    void Sender() { // lock
        while (1) {
            // send data
            // receive ack
        }

    }

    void Receiver() { // lock
        while (1) {
            // receive data
            // send ack
        }
    }
public:
    void run() {
        
    }
};


int main() {
    pause();
    _exit(0);
}

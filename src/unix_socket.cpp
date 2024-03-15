#include "unix_socket.h"
#include <chrono>
#include <thread>

UnixSocketServer::UnixSocketServer() {
    std::cout << "Creating Unix socket for comms with python interface...\n";
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, "/tmp/snr_socket.sock", sizeof(server_addr.sun_path) - 1);
    
    // Remove old socket file if it exists
    unlink(server_addr.sun_path);
    if(bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}
// void UnixSocketServer::sendMessage(const void *msg , size_t len) {
//     if(&msg != NULL)
//     {
//         int bytes_sent = sendto(sock, msg, len, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
//         if(bytes_sent != len) {
//             perror("sendto");
//             exit(EXIT_FAILURE);
//         }
//     }
    
//     std::this_thread::sleep_for(std::chrono::milliseconds(50));
// }

void UnixSocketServer::sendMessage(uint8_t message) {
    // Convert uint8_t to void *
    void *msg = static_cast<void*>(&message);
    size_t len = sizeof(message); 

    // Original if condition and sending the message
    if(msg != NULL)
    {
        int bytes_sent = sendto(sock, msg, len, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
        if(bytes_sent != len) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

uint8_t UnixSocketServer::AvgSnrArray(const void *snr, size_t len){
    const uint8_t* snr_data = static_cast<const uint8_t*>(snr);
    uint8_t sum_ = 0;
    uint8_t numVals = 0;

    for(size_t i = 0; i < len; ++i)
    {
        uint8_t currVal = snr_data[i];
        if (currVal != 0)
        {
            sum_ += currVal;
            ++numVals;
        }
    }
    uint8_t avg = sum_ / numVals;
    return avg;
}


UnixSocketServer::~UnixSocketServer() {
    close(sock);
    unlink(server_addr.sun_path);
}

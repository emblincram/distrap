#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

namespace application {
namespace discovery {
class Handshake {
   public:
    struct Identifier {
        int own;
        int peer;
    };

    Handshake(const Identifier&);
    ~Handshake();

    void start();
    void stop();

   private:
    const std::string broadcastAddr{"255.255.255.255"};
    const Identifier& port;

    uint32_t get_own_ip();
    void receive_loop();
    void send_broadcast();
    bool wait_for_ack(const uint16_t);

    bool hasReceivedRequest = false;
    bool hasReceivedAck = false;

    std::string broadcastAddress;
    int sock;
    std::atomic<bool> running;
    std::thread recvThread;
};
}  // namespace discovery
}  // namespace application

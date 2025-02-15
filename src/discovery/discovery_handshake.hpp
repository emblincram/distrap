#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#include "discovery_message.hpp"
#include "transport_interface.hpp"

namespace application::discovery {

class Handshake {
   public:
    struct Identifier {
        uint16_t own;
        uint16_t peer;
    };

    Handshake(const Identifier&, transport::PacketInterface&);
    ~Handshake();

    void start();
    void stop();

   private:
    const int64_t HANDSHAKE_TIMEOUT_SEC{3};
    const int64_t HANDSHAKE_REQUEST_DELAY_MS{200};
    // const std::string broadcastAddr{"255.255.255.255"};

    const Identifier& port;
    transport::PacketInterface& transport;

    uint32_t get_own_ip();
    void receive_loop();
    void send_broadcast();
    bool wait_for_ack(const Message::Type);

    bool hasReceivedRequest = false;
    bool hasReceivedAck = false;

    // std::string broadcastAddress;
    // int sock;
    std::atomic<bool> running;
    std::thread recvThread;
};

}  // namespace application::discovery

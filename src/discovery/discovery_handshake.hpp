/**
 * @file discovery_handshake.hpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

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
        struct {
            uint32_t own;
            uint32_t peer;
        } ip;
        struct {
            uint16_t own;
            uint16_t peer;
        } port;
    };

    Handshake(transport::PacketInterface&, Identifier&);
    ~Handshake();

    void start();
    void stop();

   private:
    const size_t HANDSHAKE_NUMBER_OF_REQUESTS{5};
    const int64_t HANDSHAKE_TIMEOUT_SEC{5};
    const int64_t HANDSHAKE_REQUEST_DELAY_MS{200};

    Identifier& identifier;
    transport::PacketInterface& transport;

    uint32_t get_own_ip();
    void receive_loop();
    void send_broadcast();

    bool hasReceivedRequest = false;
    bool hasReceivedAck = false;

    std::atomic<bool> running;
    std::thread recvThread;
};

}  // namespace application::discovery

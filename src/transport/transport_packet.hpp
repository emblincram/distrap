/**
 * @file transport_packet.hpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "transport_interface.hpp"

namespace application::transport {

class Packet : public PacketInterface {
   public:
    explicit Packet(uint16_t _local_port);
    ~Packet();

    bool send(const std::vector<uint8_t>& data, uint32_t ip, uint16_t port) override;
    std::vector<uint8_t> receive(uint32_t& sender_ip, uint16_t& sender_port) override;

   private:
    int sock;
};

}  // namespace application::transport

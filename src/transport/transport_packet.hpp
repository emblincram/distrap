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

    int sock;

   private:
};

}  // namespace application::transport

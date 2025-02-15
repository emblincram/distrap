/**
 * @file transport_interface.hpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <vector>

namespace application::transport {

class PacketInterface {
   public:
    virtual ~PacketInterface() = default;

    virtual bool send(const std::vector<uint8_t>& data, uint32_t ip, uint16_t port) = 0;
    virtual std::vector<uint8_t> receive(uint32_t& sender_ip, uint16_t& sender_port) = 0;
};

}  // namespace application::transport

/**
 * @file toolbox_ip_address.hpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>

namespace application::toolbox {

class IPAddress {
   public:
    IPAddress();
    explicit IPAddress(uint32_t);
    IPAddress(uint8_t, uint8_t, uint8_t, uint8_t);
    explicit IPAddress(const std::string &);

    std::string toString() const;
    uint32_t toUInt32() const { return address; }
    static IPAddress fromString(const std::string &ip_str);
    static IPAddress fromBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

   private:
    uint32_t address;
};

}  // namespace application::toolbox

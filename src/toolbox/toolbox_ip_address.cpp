/**
 * @file toolbox_ip_address.cpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#include "toolbox_ip_address.hpp"

#include <arpa/inet.h>  // Für inet_pton & inet_ntop

#include <sstream>
#include <stdexcept>

namespace application::toolbox {

IPAddress::IPAddress() : address(0) {}

IPAddress::IPAddress(uint32_t addr) : address(htonl(addr)) {}

IPAddress::IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    address = (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | (static_cast<uint32_t>(c) << 8) | (static_cast<uint32_t>(d));
}

IPAddress::IPAddress(const std::string &ip_str) {
    if (inet_pton(AF_INET, ip_str.c_str(), &address) != 1) {
        throw std::invalid_argument("Invalid IP address format");
    }
    // Umwandlung in Host-Byte-Reihenfolge
    address = ntohl(address);
}

/**
 * @brief Wandelt die gespeicherte 32-Bit-Adresse in eine Punkt-Notation um
 *
 * @return * std::string
 */
std::string IPAddress::toString() const {
    char buffer[INET_ADDRSTRLEN];
    // Umwandlung in Netzwerk-Byte-Reihenfolge
    uint32_t addr = htonl(address);
    if (inet_ntop(AF_INET, &addr, buffer, sizeof(buffer)) == nullptr) {
        throw std::runtime_error("Failed to convert IP to string");
    }
    return std::string(buffer);
}

/**
 * @brief Initialisierung aus Punkt-Notation
 *
 * @param ip_str
 * @return IPAddress
 */
IPAddress IPAddress::fromString(const std::string &ip_str) { return IPAddress(ip_str); }

/**
 * @brief Initialisierung aus 4 Bytes
 *
 * @param a Part I
 * @param b Part II
 * @param c Part III
 * @param d Part IV
 * @return IPAddress
 */
IPAddress IPAddress::fromBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { return IPAddress(a, b, c, d); }

}  // namespace application::toolbox

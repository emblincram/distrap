#include "ip_address.hpp"
#include <sstream>
#include <stdexcept>
#include <arpa/inet.h> // Für inet_pton & inet_ntop

// Standardkonstruktor: Setzt Adresse auf 0.0.0.0
IPAddress::IPAddress() : address(0) {}

// Konstruktor mit uint32_t
IPAddress::IPAddress(uint32_t addr) : address(htonl(addr)) {}

// Konstruktor mit 4 Bytes
IPAddress::IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    address = (static_cast<uint32_t>(a) << 24) |
              (static_cast<uint32_t>(b) << 16) |
              (static_cast<uint32_t>(c) << 8) |
              (static_cast<uint32_t>(d));
}

// Konstruktor mit Punkt-Notation
IPAddress::IPAddress(const std::string &ip_str)
{
    if (inet_pton(AF_INET, ip_str.c_str(), &address) != 1)
    {
        throw std::invalid_argument("Invalid IP address format");
    }
    address = ntohl(address); // Umwandlung in Host-Byte-Reihenfolge
}

// Wandelt die gespeicherte 32-Bit-Adresse in eine Punkt-Notation um
std::string IPAddress::toString() const
{
    char buffer[INET_ADDRSTRLEN];
    uint32_t addr = htonl(address); // Umwandlung in Netzwerk-Byte-Reihenfolge
    if (inet_ntop(AF_INET, &addr, buffer, sizeof(buffer)) == nullptr)
    {
        throw std::runtime_error("Failed to convert IP to string");
    }
    return std::string(buffer);
}

// Initialisierung aus Punkt-Notation
IPAddress IPAddress::fromString(const std::string &ip_str)
{
    return IPAddress(ip_str);
}

// Initialisierung aus 4 Bytes
IPAddress IPAddress::fromBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return IPAddress(a, b, c, d);
}

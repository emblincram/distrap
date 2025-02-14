#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#include <cstdint>
#include <string>

class IPAddress
{
private:
    uint32_t address; // Speichert die IP-Adresse als 32-Bit-Wert

public:
    // Konstruktoren
    IPAddress();
    explicit IPAddress(uint32_t addr);
    IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    explicit IPAddress(const std::string &ip_str);

    // Umwandlung zu Punkt-Notation
    std::string toString() const;

    // Getter für die interne 32-Bit-Adresse
    uint32_t toUInt32() const { return address; }

    // Initialisierung aus Punkt-Notation
    static IPAddress fromString(const std::string &ip_str);

    // Initialisierung aus 4 Bytes
    static IPAddress fromBytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
};

#endif // IP_ADDRESS_H

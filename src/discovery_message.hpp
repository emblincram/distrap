#ifndef DISCOVERY_MESSAGE_H
#define DISCOVERY_MESSAGE_H

#include <string>
#include <cstdint>

class DiscoveryMessage
{
public:
    std::string identifier;
    std::string app_ip;
    uint16_t app_port;
    std::string status;

    DiscoveryMessage() = default;
    DiscoveryMessage(std::string id, std::string ip, int port, std::string stat);

    std::string serialize() const;
    static DiscoveryMessage deserialize(const std::string &data);
};

#endif // DISCOVERY_MESSAGE_H

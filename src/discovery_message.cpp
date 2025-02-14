#include "discovery_message.hpp"
#include <sstream>

DiscoveryMessage::DiscoveryMessage(std::string id, std::string ip, int port, std::string stat)
    : identifier(std::move(id)), app_ip(std::move(ip)), app_port(port), status(std::move(stat)) {}

std::string DiscoveryMessage::serialize() const {
    std::ostringstream oss;
    oss << identifier << "|" << app_ip << "|" << app_port << "|" << status;
    return oss.str();
}

DiscoveryMessage DiscoveryMessage::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string id, ip, stat;
    int port;

    if (std::getline(iss, id, '|') &&
        std::getline(iss, ip, '|') &&
        (iss >> port) && iss.ignore() &&
        std::getline(iss, stat)) {
        return DiscoveryMessage(id, ip, port, stat);
    }

    return DiscoveryMessage();  // Falls Deserialisierung fehlschlägt, leeres Objekt zurückgeben
}

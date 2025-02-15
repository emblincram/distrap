#include "discovery_message.hpp"

#include <cstring>
#include <stdexcept>

namespace application {
namespace discovery {
Message::Message(uint16_t id, uint32_t ip, uint16_t port, uint8_t stat) : identifier(id), app_ip(ip), app_port(port), status(stat) {
}

std::vector<uint8_t> Message::serialize() const {
    std::vector<uint8_t> buffer(sizeof(identifier) + sizeof(app_ip) + sizeof(app_port) + sizeof(status));

    size_t offset = 0;
    std::memcpy(buffer.data() + offset, &identifier, sizeof(identifier));
    offset += sizeof(identifier);
    std::memcpy(buffer.data() + offset, &app_ip, sizeof(app_ip));
    offset += sizeof(app_ip);
    std::memcpy(buffer.data() + offset, &app_port, sizeof(app_port));
    offset += sizeof(app_port);
    std::memcpy(buffer.data() + offset, &status, sizeof(status));

    return buffer;
}

Message Message::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() != sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t)) {
        // throw std::runtime_error("Invalid binary data size");
        return Message();
    }

    Message msg;
    size_t offset = 0;
    std::memcpy(&msg.identifier, data.data() + offset, sizeof(msg.identifier));
    offset += sizeof(msg.identifier);
    std::memcpy(&msg.app_ip, data.data() + offset, sizeof(msg.app_ip));
    offset += sizeof(msg.app_ip);
    std::memcpy(&msg.app_port, data.data() + offset, sizeof(msg.app_port));
    offset += sizeof(msg.app_port);
    std::memcpy(&msg.status, data.data() + offset, sizeof(msg.status));

    return msg;
}
}  // namespace discovery
}  // namespace application

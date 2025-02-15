#pragma once

#include <cstdint>
#include <vector>

namespace application {
namespace discovery {
class Message {
   public:
    uint16_t identifier;  // Nachrichtentyp
    uint32_t app_ip;      // IPv4 als uint32_t
    uint16_t app_port;    // Portnummer
    uint8_t status;       // Status-Code

    Message() = default;
    Message(uint16_t id, uint32_t ip, uint16_t port, uint8_t stat);

    std::vector<uint8_t> serialize() const;
    static Message deserialize(const std::vector<uint8_t> &data);
};
}  // namespace discovery
}  // namespace application

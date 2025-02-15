/**
 * @file discovery_message.cpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#include "discovery_message.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace application::discovery {

Message::Message(const Payload& _payload) : payload(_payload) {}

std::vector<uint8_t> Message::serialize() const {
    std::vector<uint8_t> buffer(sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));
    size_t offset = 0;

    const uint16_t identifier = static_cast<uint16_t>(payload.identifier);
    std::memcpy(buffer.data() + offset, &identifier, sizeof(identifier));
    offset += sizeof(identifier);

    std::memcpy(buffer.data() + offset, &payload.app_ip, sizeof(payload.app_ip));
    offset += sizeof(payload.app_ip);

    std::memcpy(buffer.data() + offset, &payload.app_port, sizeof(payload.app_port));
    offset += sizeof(payload.app_port);

    const uint8_t status = static_cast<uint8_t>(payload.status);
    std::memcpy(buffer.data() + offset, &status, sizeof(status));

    return buffer;
}

Message Message::deserialize(const std::vector<uint8_t>& data) {
    constexpr size_t expected_size = sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t);

    if (data.size() != expected_size) {
        std::cerr << "Fehlerhafte Nachricht! Erwartete " << expected_size;
        std::cerr << " Bytes, aber erhielt " << data.size() << " Bytes.\n";
        return Message();
    }

    Message::Payload payload;
    size_t offset = 0;

    uint16_t identifier;
    std::memcpy(&identifier, data.data() + offset, sizeof(identifier));
    offset += sizeof(identifier);
    payload.identifier = static_cast<Type>(identifier);

    std::memcpy(&payload.app_ip, data.data() + offset, sizeof(payload.app_ip));
    offset += sizeof(payload.app_ip);

    std::memcpy(&payload.app_port, data.data() + offset, sizeof(payload.app_port));
    offset += sizeof(payload.app_port);

    uint8_t status;
    std::memcpy(&status, data.data() + offset, sizeof(status));
    payload.status = static_cast<Status>(status);

    return Message(payload);
}

}  // namespace application::discovery

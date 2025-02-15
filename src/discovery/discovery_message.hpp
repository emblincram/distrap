/**
 * @file discovery_message.hpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace application::discovery {

class Message {
   public:
    enum class Type : uint16_t {
        REQUEST = 1,
        ACK = 2,
        UNDEFINED = 0xff
    };

    enum class Status : uint8_t {
        UNKNOWN = 0,
        SUCCESS = 1,
        ERROR = 2
    };

    struct Payload {
        Type identifier;    // Nachrichtentyp
        uint32_t app_ip;    // IPv4 als uint32_t
        uint16_t app_port;  // Portnummer
        Status status;      // Status-Code
    } payload;

    Message() = default;
    Message(const Payload &);

    std::vector<uint8_t> serialize() const;
    static Message deserialize(const std::vector<uint8_t> &data);

    inline std::string to_string(Message::Type type) {
        static const std::unordered_map<Message::Type, std::string> typeNames = {{Message::Type::REQUEST, "REQUEST"}, {Message::Type::ACK, "ACK"}};
        return typeNames.at(type);
    }

    inline std::string to_string(Message::Status status) {
        static const std::unordered_map<Message::Status, std::string> statusNames = {
            {Message::Status::UNKNOWN, "UNKNOWN"}, {Message::Status::SUCCESS, "SUCCESS"}, {Message::Status::ERROR, "ERROR"}};
        return statusNames.at(status);
    }
};

}  // namespace application::discovery

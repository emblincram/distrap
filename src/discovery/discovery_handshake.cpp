/**
 * @file discovery_handshake.cpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#include "discovery_handshake.hpp"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include "discovery_message.hpp"
#include "toolbox_ip_address.hpp"

namespace application::discovery {

Handshake::Handshake(transport::PacketInterface& _transport, Identifier& _identifier) : transport(_transport), identifier(_identifier), running(false) {}

Handshake::~Handshake() {}

void Handshake::start() {
    identifier.ip.own = get_own_ip();
    running = true;
    recvThread = std::thread(&Handshake::receive_loop, this);
    send_broadcast();
}

void Handshake::stop() {
    running = false;

    Message quit_msg(Message::Payload{Message::Type::UNDEFINED, identifier.ip.own, identifier.port.own, Message::Status::UNKNOWN});
    transport.send(quit_msg.serialize(), identifier.ip.own, identifier.port.own);

    if (recvThread.joinable()) {
        recvThread.join();
    }
}

void Handshake::receive_loop() {
    auto start_time = std::chrono::steady_clock::now();

    while (running) {
        uint32_t sender_ip;
        uint16_t sender_port;

        auto data = transport.receive(sender_ip, sender_port);
        if (!data.empty()) {
            try {
                Message receivedMsg = Message::deserialize(data);
                const Message::Payload& payload = receivedMsg.payload;

                std::cout << "handshake: recv " << inet_ntoa(*(struct in_addr*)&sender_ip) << ":" << sender_port;
                std::cout << " [ ID: " << static_cast<int>(payload.identifier);
                std::cout << " | IP: " << inet_ntoa(*(struct in_addr*)&payload.app_ip);
                std::cout << " | PORT: " << payload.app_port;
                std::cout << " | STATUS " << static_cast<int>(payload.status) << " ]" << std::endl;

                if (payload.identifier == Message::Type::REQUEST) {
                    if (payload.app_port == identifier.port.peer && sender_port == identifier.port.peer) {
                        hasReceivedRequest = true;

                        // store sender ip address
                        identifier.ip.peer = sender_ip;

                        Message ackMsg(Message::Payload{Message::Type::ACK, payload.app_ip, payload.app_port, Message::Status::SUCCESS});
                        std::vector<uint8_t> ackData = ackMsg.serialize();

                        std::cout << "handshake: send ACK " << inet_ntoa(*(struct in_addr*)&sender_ip) << ":" << sender_port << std::endl;
                        transport.send(ackData, sender_ip, sender_port);
                    }

                } else if (payload.identifier == Message::Type::ACK) {
                    if (payload.app_ip == identifier.ip.own && payload.app_port == identifier.port.own &&
                        sender_port == identifier.port.peer) {  // Prüfe, ob ACK für uns ist
                        hasReceivedAck = true;
                        std::cout << "handshake: recv ACK." << std::endl;
                    }
                }

                if (hasReceivedRequest && hasReceivedAck) {
                    std::cout << "handshake: success." << std::endl;
                    running = false;
                    break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Fehler bei der Deserialisierung: " << e.what() << std::endl;
            }
        }

        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count();
        if (elapsed_time > HANDSHAKE_TIMEOUT_SEC) {
            std::cout << "handshake: timeout after" << HANDSHAKE_TIMEOUT_SEC << " sec." << std::endl;
            running = false;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(HANDSHAKE_REQUEST_DELAY_MS));
    }
}

void Handshake::send_broadcast() {
    const uint32_t own_ip = get_own_ip();
    try {
        Message message(Message::Payload{Message::Type::REQUEST, own_ip, identifier.port.own, Message::Status::SUCCESS});
        std::vector<uint8_t> messageData = message.serialize();

        for (int i = 0; i < HANDSHAKE_NUMBER_OF_REQUESTS && running; ++i) {
            std::cout << "handshake: send broadcast " << i + 1 << "/" << HANDSHAKE_NUMBER_OF_REQUESTS;
            std::cout << " 255.255.255.255:" << identifier.port.peer;
            std::cout << " [ ID: " << static_cast<int>(message.payload.identifier);
            std::cout << " | IP: " << inet_ntoa(*(struct in_addr*)&message.payload.app_ip);
            std::cout << " | PORT: " << message.payload.app_port;
            std::cout << " | STATUS " << static_cast<int>(message.payload.status) << " ]" << std::endl;

            transport.send(messageData, INADDR_BROADCAST, identifier.port.peer);

            if (hasReceivedAck) {
                std::cout << "handshake: send broadcast done." << std::endl;
                running = false;
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(HANDSHAKE_TIMEOUT_SEC * 1000 / HANDSHAKE_NUMBER_OF_REQUESTS));
        }
    } catch (const std::exception& e) {
        std::cerr << "Fehler bei der Serialisierung: " << e.what() << std::endl;
    }
}

uint32_t Handshake::get_own_ip() {
    struct ifaddrs *ifaddr, *ifa;
    int family;
    uint32_t ip_binary = 0;  // Default-Wert

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return 0;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET) {
            // Nur IPv4
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;

            // Lokale Adressen (127.0.0.1) ignorieren
            if ((ntohl(addr->sin_addr.s_addr) & 0xFF000000) == 0x7F000000) {
                continue;
            }

            ip_binary = addr->sin_addr.s_addr;  // Bereits in Netzwerkreihenfolge
            break;                              // Erste gültige IP nehmen
        }
    }

    freeifaddrs(ifaddr);
    return ip_binary;
}

}  // namespace application::discovery

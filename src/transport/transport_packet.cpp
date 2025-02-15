/**
 * @file transport_packet.cpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#include "transport_packet.hpp"

#include <cstring>

namespace application::transport {

Packet::Packet(uint16_t _local_port) {
    // UDP-Socket erstellen
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Fehler beim Erstellen des Sockets: " << strerror(errno) << std::endl;
        return;
    }

    int reuseEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseEnable, sizeof(reuseEnable)) < 0) {
        std::cerr << "Fehler beim Aktivieren von SO_REUSEADDR: " << strerror(errno) << std::endl;
        close(sock);
        return;
    }

    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        std::cerr << "Fehler beim Aktivieren von SO_BROADCAST: " << strerror(errno) << std::endl;
        close(sock);
        return;
    }

    // Empfangs-Socket vorbereiten
    struct sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(_local_port);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        std::cerr << "Fehler beim Binden des Sockets: " << strerror(errno) << " (Port: " << _local_port << ")" << std::endl;
        close(sock);
        sock = -1;
    } else {
        std::cout << "Socket erfolgreich gebunden auf Port " << _local_port << std::endl;
    }
}

Packet::~Packet() {
    if (sock >= 0) {
        close(sock);
    }
}

bool Packet::send(const std::vector<uint8_t>& data, uint32_t _ip, uint16_t _port) {
    if (sock < 0)
        return false;

    struct sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(_port);
    destAddr.sin_addr.s_addr = _ip;

    ssize_t sentBytes = sendto(sock, data.data(), data.size(), 0, (struct sockaddr*)&destAddr, sizeof(destAddr));

    if (sentBytes > 0) {
        if (_ip == INADDR_BROADCAST) {
            std::cout << "transport: send broarcast 255.255.255.255:" << _port << std::endl;
        } else {
            std::cout << "transport: send unicast " << inet_ntoa(*(struct in_addr*)&_ip) << ":" << _port << std::endl;
        }
    }

    return sentBytes > 0;
}

std::vector<uint8_t> Packet::receive(uint32_t& _sender_ip, uint16_t& _sender_port) {
    if (sock < 0)
        return {};

    struct sockaddr_in senderAddr{};
    socklen_t addrLen = sizeof(senderAddr);
    std::vector<uint8_t> buffer(1024);

    ssize_t recvLen = recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&senderAddr, &addrLen);
    if (recvLen > 0) {
        buffer.resize(recvLen);
        _sender_ip = senderAddr.sin_addr.s_addr;
        _sender_port = ntohs(senderAddr.sin_port);

        if (_sender_ip == INADDR_BROADCAST) {
            std::cout << "transport: recv broadcast 255.255.255.255:" << _sender_port << std::endl;
        } else {
            std::cout << "transport: recv broadcast " << inet_ntoa(senderAddr.sin_addr) << ":" << _sender_port << std::endl;
        }

        return buffer;
    }
    return {};
}

}  // namespace application::transport

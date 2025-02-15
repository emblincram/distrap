#include "transport_packet.hpp"

namespace application::transport {

Packet::Packet(uint16_t local_port) {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Fehler beim Erstellen des UDP-Sockets\n";
        return;
    }

    struct sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(local_port);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        std::cerr << "Fehler beim Binden des UDP-Sockets\n";
        close(sock);
        sock = -1;
    }
}

Packet::~Packet() {
    if (sock >= 0) {
        close(sock);
    }
}

bool Packet::send(const std::vector<uint8_t>& data, uint32_t ip, uint16_t port) {
    if (sock < 0)
        return false;

    struct sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    destAddr.sin_addr.s_addr = ip;

    ssize_t sentBytes = sendto(sock, data.data(), data.size(), 0, (struct sockaddr*)&destAddr, sizeof(destAddr));
    return sentBytes > 0;
}

std::vector<uint8_t> Packet::receive(uint32_t& sender_ip, uint16_t& sender_port) {
    if (sock < 0)
        return {};

    struct sockaddr_in senderAddr{};
    socklen_t addrLen = sizeof(senderAddr);
    std::vector<uint8_t> buffer(1024);

    ssize_t recvLen = recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&senderAddr, &addrLen);
    if (recvLen > 0) {
        buffer.resize(recvLen);
        sender_ip = senderAddr.sin_addr.s_addr;
        sender_port = ntohs(senderAddr.sin_port);
        return buffer;
    }
    return {};
}

}  // namespace application::transport

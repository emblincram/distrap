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

namespace application {
namespace discovery {
Handshake::Handshake(const Identifier& _port) : broadcastAddress(broadcastAddr), port(_port), running(false), sock(-1) {
}

Handshake::~Handshake() {
    stop();
}

void Handshake::start() {
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

    // Empfangs-Socket vorbereiten
    struct sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port.own);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        std::cerr << "Fehler beim Binden des Sockets: " << strerror(errno) << " (Port: " << port.own << ")" << std::endl;
        close(sock);
        return;
    }
    std::cout << "Socket erfolgreich gebunden auf Port " << port.own << std::endl;

    running = true;
    recvThread = std::thread(&Handshake::receive_loop, this);
    send_broadcast();
}

void Handshake::stop() {
    running = false;
    if (recvThread.joinable()) {
        recvThread.join();
    }
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}

void Handshake::receive_loop() {
    struct sockaddr_in recvAddr{};
    socklen_t addrLen = sizeof(recvAddr);
    std::vector<uint8_t> buffer(1024);

    std::cout << "Warte auf Nachrichten..." << std::endl;

    while (running) {
        ssize_t recvLen = recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&recvAddr, &addrLen);
        if (recvLen > 0) {
            buffer.resize(recvLen);

            try {
                Message receivedMsg = Message::deserialize(buffer);
                std::cout << "Empfangen von " << inet_ntoa(recvAddr.sin_addr) << " | ID: " << receivedMsg.identifier
                          << " | IP: " << inet_ntoa(*(struct in_addr*)&receivedMsg.app_ip) << " | Port: " << receivedMsg.app_port
                          << " | Status: " << static_cast<int>(receivedMsg.status) << std::endl;

                if (receivedMsg.identifier == 1) {
                    hasReceivedRequest = true;

                    Message ackMsg(2, receivedMsg.app_ip, receivedMsg.app_port, 1);
                    std::vector<uint8_t> ackData = ackMsg.serialize();

                    sendto(sock, ackData.data(), ackData.size(), 0, (struct sockaddr*)&recvAddr, sizeof(recvAddr));
                    std::cout << "ACK gesendet für ID: " << receivedMsg.identifier << std::endl;
                } else if (receivedMsg.identifier == 2) {
                    hasReceivedAck = true;
                    std::cout << "ACK empfangen!" << std::endl;
                }

                if (hasReceivedRequest && hasReceivedAck) {
                    std::cout << "Handshake erfolgreich!" << std::endl;
                    running = false;
                }
            } catch (const std::exception& e) {
                std::cerr << "Fehler bei der Deserialisierung: " << e.what() << std::endl;
            }
        }
    }
}

void Handshake::send_broadcast() {
    struct sockaddr_in broadcastAddr{};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(port.peer);
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastAddress.c_str());

    // SO_BROADCAST setzen (für sendto())
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        std::cerr << "Fehler beim Aktivieren von SO_BROADCAST: " << strerror(errno) << std::endl;
        return;
    }

    const uint32_t own_ip = get_own_ip();
    Message message(1, own_ip, port.own, 0);  // DISCOVERY_REQUEST
    std::vector<uint8_t> messageData = message.serialize();

    for (int i = 0; i < 10 && running; ++i) {
        // Nachricht senden
        ssize_t sentBytes =
            sendto(sock, messageData.data(), messageData.size(), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

        if (sentBytes < 0) {
            std::cerr << "Fehler beim Senden des Broadcasts: " << strerror(errno) << std::endl;
        } else {
            std::cout << "Broadcast gesendet (" << i + 1 << "/10)" << std::endl;
        }

        // **Falls ACK erhalten, aber noch keine Anfrage, weiter senden**
        if (hasReceivedAck && !hasReceivedRequest) {
            std::cout << "ACK erhalten, warte auf DISCOVERY_REQUEST..." << std::endl;
        }

        // **Falls beide Bedingungen erfüllt sind, abbrechen**
        if (hasReceivedAck && hasReceivedRequest) {
            std::cout << "Beide Seiten bestätigt. Beenden!" << std::endl;
            return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

bool Handshake::wait_for_ack(const uint16_t message_id) {
    struct sockaddr_in recvAddr{};
    socklen_t addrLen = sizeof(recvAddr);
    std::vector<uint8_t> buffer(1024);

    // Warte maximal 3 Sekunden auf ein ACK
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(3)) {
        ssize_t recvLen = recvfrom(sock, buffer.data(), buffer.size(), MSG_DONTWAIT, (struct sockaddr*)&recvAddr, &addrLen);
        if (recvLen > 0) {
            buffer.resize(recvLen);
            try {
                Message response = Message::deserialize(buffer);
                if (response.identifier == 2 && response.identifier == message_id) {
                    std::cout << "ACK erhalten von " << inet_ntoa(recvAddr.sin_addr) << std::endl;
                    return true;
                }
            } catch (const std::exception& e) {
                std::cerr << "Fehler bei der Deserialisierung: " << e.what() << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Warten
    }

    std::cout << "Kein ACK erhalten, erneut senden..." << std::endl;
    return false;  // Kein ACK erhalten
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
            ip_binary = addr->sin_addr.s_addr;  // Bereits in Netzwerkreihenfolge
            break;                              // Erste gültige IP nehmen
        }
    }

    freeifaddrs(ifaddr);
    return ip_binary;
}

}  // namespace discovery
}  // namespace application


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

Handshake::Handshake(const Identifier& _port, transport::PacketInterface& _transport)
    : port(_port), transport(_transport), /*broadcastAddress(broadcastAddr),*/ running(false) /*, sock(-1)*/ {}

Handshake::~Handshake() { stop(); }

void Handshake::start() {
    /*
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
    */

    running = true;
    recvThread = std::thread(&Handshake::receive_loop, this);
    send_broadcast();
}

void Handshake::stop() {
    running = false;
    if (recvThread.joinable()) {
        recvThread.join();
    }
    /*
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
    */
}

void Handshake::receive_loop() {
    /*
    struct sockaddr_in recvAddr{};
    socklen_t addrLen = sizeof(recvAddr);
    std::vector<uint8_t> buffer(1024);

    std::cout << "Warte auf Nachrichten..." << std::endl;
    */

    while (running) {
        uint32_t sender_ip;
        uint16_t sender_port;

        /*
        ssize_t recvLen = recvfrom(sock, buffer.data(), buffer.size(), 0, (struct sockaddr*)&recvAddr, &addrLen);
        */
        auto data = transport.receive(sender_ip, sender_port);
        // if (recvLen > 0) {
        if (!data.empty() > 0) {
            // buffer.resize(recvLen);
            try {
                // Message receivedMsg = Message::deserialize(buffer);
                Message receivedMsg = Message::deserialize(data);
                const Message::Payload& payload = receivedMsg.payload;

                std::cout << "Empfangen von " << /*inet_ntoa(recvAddr.sin_addr)*/ inet_ntoa(*(struct in_addr*)&sender_ip);
                std::cout << " | ID: " << static_cast<int>(payload.identifier);
                std::cout << " | IP: " << inet_ntoa(*(struct in_addr*)&payload.app_ip);
                std::cout << " | Port: " << payload.app_port;
                std::cout << " | Status: " << static_cast<int>(payload.status) << std::endl;

                if (payload.identifier == Message::Type::REQUEST) {
                    hasReceivedRequest = true;

                    Message ackMsg(Message::Payload{Message::Type::ACK, payload.app_ip, payload.app_port, Message::Status::SUCCESS});
                    std::vector<uint8_t> ackData = ackMsg.serialize();

                    /*
                    sendto(sock, ackData.data(), ackData.size(), 0, (struct sockaddr*)&recvAddr, sizeof(recvAddr));
                    */
                    transport.send(ackData, sender_ip, sender_port);
                    std::cout << "ACK gesendet für ID: " << static_cast<int>(payload.identifier) << std::endl;

                } else if (payload.identifier == Message::Type::ACK) {
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
    /*
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
    */

    const uint32_t own_ip = get_own_ip();
    Message message(Message::Payload{Message::Type::REQUEST, own_ip, port.own, Message::Status::SUCCESS});
    std::vector<uint8_t> messageData = message.serialize();

    for (int i = 0; i < 10 && running; ++i) {
        /*
        ssize_t sentBytes = sendto(sock, messageData.data(), messageData.size(), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        */
        transport.send(messageData, INADDR_BROADCAST, port.peer);
        if (wait_for_ack(Message::Type::REQUEST))
            return;

        /*
        if (sentBytes < 0) {
            std::cerr << "Fehler beim Senden des Broadcasts: " << strerror(errno) << std::endl;
        } else {
            std::cout << "Broadcast gesendet (" << i + 1 << "/10)";
            std::cout << " an " << broadcastAddress;
            std::cout << " auf Port " << port.peer;
            std::cout << std::endl;
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
        */

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

bool Handshake::wait_for_ack(const Message::Type _message_type) {
    /*
    struct sockaddr_in recvAddr{};
    socklen_t addrLen = sizeof(recvAddr);
    std::vector<uint8_t> buffer(1024);
    */

    uint32_t sender_ip;
    uint16_t sender_port;

    // Warte maximal 3 Sekunden auf ein ACK
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(HANDSHAKE_TIMEOUT_SEC)) {
        /*
        ssize_t recvLen = recvfrom(sock, buffer.data(), buffer.size(), MSG_DONTWAIT, (struct sockaddr*)&recvAddr, &addrLen);
        */
        auto data = transport.receive(sender_ip, sender_port);
        // if (recvLen > 0) {
        // buffer.resize(recvLen);
        if (!data.empty() > 0) {
            try {
                // Message response = Message::deserialize(buffer);
                Message response = Message::deserialize(data);
                const Message::Payload& payload = response.payload;

                if (payload.identifier == Message::Type::ACK && _message_type == Message::Type::REQUEST) {
                    std::cout << "ACK erhalten von " << /*inet_ntoa(recvAddr.sin_addr) << */ std::endl;
                    return true;
                }
            } catch (const std::exception& e) {
                std::cerr << "Fehler bei der Deserialisierung: " << e.what() << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(HANDSHAKE_REQUEST_DELAY_MS));
    }

    // Kein ACK erhalten
    std::cout << "Kein ACK erhalten, erneut senden..." << std::endl;
    return false;
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

#include "discovery.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

Discovery::Discovery(const std::string &broadcastAddr, int port_own, int port_ext)
    : broadcastAddress(broadcastAddr), port_own(port_own), port_ext(port_ext),
      running(false), sock(-1) {}

Discovery::~Discovery()
{
    stop();
}

void Discovery::start()
{
    running = true;

    // UDP-Socket erstellen
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "Fehler beim Erstellen des Sockets: " << strerror(errno) << std::endl;
        return;
    }

    // Setze SO_REUSEADDR (Mehrfachbindung erlauben)
    int reuseEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseEnable, sizeof(reuseEnable)) < 0)
    {
        std::cerr << "Fehler beim Aktivieren von SO_REUSEADDR: " << strerror(errno) << std::endl;
        close(sock);
        return;
    }

    // Empfangs-Socket vorbereiten
    struct sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port_own);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bindet an alle Netzwerkschnittstellen

    if (bind(sock, (struct sockaddr *)&localAddr, sizeof(localAddr)) < 0)
    {
        std::cerr << "Fehler beim Binden des Sockets: " << strerror(errno)
                  << " (Port: " << port_own << ")" << std::endl;
        close(sock);
        return;
    }

    std::cout << "Socket erfolgreich gebunden auf Port " << port_own << std::endl;

    // Empfangs-Thread starten
    recvThread = std::thread(&Discovery::receiveLoop, this);

    // Broadcast senden
    sendBroadcast();
}

void Discovery::stop()
{
    running = false;
    if (recvThread.joinable())
    {
        recvThread.join();
    }
    if (sock >= 0)
    {
        close(sock);
        sock = -1;
    }
}

void Discovery::receiveLoop()
{
    struct sockaddr_in recvAddr{};
    socklen_t addrLen = sizeof(recvAddr);
    char buffer[1024];

    std::cout << "Warte auf Nachrichten..." << std::endl;

    while (running)
    {
        ssize_t recvLen = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr *)&recvAddr, &addrLen);
        if (recvLen > 0)
        {
            buffer[recvLen] = '\0';
            std::string receivedMsg(buffer);
            std::cout << "Nachricht von " << inet_ntoa(recvAddr.sin_addr) << ": " << receivedMsg << std::endl;

            if (receivedMsg.find("DISCOVERY_REQUEST") != std::string::npos)
            {
                hasReceivedRequest = true;

                // ACK senden
                std::string ackMessage = "ACK|" + receivedMsg.substr(receivedMsg.find("ID:"));
                sendto(sock, ackMessage.c_str(), ackMessage.size(), 0,
                       (struct sockaddr *)&recvAddr, sizeof(recvAddr));
                std::cout << "ACK gesendet für " << receivedMsg << std::endl;
            }
            else if (receivedMsg.find("ACK") != std::string::npos)
            {
                hasReceivedAck = true;
                std::cout << "ACK empfangen!" << std::endl;
            }

            // **NEUE BEENDIGUNGSLOGIK**
            if (hasReceivedRequest && hasReceivedAck)
            {
                std::cout << "Beide Programme haben sich erkannt! Beende..." << std::endl;
                running = false;
            }
        }
    }
}

void Discovery::sendBroadcast()
{
    struct sockaddr_in broadcastAddr{};
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(port_ext);
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastAddress.c_str());

    // SO_BROADCAST setzen (für sendto())
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
    {
        std::cerr << "Fehler beim Aktivieren von SO_BROADCAST: " << strerror(errno) << std::endl;
        return;
    }

    std::string message = "DISCOVERY_REQUEST|ID:1234"; // ID für Tracking

    for (int i = 0; i < 10 && running; ++i)
    {
        // Nachricht senden
        ssize_t sentBytes = sendto(sock, message.c_str(), message.size(), 0,
                                   (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));

        if (sentBytes < 0)
        {
            std::cerr << "Fehler beim Senden des Broadcasts: " << strerror(errno) << std::endl;
        }
        else
        {
            std::cout << "Broadcast gesendet (" << i + 1 << "/10)" << std::endl;
        }

        // **Falls ACK erhalten, aber noch keine Anfrage, weiter senden**
        if (hasReceivedAck && !hasReceivedRequest)
        {
            std::cout << "ACK erhalten, warte auf DISCOVERY_REQUEST..." << std::endl;
        }

        // **Falls beide Bedingungen erfüllt sind, abbrechen**
        if (hasReceivedAck && hasReceivedRequest)
        {
            std::cout << "Beide Seiten bestätigt. Beenden!" << std::endl;
            return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

bool Discovery::waitForAck(const std::string &message_id)
{
    struct sockaddr_in recvAddr{};
    socklen_t addrLen = sizeof(recvAddr);
    char buffer[1024];

    // Warte maximal 3 Sekunden auf ein ACK
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(3))
    {
        ssize_t recvLen = recvfrom(sock, buffer, sizeof(buffer) - 1, MSG_DONTWAIT,
                                   (struct sockaddr *)&recvAddr, &addrLen);
        if (recvLen > 0)
        {
            buffer[recvLen] = '\0';
            std::string response(buffer);

            // Prüfen, ob ACK enthalten ist
            if (response.find("ACK|" + message_id) != std::string::npos)
            {
                std::cout << "Bestätigung erhalten von " << inet_ntoa(recvAddr.sin_addr) << std::endl;
                return true;  // ACK erhalten
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Warten
    }

    std::cout << "Kein ACK erhalten, erneut senden..." << std::endl;
    return false;  // Kein ACK erhalten
}

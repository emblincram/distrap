#ifndef DISCOVERY_HPP
#define DISCOVERY_HPP

#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <atomic>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

class Discovery
{
public:
    Discovery(const std::string &broadcastAddr, int port_own, int port_ext);
    ~Discovery();

    void start();
    void stop();

private:
    void receiveLoop();
    void sendBroadcast();
    bool waitForAck(const std::string &);

    bool hasReceivedRequest = false; // Haben wir eine DISCOVERY_REQUEST empfangen?
    bool hasReceivedAck = false;     // Haben wir ein ACK empfangen?

    std::string broadcastAddress;
    int port_own;
    int port_ext;
    int sock;
    std::atomic<bool> running;
    std::thread recvThread;
};

#endif // DISCOVERY_HPP

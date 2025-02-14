// udp_discovery.h
#ifndef UDP_DISCOVERY_H
#define UDP_DISCOVERY_H

#include "udp_stub.hpp"
#include <string>
#include <atomic>

class UDPDiscovery
{
public:
    UDPDiscovery(UDPStub &_udp_stub, int _local_port);
    void start();
    void stop();
    std::string getPartnerIP() const;

private:
    UDPStub &udp_stub;

    int local_port;
    int partner_port;

    std::string local_ip;
    std::string partner_ip;
    std::atomic<bool> running;

    void determineLocalIP();
    void broadcastLocalIP();
    void handleIncomingMessage(const std::string &message);
};

#endif // UDP_DISCOVERY_H

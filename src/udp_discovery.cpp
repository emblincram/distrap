// udp_discovery.cpp
#include "udp_discovery.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>

UDPDiscovery::UDPDiscovery(UDPStub &_udp_stub, int _local_port) : local_port(_local_port),
                                                                  partner_port(local_port == 51234 ? 51235 : 51234),
                                                                  /*udp_stub("0.0.0.0", local_port, "255.255.255.255", partner_port),*/
                                                                  udp_stub(_udp_stub),
                                                                  running(false)
{
    determineLocalIP();
}

void UDPDiscovery::determineLocalIP()
{
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == nullptr)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            char host[NI_MAXHOST];
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST) == 0)
            {
                if (strcmp(host, "127.0.0.1") != 0)
                {
                    local_ip = host;
                    break;
                }
            }
        }
    }
    freeifaddrs(ifaddr);
}

void UDPDiscovery::broadcastLocalIP()
{
    if (!local_ip.empty())
    {
        udp_stub.sendMessage(local_ip);
    }
}

void UDPDiscovery::start()
{
    running = true;
    udp_stub.startReceiving([this](const std::string &message)
                            { handleIncomingMessage(message); });
    broadcastLocalIP();
}

void UDPDiscovery::stop()
{
    running = false;
    udp_stub.stop();
}

void UDPDiscovery::handleIncomingMessage(const std::string &message)
{
    if (partner_ip.empty())
    {
        partner_ip = message;
        std::cout << "Discovered partner IP: " << partner_ip << std::endl;
    }
}

std::string UDPDiscovery::getPartnerIP() const
{
    return partner_ip;
}

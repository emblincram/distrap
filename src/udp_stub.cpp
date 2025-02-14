#include "udp_stub.hpp"
#include "ifaddrs.h"
#include <netdb.h>

UDPStub::UDPStub(int local_port, int remote_port)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    determineLocalIP();

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(local_ip.c_str());
    local_addr.sin_port = htons(local_port);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(remote_ip.c_str());
    remote_addr.sin_port = htons(remote_port);

    running = false;
}

void UDPStub::determineLocalIP()
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

void UDPStub::broadcastLocalIP()
{
    if (!local_ip.empty())
    {
        sendMessage(local_ip);
    }
}

UDPStub::~UDPStub()
{
    stop();
    close(sockfd);
}

void UDPStub::startReceiving(Callback callback)
{
    running = true;
    recv_thread = std::thread([this, callback]()
                              {
        char buffer[1024];
        while (running) {
            socklen_t len = sizeof(local_addr);
            int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&local_addr, &len);
            if (n > 0) {
                buffer[n] = '\0';
                callback(std::string(buffer));
            }
        } });
}

void UDPStub::sendMessage(const std::string &message)
{
    sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
}

void UDPStub::stop()
{
    running = false;
    if (recv_thread.joinable())
        recv_thread.join();
}

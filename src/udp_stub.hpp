#pragma once

#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

class UDPStub
{
public:
    using Callback = std::function<void(const std::string &)>;

    UDPStub(int local_port, int remote_port);
    ~UDPStub();

    void startReceiving(Callback callback);
    void sendMessage(const std::string &message);
    void stop();

private:
    int sockfd;
    sockaddr_in local_addr;
    sockaddr_in remote_addr;
    std::thread recv_thread;
    std::atomic<bool> running;

    std::string local_ip;
    std::string remote_ip;

    void determineLocalIP();
    void broadcastLocalIP();
};

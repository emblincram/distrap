// application_a.cpp
#include "application_a.hpp"
#include <iostream>

ApplicationA::ApplicationA(const std::string &local_ip, int local_port, const std::string &remote_ip, int remote_port)
    : udp_stub(local_port, remote_port) {}

void ApplicationA::start()
{
    udp_stub.startReceiving([this](const std::string &message)
                            { handleMessage(message); });
}

void ApplicationA::sendMessage(const std::string &message)
{
    udp_stub.sendMessage(message);
}

void ApplicationA::stop()
{
    udp_stub.stop();
}

void ApplicationA::handleMessage(const std::string &message)
{
    std::cout << "Received in A: " << message << std::endl;
}

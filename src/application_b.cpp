// application_b.cpp
#include "application_b.hpp"
#include <iostream>
#include <sstream>

ApplicationB::ApplicationB(const std::string &local_ip, int local_port, const std::string &remote_ip, int remote_port)
    : udp_stub(local_port, remote_port) {}

void ApplicationB::start()
{
    udp_stub.startReceiving([this](const std::string &data)
                            { handleIncomingData(data); });
}

void ApplicationB::stop()
{
    udp_stub.stop();
}

void ApplicationB::handleIncomingData(const std::string &data)
{
    std::vector<float> streamData;
    std::istringstream iss(data);
    float value;
    while (iss >> value)
    {
        streamData.push_back(value);
    }
    processStreamData(streamData);
}

void ApplicationB::processStreamData(const std::vector<float> &streamData)
{
    for (float value : streamData)
    {
        std::cout << "Received stream value: " << value << std::endl;
    }
}

// application_b.h
#ifndef APPLICATION_B_H
#define APPLICATION_B_H

#include "udp_stub.hpp"
#include <vector>

class ApplicationB {
public:
    ApplicationB(const std::string& local_ip, int local_port, const std::string& remote_ip, int remote_port);
    void start();
    void stop();

private:
    UDPStub udp_stub;
    void handleIncomingData(const std::string& data);
    void processStreamData(const std::vector<float>& streamData);
};

#endif // APPLICATION_B_H

// application_a.h
#ifndef APPLICATION_A_H
#define APPLICATION_A_H

#include "udp_stub.hpp"

class ApplicationA {
public:
    ApplicationA(const std::string& local_ip, int local_port, const std::string& remote_ip, int remote_port);
    void start();
    void sendMessage(const std::string& message);
    void stop();

private:
    UDPStub udp_stub;
    void handleMessage(const std::string& message);
};

#endif // APPLICATION_A_H

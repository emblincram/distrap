#include "discovery_handshake.hpp"
#include "transport_packet.hpp"

int main(int argc, char **argv) {
    const application::discovery::Handshake::Identifier port{50010, 50011};
    application::transport::Packet packet(port.own);
    application::discovery::Handshake handshake(port, packet);
    handshake.start();
    std::cout << "start" << std::endl;
    handshake.stop();
    std::cout << "stop" << std::endl;

    return 0;
}

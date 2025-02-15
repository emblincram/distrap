
#include "discovery_handshake.hpp"

int main(int argc, char **argv)
{
    const application::discovery::Handshake::Identifier port{50001, 50000};
    application::discovery::Handshake handshake(port);

    handshake.start();
    std::cout << "start" << std::endl;
    handshake.stop();
    std::cout << "stop" << std::endl;

    return 0;
}

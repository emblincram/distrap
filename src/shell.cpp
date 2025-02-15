/**
 * @file shell.cpp
 * @author Roman Koch (koch.roman@gmail.com)
 * 
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#include "discovery_handshake.hpp"
#include "transport_packet.hpp"

int main(int argc, char **argv) {
    application::discovery::Handshake::Identifier identifier{{0, 0}, {50011, 50010}};
    application::transport::Packet packet(identifier.port.own);
    application::discovery::Handshake handshake(packet, identifier);

    handshake.start();
    std::cout << "start" << std::endl;
    handshake.stop();
    std::cout << "stop" << std::endl;

    std::cout << "own  address: " << identifier.ip.own << ":" << identifier.port.own << std::endl;
    std::cout << "peer address: " << identifier.ip.peer << ":" << identifier.port.peer << std::endl;

    return 0;
}

#include "discovery.hpp"

int main(int argc, char **argv)
{
    Discovery discovery("255.255.255.255", 50000, 50001);

    discovery.start();
    std::cout << "start" << std::endl;
    discovery.stop();
    std::cout << "stop" << std::endl;

    return 0;
}

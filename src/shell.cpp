
#include "discovery.hpp"

int main(int argc, char **argv)
{
    Discovery discovery("255.255.255.255", 50001, 50000);

    discovery.start();
    std::cout << "start" << std::endl;
    discovery.stop();
    std::cout << "stop" << std::endl;

    return 0;
}

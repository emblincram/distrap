/**
 * @file test.cpp
 * @author Roman Koch (koch.roman@gmail.com)
 *
 * @copyright Copyright (c) 2025 Roman Koch
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include "toolbox_ip_address.hpp"

using namespace application::toolbox;

void test_ip_address() {
    IPAddress ip1(192, 168, 1, 10);
    std::cout << "IP1: " << ip1.toString() << " (uint32: " << ip1.toUInt32() << ")\n";

    IPAddress ip2 = IPAddress::fromString("10.0.0.1");
    std::cout << "IP2: " << ip2.toString() << " (uint32: " << ip2.toUInt32() << ")\n";
}

int main() {
    test_ip_address();

    return 0;
}

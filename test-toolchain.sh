#!/bin/bash

source /opt/poky/3.1.2/environment-setup-armv7at2hf-neon-poky-linux-gnueabi

cmake -DCMAKE_TOOLCHAIN_FILE=yocto-toolchain.cmake ..

make -j$(nproc)

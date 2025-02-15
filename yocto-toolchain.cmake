# Toolchain für Yocto-SDK auf ARMv7 mit Neon
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Compiler direkt setzen (vollständige Pfade)
set(CMAKE_C_COMPILER "/mnt/ssd/work/bbb-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "/mnt/ssd/work/bbb-sdk/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++")

# Cross-Compile-Sysroot setzen (WICHTIG!)
set(CMAKE_SYSROOT "/mnt/ssd/work/bbb-sdk/sysroots/armv7at2hf-neon-poky-linux-gnueabi")

# Standard CFLAGS und LDFLAGS aus Yocto übernehmen
set(CMAKE_C_FLAGS "-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -O2 -pipe -g -feliminate-unused-debug-types")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--sysroot=${CMAKE_SYSROOT} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-z,relro,-z,now")

# Finden von Bibliotheken und Includes innerhalb des Cross-Compile-Sysroots
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

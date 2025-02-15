# Vor der Nutzung das Yocto-SDK aktivieren
# Finde das Yocto SDK Environment

execute_process(
    COMMAND bash -c "echo $OECORE_NATIVE_SYSROOT"
    OUTPUT_VARIABLE SYSROOT_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND bash -c "echo $CXX"
    OUTPUT_VARIABLE CXX_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND bash -c "echo $CC"
    OUTPUT_VARIABLE C_COMPILER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSROOT ${SYSROOT_PATH})
set(CMAKE_C_COMPILER ${C_COMPILER})
set(CMAKE_CXX_COMPILER ${CXX_COMPILER})
set(CMAKE_FIND_ROOT_PATH ${SYSROOT_PATH})

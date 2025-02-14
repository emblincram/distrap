cmake_minimum_required(VERSION 3.10)

project(disp_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/application_a.cpp
    ${SRC_DIR}/application_b.cpp
    ${SRC_DIR}/data_record.cpp
    ${SRC_DIR}/discovery_message.cpp
    ${SRC_DIR}/udp_stub.cpp
    ${SRC_DIR}/udp_discovery.cpp
    ${SRC_DIR}/discovery.cpp
    ${SRC_DIR}/ip_address.cpp
)

set(HEADERS
    ${SRC_DIR}/application_a.hpp
    ${SRC_DIR}/application_b.hpp
    ${SRC_DIR}/data_record.hpp
    ${SRC_DIR}/discovery_message.hpp
    ${SRC_DIR}/udp_stub.hpp
    ${SRC_DIR}/udp_discovery.hpp
    ${SRC_DIR}/discovery.hpp
    ${SRC_DIR}/ip_address.hpp
)

add_executable(core ${SRC_DIR}/core.cpp ${SOURCES} ${HEADERS})
target_include_directories(core PRIVATE ${SRC_DIR})

add_executable(shell ${SRC_DIR}/shell.cpp ${SOURCES} ${HEADERS})
target_include_directories(shell PRIVATE ${SRC_DIR})

add_executable(test ${SRC_DIR}/test.cpp ${SOURCES} ${HEADERS})
target_include_directories(shell PRIVATE ${SRC_DIR})

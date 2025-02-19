# SPDX-License-Identifier: MIT
# SPDX-Author: Roman Koch <koch.romam@gmail.com>
# SPDX-Copyright: 2024 Roman Koch <koch.romam@gmail.com>

cmake_minimum_required(VERSION 3.10)

project(da)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/toolbox/toolbox_ip_address.cpp
    ${SRC_DIR}/discovery/discovery_message.cpp
    ${SRC_DIR}/discovery/discovery_handshake.cpp
    ${SRC_DIR}/transport/transport_packet.cpp
)

set(HEADERS
    ${SRC_DIR}/toolbox/toolbox_ip_address.hpp
    ${SRC_DIR}/discovery/discovery_message.hpp    
    ${SRC_DIR}/discovery/discovery_handshake.hpp
    ${SRC_DIR}/transport/transport_interface.hpp
    ${SRC_DIR}/transport/transport_packet.hpp
)

set(INCLUDE_DIRS
    ${SRC_DIR}
    ${SRC_DIR}/toolbox
    ${SRC_DIR}/discovery
    ${SRC_DIR}/transport
)

add_executable(${PROJECT_NAME}test ${SRC_DIR}/test.cpp ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}test PRIVATE ${INCLUDE_DIRS})

add_executable(${PROJECT_NAME}core ${SRC_DIR}/core.cpp ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}core PRIVATE ${INCLUDE_DIRS})

add_executable(${PROJECT_NAME}shell ${SRC_DIR}/shell.cpp ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}shell PRIVATE ${INCLUDE_DIRS})

set(CMAKE_INSTALL_PREFIX "/usr")

install(TARGETS ${PROJECT_NAME}test ${PROJECT_NAME}core ${PROJECT_NAME}shell
    RUNTIME DESTINATION bin
    #LIBRARY DESTINATION lib
    #ARCHIVE DESTINATION lib
)

cmake_minimum_required(VERSION 3.10)

project(disp_app)

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

add_executable(datest ${SRC_DIR}/test.cpp ${SOURCES} ${HEADERS})
target_include_directories(datest PRIVATE ${INCLUDE_DIRS})

add_executable(dacore ${SRC_DIR}/core.cpp ${SOURCES} ${HEADERS})
target_include_directories(dacore PRIVATE ${INCLUDE_DIRS})

add_executable(dashell ${SRC_DIR}/shell.cpp ${SOURCES} ${HEADERS})
target_include_directories(dashell PRIVATE ${INCLUDE_DIRS})

set(CMAKE_INSTALL_PREFIX "/usr")

install(TARGETS datest dacore dashell
    RUNTIME DESTINATION bin
    #LIBRARY DESTINATION lib
    #ARCHIVE DESTINATION lib
)

cmake_minimum_required(VERSION 3.10)

project(SoftwareSafetyReliabilityProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

if (WIN32)
    set(PLATFORM_LIBS ws2_32)
endif()

add_executable(client_app
    client/main.cpp
    client/client.cpp
    client/packet.cpp
    client/utils/logger.cpp
)

target_include_directories(client_app PRIVATE
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/client/utils
)

target_link_libraries(client_app PRIVATE
    ${OPENSSL_LIBRARIES}
    ${PLATFORM_LIBS}
)
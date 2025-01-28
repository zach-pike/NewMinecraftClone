cmake_minimum_required(VERSION 3.30.0)

set(
    CLIENT_CXX_FILES

    Main.cpp
    GameNetworkClient/GameNetworkClient.cpp
)

add_executable(Client ${CLIENT_CXX_FILES})
target_link_libraries(Client ENet)

target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/lib)
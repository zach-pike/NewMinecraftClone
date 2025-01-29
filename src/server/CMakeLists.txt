cmake_minimum_required(VERSION 3.30.0)

set(
    SERVER_CXX_FILES
    
    Main.cpp
    GameServer/GameServer.cpp
)

add_executable(Server ${SERVER_CXX_FILES})
target_link_libraries(Server ENet Common)

target_include_directories(Server PRIVATE ${CMAKE_SOURCE_DIR}/lib)
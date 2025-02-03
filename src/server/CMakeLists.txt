cmake_minimum_required(VERSION 3.30.0)

set(
    SERVER_CXX_FILES
    
    Main.cpp
    GameServer/GameServer.cpp
    GameServer/ServerWorld/ServerWorld.cpp
    GameServer/ServerWorld/ServerChunk/ServerChunk.cpp
)

add_executable(Server ${SERVER_CXX_FILES})

if (UNIX)
    target_link_libraries(Server PRIVATE ENet Common pthread)
else()
    target_link_libraries(Server PRIVATE ENet Common)
endif (UNIX)

target_include_directories(Server PRIVATE ${CMAKE_SOURCE_DIR}/lib)
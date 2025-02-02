cmake_minimum_required(VERSION 3.30.0)

add_library(ENet STATIC ENetImpl.cpp)

if(WIN32)
    target_link_libraries(ENet ws2_32 winmm)
endif()


target_include_directories(ENet PRIVATE ${CMAKE_SOURCE_DIR}/lib)

add_library(STBImage STATIC stb_image_impl.cpp)
target_include_directories(STBImage PRIVATE ${CMAKE_SOURCE_DIR}/lib)

set(COMMON_SOURCE
    Common/Types.cpp

    Common/Packets/PlayerState/PlayerState.cpp
    Common/Packets/UpdatePlayerState/UpdatePlayerState.cpp
    Common/Packets/ChunkRequest/ChunkRequest.cpp
    Common/Packets/ChunkResponse/ChunkResponse.cpp

    Common/Logger/Logger.cpp
)

add_library(Common STATIC ${COMMON_SOURCE})
target_include_directories(Common PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(Common PUBLIC ${CMAKE_SOURCE_DIR}/lib/glm/glm)

target_include_directories(Common PUBLIC ${CMAKE_SOURCE_DIR}/src/lib)
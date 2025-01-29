cmake_minimum_required(VERSION 3.30.0)

add_library(ENet STATIC ENetImpl.cpp)
target_link_libraries(ENet ws2_32 winmm)
target_include_directories(ENet PRIVATE ${CMAKE_SOURCE_DIR}/lib)

set(COMMON_SOURCE
    Common/PlayerState/PlayerState.cpp
)

add_library(Common STATIC ${COMMON_SOURCE})
target_include_directories(Common PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(Common PUBLIC ${CMAKE_SOURCE_DIR}/lib/glm/glm)

target_include_directories(Common PUBLIC ${CMAKE_SOURCE_DIR}/src/lib)
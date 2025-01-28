cmake_minimum_required(VERSION 3.30.0)

add_library(ENet STATIC ENetImpl.cpp)
target_link_libraries(ENet ws2_32 winmm)
target_include_directories(ENet PRIVATE ${CMAKE_SOURCE_DIR}/lib)

cmake_minimum_required(VERSION 3.30.0)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/lib/glad/glad/glad.c)
target_include_directories(glad PUBLIC 
    ${CMAKE_SOURCE_DIR}/lib/glad
)

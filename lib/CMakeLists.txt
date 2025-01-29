cmake_minimum_required(VERSION 3.30.0)

add_subdirectory(zlib/)
add_subdirectory(GLFW/)
add_subdirectory(glad/)

# Path to zlib include directory
set(ZLIB_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/zlib")

# Path to the generated zlib static library
set(ZLIB_LIBRARY "${CMAKE_BINARY_DIR}/lib/zlib/libzlibstatic.a") # Adjust based on your zlib build settings

# Mark these variables as cached so they persist into libnbtplusplus's CMake context
set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} CACHE PATH "Path to zlib include directory")
set(ZLIB_LIBRARY ${ZLIB_LIBRARY} CACHE FILEPATH "Path to zlib library")

add_subdirectory(libnbtplusplus/)

# Ensure libnbtplusplus or any other target includes zlib's paths
target_include_directories(nbt++ PRIVATE
    ${CMAKE_BINARY_DIR}/lib/zlib # Path to generated headers (e.g., zconf.h)
    ${CMAKE_SOURCE_DIR}/lib/zlib # Path to source headers (e.g., zlib.h)
)

set(IMGUI_SOURCE
    imgui/imgui_demo.cpp    
    imgui/imgui_draw.cpp    
    imgui/imgui_tables.cpp    
    imgui/imgui_widgets.cpp    
    imgui/imgui.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_library(IMGui STATIC ${IMGUI_SOURCE})
target_include_directories(IMGui PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(IMGui PUBLIC ${CMAKE_SOURCE_DIR}/lib/imgui)
target_include_directories(IMGui PUBLIC ${CMAKE_SOURCE_DIR}/lib/imgui/backends)
cmake_minimum_required(VERSION 3.30.0)

add_subdirectory(zlib/)

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
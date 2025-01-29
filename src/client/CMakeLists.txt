cmake_minimum_required(VERSION 3.30.0)

set(
    CLIENT_CXX_FILES

    Main.cpp
    GameNetworkClient/GameNetworkClient.cpp
    GameClient/GameClient.cpp
    GL/Shader/Shader.cpp
    GL/ShaderProgram/ShaderProgram.cpp
    GL/ShaderLoaders/VertexFragment.cpp
)

link_directories("${CMAKE_SOURCE_DIR}/lib/GLFW") 

add_executable(Client ${CLIENT_CXX_FILES})
target_link_libraries(Client PRIVATE ENet glfw3 glad IMGui Common)

target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/lib/glm/glm)
target_include_directories(Client PRIVATE ${CMAKE_SOURCE_DIR}/src/client)
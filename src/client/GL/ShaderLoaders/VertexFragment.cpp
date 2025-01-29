#include "VertexFragment.hpp"

#include <iostream>

std::unique_ptr<ShaderProgram> loadVertexFragmentShader(std::string basePath) {
    Shader vertexShader(GL_VERTEX_SHADER);
    vertexShader.addSourceCodeFromPath(basePath + "vertex.glsl");
    vertexShader.compile();

    if (!vertexShader.isShaderCompiled()) {
        std::string vertexShaderLog = vertexShader.getInfoLog();
        std::cout << vertexShaderLog << '\n';
        std::exit(1);
    }

    Shader fragmentShader(GL_FRAGMENT_SHADER);
    fragmentShader.addSourceCodeFromPath(basePath + "fragment.glsl");
    fragmentShader.compile();

    if (!fragmentShader.isShaderCompiled()) {
        std::string fragmentShaderLog = fragmentShader.getInfoLog();
        std::cout << fragmentShaderLog << '\n';
        std::exit(1);
    }

    // Assemble shaders together
    auto program = std::make_unique<ShaderProgram>();

    program->addShader(std::move(vertexShader));
    program->addShader(std::move(fragmentShader));

    program->linkProgram();

    if (!program->isProgramLinked()) {
        std::string programLog = program->getInfoLog();
        std::cout << programLog << '\n';
        std::exit(1);
    }

    program->getShaderList().clear();

    return program;
}
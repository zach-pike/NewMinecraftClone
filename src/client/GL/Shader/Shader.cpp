#include "Shader.hpp"

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(Shader&& other) {
    shader = other.shader;
    initialized = other.initialized;
    
    other.initialized = false;
}

Shader::Shader(GLenum shaderType) {
    shader = glCreateShader(shaderType);
    initialized = true;
}

Shader::~Shader() {
    if (initialized)
        glDeleteShader(shader);
}

void Shader::addSourceCode(std::string code) {
    const char* codeCStr = code.c_str();
    glShaderSource(shader, 1, &codeCStr, nullptr);
}

void Shader::addSourceCodeFromPath(std::string path) {
    std::ifstream f(path);

    if (!f.is_open()) throw std::runtime_error("Could not open " + path);

    std::stringstream ss;
    ss << f.rdbuf();
    f.close();

    addSourceCode(ss.str());
}

void Shader::compile() {
    glCompileShader(shader);
}

bool Shader::isShaderCompiled() {
    GLint value;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &value);

    return value == GL_TRUE;
}

GLenum Shader::getShaderType() {
    GLint shaderType;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
    return shaderType;
}

std::string Shader::getInfoLog() {
    int infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> shaderLogBuffer(infoLogLength);
    glGetShaderInfoLog(shader, infoLogLength, nullptr, &shaderLogBuffer[0]);
    
    return std::string(shaderLogBuffer.begin(), shaderLogBuffer.end());
}

GLuint Shader::getShaderId() {
    return shader;
}
#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram() {
    program = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(program);
}

void ShaderProgram::addShader(Shader&& shader) {
    shaders.push_back(std::move(shader));
}

std::vector<Shader>& ShaderProgram::getShaderList() {
    return shaders;
}

void ShaderProgram::linkProgram() {
    for(auto& shader : shaders) {
        glAttachShader(program, shader.getShaderId());
    }

    glLinkProgram(program);

    for(auto& shader : shaders) {
        glDetachShader(program, shader.getShaderId());
    }
}

bool ShaderProgram::isProgramLinked() {
    GLint value;
    glGetProgramiv(program, GL_LINK_STATUS, &value);
    return value == GL_TRUE;
}

std::string ShaderProgram::getInfoLog() {
    int infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> programLogBuffer(infoLogLength);
    glGetProgramInfoLog(program, infoLogLength, nullptr, &programLogBuffer[0]);

    return std::string(programLogBuffer.begin(), programLogBuffer.end());
}

void ShaderProgram::use() {
    glUseProgram(program);
}

void ShaderProgram::unbind() {
    glUseProgram(0);
}

GLuint ShaderProgram::getUniformLocation(std::string location) {
    return glGetUniformLocation(program, location.c_str());
}
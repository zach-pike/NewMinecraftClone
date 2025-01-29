#pragma once

#include "GL/Shader/Shader.hpp"

#include <vector>
#include <string>

class ShaderProgram {
private:
    std::vector<Shader> shaders;

    GLuint program;
public:
    ShaderProgram(const ShaderProgram&) = delete; // non construction-copyable
    ShaderProgram& operator=(const ShaderProgram&) = delete; // non copyable

    ShaderProgram();
    ~ShaderProgram();

    void addShader(Shader&& shader);
    std::vector<Shader>& getShaderList();

    void linkProgram();

    bool isProgramLinked();
    std::string getInfoLog();

    // Tell OpenGL to switch to this shader program;
    void use();

    void unbind();

    GLuint getUniformLocation(std::string location);
};
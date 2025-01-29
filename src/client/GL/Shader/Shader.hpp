#pragma once

#include "glad/glad.h"

#include <string>

class Shader {
private:
    GLuint shader;
    
    bool initialized = false;
public:
    Shader(const Shader&) = delete; // non construction-copyable
    Shader& operator=(const Shader&) = delete; // non copyable

    Shader(Shader&& other);
    Shader(GLenum shaderType);

    ~Shader();

    void addSourceCode(std::string code);
    void addSourceCodeFromPath(std::string path);

    void compile();

    bool isShaderCompiled();
    GLenum getShaderType();
    std::string getInfoLog();

    GLuint getShaderId();
};
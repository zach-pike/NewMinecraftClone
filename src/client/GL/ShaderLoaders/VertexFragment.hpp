#pragma once

#include <memory>
#include <string>

#include "GL/ShaderProgram/ShaderProgram.hpp"

std::unique_ptr<ShaderProgram> loadVertexFragmentShader(std::string basePath);
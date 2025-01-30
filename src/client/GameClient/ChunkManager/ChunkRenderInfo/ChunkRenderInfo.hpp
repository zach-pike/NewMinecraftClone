#pragma once

#include <memory>
#include "GL/ShaderProgram/ShaderProgram.hpp"

#include "glad/glad.h"

class ChunkRenderInfo {
public:
    struct TerrainRenderingUniforms {
        GLuint mvp, sampler;
    };
private:
    std::unique_ptr<ShaderProgram> terrainRenderShader;
    TerrainRenderingUniforms       terrainRenderUniforms;
    GLuint                         terrainTexture;

    void loadShaders();
    void unloadShaders();

    void loadTextures();
    void unloadTextures();
public:
    ChunkRenderInfo();
    ~ChunkRenderInfo();

    void reloadShaders();
    void reloadTextures();

    std::unique_ptr<ShaderProgram>& getTerrainRenderingShader();
    const TerrainRenderingUniforms& getTerrainRenderingUniforms() const;
    GLuint getTerrainTexture();
};
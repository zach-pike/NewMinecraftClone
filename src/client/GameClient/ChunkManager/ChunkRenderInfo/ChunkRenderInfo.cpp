#include "ChunkRenderInfo.hpp"

#include <cstdint>

#include "GL/ShaderLoaders/VertexFragment.hpp"

#include <stb_image.h>

void ChunkRenderInfo::loadShaders() {
    terrainRenderShader = loadVertexFragmentShader("./shader/chunk/");

    terrainRenderShader->use();
    terrainRenderUniforms.mvp = terrainRenderShader->getUniformLocation("MVP");
    terrainRenderUniforms.sampler = terrainRenderShader->getUniformLocation("samp");
    terrainRenderShader->unbind();
}

void ChunkRenderInfo::unloadShaders() {
    terrainRenderShader.reset();
}

void ChunkRenderInfo::loadTextures() {
    glGenTextures(1, &terrainTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, terrainTexture);

    const int numberTextures = 1;
    int sizeX = 16;
    int sizeY = 16;

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, sizeX, sizeY, numberTextures);
    const char* images[numberTextures] = { "resources\\dirt.png" };

    std::vector<std::uint8_t> imgdata;
    for (int i=0; i<numberTextures; i++) {
        int w, h;
        void* d = stbi_load(images[i], &w, &h, nullptr, 4);

        imgdata.insert(imgdata.end(), (std::uint8_t*)d, (std::uint8_t*)d + sizeX*sizeY*4);

        stbi_image_free(d);
    }
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, sizeX, sizeY, numberTextures, GL_RGBA, GL_UNSIGNED_BYTE, imgdata.data());

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void ChunkRenderInfo::unloadTextures() {
    glDeleteTextures(1, &terrainTexture);
}

ChunkRenderInfo::ChunkRenderInfo() {
    loadShaders();
    loadTextures();
}

ChunkRenderInfo::~ChunkRenderInfo() {
    unloadShaders();
    unloadTextures();
}

void ChunkRenderInfo::reloadShaders() {
    unloadShaders();
    loadShaders();
}

void ChunkRenderInfo::reloadTextures() {
    unloadTextures();
    loadTextures();
}

std::unique_ptr<ShaderProgram>& ChunkRenderInfo::getTerrainRenderingShader() {
    return terrainRenderShader;
}

const ChunkRenderInfo::TerrainRenderingUniforms& ChunkRenderInfo::getTerrainRenderingUniforms() const {
    return terrainRenderUniforms;
}

GLuint ChunkRenderInfo::getTerrainTexture() {
    return terrainTexture;
}
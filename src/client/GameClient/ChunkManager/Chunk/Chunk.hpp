#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "glad/glad.h"
#include "glm.hpp"

#include "Common/Types.hpp"
#include "GameClient/ChunkManager/ChunkRenderInfo/ChunkRenderInfo.hpp"

class ChunkManager;

class Chunk {
private:
    std::array<std::uint8_t, CHUNK_X * CHUNK_Y * CHUNK_Z> blockData;

    GLuint vertexIndexBuffer, vertexPositionBuffer, vertexTextureBuffer;
    std::size_t lastVertexCount;

    // Allows for temporarily diasbling the rendering of this mesh
    bool meshRendering = true;

    // is the mesh on the gpu outdated related to the block data?
    bool meshUpToDate = false;

    // are the buffers valid?
    bool buffersReady = false;

    void createBuffers();
    void deleteBuffers();
public:
    Chunk();
    ~Chunk();

    Chunk(Chunk&&) = delete;
    void operator=(Chunk&&) = delete;

    void setBlockData(std::vector<std::uint8_t>& data);

    bool drawChunk(ChunkCoordinate cc, ChunkManager& world);
    void renderChunk(std::shared_ptr<ChunkRenderInfo>& renderInfo, glm::mat4 MVP);

    void setChunkBlock(BlockCoordinate v, std::uint8_t block);
    std::uint8_t getChunkBlock(BlockCoordinate v) const;

    void tick();

    void markForRedraw();
};
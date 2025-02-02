#include "Chunk.hpp"

#include <vector>
#include <iostream>

#include "glm.hpp"

#include "GameClient/ChunkManager/ChunkManager.hpp"
#include "Block/Block.hpp"

#define ChunkIndexFormula(csx, csz, x, y, z) (x + z * csx + y * csx * csz)
#define ReverseChunkIndexFormula(csx, csz, idx) glm::ivec3( \
    (idx % csx), \
    (idx / (csx * csz)), \
    ((idx / csx) % csz) \
)

static const BlockFace cullingFaces[] = { BlockFace::NegativeX, BlockFace::PositiveX,
                                          BlockFace::NegativeY, BlockFace::PositiveY,
                                          BlockFace::NegativeZ, BlockFace::PositiveZ };

struct CullingOffset {
    int x, y, z;
};

static const CullingOffset cullingOffset[] = {
    CullingOffset{ -1, 0, 0 },
    CullingOffset{  1, 0, 0 },
    CullingOffset{ 0, -1, 0 },
    CullingOffset{ 0,  1, 0 },
    CullingOffset{ 0, 0, -1 },
    CullingOffset{ 0, 0,  1 },
};

template <typename T>
static T mod(T a, T b) {
    return (a % b + b) % b;
}

void Chunk::createBuffers() {
    if (buffersReady) return;

    glGenBuffers(1, &vertexIndexBuffer);
    glGenBuffers(1, &vertexPositionBuffer);
    glGenBuffers(1, &vertexTextureBuffer);

    buffersReady = true;
    meshUpToDate = true;
}

void Chunk::deleteBuffers() {
    if (!buffersReady) return;

    glDeleteBuffers(1, &vertexIndexBuffer);
    glDeleteBuffers(1, &vertexPositionBuffer);
    glDeleteBuffers(1, &vertexTextureBuffer);

    buffersReady = false;
}

Chunk::Chunk() {
    blockData.fill(0);
}

Chunk::~Chunk() {
    deleteBuffers();
}

void Chunk::setBlockData(std::vector<std::uint8_t>& data) {
    assert(data.size() == (CHUNK_X * CHUNK_Y * CHUNK_Z));
    std::copy(data.begin(), data.end(),  blockData.begin());
    
    meshUpToDate = false;
}

bool Chunk::drawChunk(ChunkCoordinate cc, ChunkManager& manager) {
    if (meshUpToDate) return false;

    if (!buffersReady) {
        createBuffers();
    }

    // Fun code
    std::vector<GLuint> vertexIndex;
    std::vector<glm::vec3> offsets;
    std::vector<GLuint> textureIndexs;

    auto addFace = [&](BlockFace face, glm::vec3 offset, GLuint textureIndex) {
        vertexIndex.push_back((GLuint)face*6 + 0);
        vertexIndex.push_back((GLuint)face*6 + 1);
        vertexIndex.push_back((GLuint)face*6 + 2);
        vertexIndex.push_back((GLuint)face*6 + 3);
        vertexIndex.push_back((GLuint)face*6 + 4);
        vertexIndex.push_back((GLuint)face*6 + 5);

        offsets.push_back(offset);
        offsets.push_back(offset);
        offsets.push_back(offset);
        offsets.push_back(offset);
        offsets.push_back(offset);
        offsets.push_back(offset);

        textureIndexs.push_back(textureIndex);
        textureIndexs.push_back(textureIndex);
        textureIndexs.push_back(textureIndex);
        textureIndexs.push_back(textureIndex);
        textureIndexs.push_back(textureIndex);
        textureIndexs.push_back(textureIndex);
    };

    auto addBlock = [&](glm::vec3 offset, GLuint blockID) {
        int worldX = cc.x * CHUNK_X + offset.x;
        int worldY = cc.y * CHUNK_Y + offset.y;
        int worldZ = cc.z * CHUNK_Z + offset.z;

        for (int i = 0; i < 6; i++) {
            CullingOffset off = cullingOffset[i];

            int checkX = offset.x + off.x;
            int checkY = offset.y + off.y;
            int checkZ = offset.z + off.z;

            bool isInsideChunk = (checkX >= 0 && checkX < CHUNK_X) &&
                                (checkY >= 0 && checkY < CHUNK_Y) &&
                                (checkZ >= 0 && checkZ < CHUNK_Z);

            if (isInsideChunk) {
                // Block to check is in the current chunk
                if (getChunkBlock(BlockCoordinate{ checkX, checkY, checkZ }) != 0) continue;
            } else {
                // Block is outside the current chunk, check the neighboring chunk
                int chunkOffsetX = (checkX < 0) ? -1 : (checkX >= CHUNK_X ? 1 : 0);
                int chunkOffsetY = (checkY < 0) ? -1 : (checkY >= CHUNK_Y ? 1 : 0);
                int chunkOffsetZ = (checkZ < 0) ? -1 : (checkZ >= CHUNK_Z ? 1 : 0);

                auto checkingChunk = ChunkCoordinate{ cc.x + chunkOffsetX, cc.y + chunkOffsetY, cc.z + chunkOffsetZ };
                if (manager.getChunks().count(checkingChunk)) {
                    const auto& chunk = manager.getChunks().at(checkingChunk);

                    // Adjust coordinates to be relative to the neighboring chunk
                    int localX = mod(checkX, CHUNK_X);
                    int localY = mod(checkY, CHUNK_Y);
                    int localZ = mod(checkZ, CHUNK_Z);

                    if (chunk->getChunkBlock(BlockCoordinate{ localX, localY, localZ }) != 0) continue;
                }
            }

            // If we reach here, the neighboring block is air, so add the face
            addFace((BlockFace)i, offset, blockTextureIDs[blockID][i]);
        }
    };
        
    for (int i=0; i<(CHUNK_X * CHUNK_Y * CHUNK_Z); i++) {
        glm::ivec3 p = ReverseChunkIndexFormula(CHUNK_X, CHUNK_Z, i);
        std::uint8_t b = getChunkBlock(BlockCoordinate{ p.x, p.y, p.z });

        if (b != 0)
            addBlock(p, b);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * vertexIndex.size(), vertexIndex.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * offsets.size(), offsets.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * textureIndexs.size(), textureIndexs.data(), GL_STATIC_DRAW);

    meshUpToDate = true;
    lastVertexCount = vertexIndex.size();

    return true;
}

void Chunk::renderChunk(std::shared_ptr<ChunkRenderInfo>& renderInfo, glm::mat4 MVP) {
    if (!meshRendering || !buffersReady) return;

    auto& shader = renderInfo->getTerrainRenderingShader();
    const auto& uniforms = renderInfo->getTerrainRenderingUniforms();
    
    shader->use();
    glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &MVP[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, renderInfo->getTerrainTexture());
    glUniform1i(uniforms.sampler, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexIndexBuffer);
    glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBuffer);
    glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, lastVertexCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Chunk::setChunkBlock(BlockCoordinate b, std::uint8_t v) {
    blockData.at(ChunkIndexFormula(CHUNK_X, CHUNK_Z, b.x, b.y, b.z)) = v;
}

std::uint8_t Chunk::getChunkBlock(BlockCoordinate b) const {
    return blockData.at(ChunkIndexFormula(CHUNK_X, CHUNK_Z, b.x, b.y, b.z));
}

void Chunk::tick() {

}

void Chunk::markForRedraw() {
    meshUpToDate = false;
}
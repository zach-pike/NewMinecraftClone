#include "ChunkManager.hpp"

#include "gtc/matrix_transform.hpp"

std::map<ChunkCoordinate, std::shared_ptr<Chunk>>& ChunkManager::getChunks() {
    return chunks;
}

void ChunkManager::renderWorld(std::shared_ptr<ChunkRenderInfo>& renderInfo, glm::mat4 viewProjection) {
    for (auto& kv : chunks) {
        auto& pos = kv.first;
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, pos.y, pos.z)*glm::vec3(CHUNK_X, CHUNK_Y, CHUNK_Z));
    
        kv.second->renderChunk(renderInfo, viewProjection * modelMatrix);
    }
}

void ChunkManager::unloadChunksGracefully() {
    chunks.clear();
}

void ChunkManager::tick() {
    for (auto& kv : chunks) {
        kv.second->drawChunk(kv.first, *this);
    }
}
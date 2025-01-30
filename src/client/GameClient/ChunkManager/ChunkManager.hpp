#pragma once

#include <map>
#include <memory>

#include "glm.hpp"

#include "ChunkRenderInfo/ChunkRenderInfo.hpp"
#include "Common/Types.hpp"
#include "Chunk/Chunk.hpp"

class ChunkManager {
private:
    std::map<ChunkCoordinate, std::shared_ptr<Chunk>> chunks;
public:
    ChunkManager() = default;
    ~ChunkManager() = default;

    std::map<ChunkCoordinate, std::shared_ptr<Chunk>>& getChunks();

    void renderWorld(std::shared_ptr<ChunkRenderInfo>& renderInfo, glm::mat4 viewProjection);

    void unloadChunksGracefully();
};

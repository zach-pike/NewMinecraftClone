#pragma once

#include <map>

#include "Common/Types.hpp"
#include "ServerChunk/ServerChunk.hpp"

class ServerWorld {
private:
    std::map<ChunkCoordinate, ServerChunk> worldChunks;
public:
    ServerWorld() = default;
    ~ServerWorld() = default;

    void generateWorld();
};
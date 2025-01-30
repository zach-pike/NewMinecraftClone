#pragma once

#include <map>
#include <memory>

#include "Common/Types.hpp"
#include "ServerChunk/ServerChunk.hpp"

class ServerWorld {
private:
    std::map<ChunkCoordinate, std::shared_ptr<ServerChunk>> worldChunks;
public:
    ServerWorld() = default;
    ~ServerWorld() = default;

    void generateWorld();

    void generateChunk(ChunkCoordinate c);
    std::shared_ptr<ServerChunk> getChunk(ChunkCoordinate c);
};
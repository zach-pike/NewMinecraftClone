#include "ServerWorld.hpp"

void ServerWorld::generateWorld() {}

void ServerWorld::generateChunk(ChunkCoordinate c) {
    auto chunk = std::make_shared<ServerChunk>();
    chunk->fillBlocks(0, 0, 0, 14, 1, 14, 1);
    worldChunks.insert({ c, chunk });
}

std::shared_ptr<ServerChunk> ServerWorld::getChunk(ChunkCoordinate c) {
    if (worldChunks.count(c) < 1) {
        generateChunk(c);
    }

    return worldChunks.at(c);
}
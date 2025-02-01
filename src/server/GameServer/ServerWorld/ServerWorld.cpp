#include "ServerWorld.hpp"

void ServerWorld::generateWorld() {}

void ServerWorld::generateChunk(ChunkCoordinate c) {
    auto chunk = std::make_shared<ServerChunk>();
    // chunk->fillBlocks(0, 0, 0, 15, 0, 15, 2);
    chunk->setChunkBlock(0, 0, 0, 2);
    chunk->setChunkBlock(1, 0, 0, 2);
    chunk->setChunkBlock(2, 0, 0, 2);
    chunk->setChunkBlock(1, 1, 0, 2);
    chunk->setChunkBlock(1, 2, 0, 2);
    worldChunks.insert({ c, chunk });
}

std::shared_ptr<ServerChunk> ServerWorld::getChunk(ChunkCoordinate c) {
    if (worldChunks.count(c) < 1) {
        generateChunk(c);
    }

    return worldChunks.at(c);
}
#include "ServerWorld.hpp"

void ServerWorld::generateWorld() {
    worldChunks.insert({ ChunkCoordinate{ 0, 0, 0}, ServerChunk() });
    worldChunks.begin()->second.fillBlocks(0, 0, 0, 14, 14, 14, 1);
}
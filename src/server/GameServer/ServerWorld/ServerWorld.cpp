#include "ServerWorld.hpp"

template <typename T>
inline
T clamp(T min, T max, T v) {
    return v < min ? min : (v > max ? max : v);
}

void ServerWorld::generateWorld() {}

void ServerWorld::generateChunk(ChunkCoordinate c) {
    auto chunk = std::make_shared<ServerChunk>();

    for (int x=0; x<=15; x++) {
        for (int z=0; z<=15; z++) {
            int noiseHeight = perlin.octave2D_01(((c.x * 16 + x) * 0.01), ((c.z * 16 + z) * 0.01), 4) * 65;

            int chunkYMin = c.y * CHUNK_Y;
            int chunkYMax = c.y * CHUNK_Y + (CHUNK_Y - 1);

            int fillMinY = clamp(chunkYMin, chunkYMax, 0);
            int fillMaxY = clamp(chunkYMin, chunkYMax, noiseHeight);

            int fillMinYOff = fillMinY - (c.y * CHUNK_Y);
            int fillMaxYOff = fillMaxY - (c.y * CHUNK_Y);


            for (int y=fillMinYOff; y<fillMaxYOff; y++) {
                chunk->setChunkBlock(x, y, z, 1);
            }

            if (fillMinYOff != fillMaxYOff) chunk->setChunkBlock(x, fillMaxYOff, z, 1);

            if (fillMaxY == noiseHeight) chunk->setChunkBlock(x, fillMaxYOff, z, 2);
        }
    }
    
    worldChunks.insert({ c, chunk });
}

std::shared_ptr<ServerChunk> ServerWorld::getChunk(ChunkCoordinate c) {
    if (worldChunks.count(c) < 1) {
        generateChunk(c);
    }

    return worldChunks.at(c);
}
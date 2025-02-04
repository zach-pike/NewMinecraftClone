#include "ServerWorld.hpp"

#include <iostream>

template <typename T>
inline
T clamp(T min, T max, T v) {
    return v < min ? min : (v > max ? max : v);
}

template <typename T>
static T mod(T a, T b) {
    return (a % b + b) % b;
}

float scalar(std::mt19937& rng) {
    return (float)rng() / rng.max();
}

void ServerWorld::generateWorld() {}

void ServerWorld::generateChunk(ChunkCoordinate c) {
    auto chunk = std::make_shared<ServerChunk>();

    std::mt19937 treeRng;

    float v = seed + (float)c.x / c.z;
    treeRng.seed(*(std::uint32_t*)&v);

    for (int x = 0; x < CHUNK_X; x++) {
        for (int z = 0; z < CHUNK_Z; z++) {
            int noiseHeight = perlin.octave2D_01(((c.x * CHUNK_X + x) * 0.01), ((c.z * CHUNK_Z + z) * 0.01), 4) * 65;
            float tree = scalar(treeRng);

            int chunkYMin = c.y * CHUNK_Y;
            int chunkYMax = c.y * CHUNK_Y + (CHUNK_Y - 1);

            int fillMinY = clamp(chunkYMin, chunkYMax, 0);
            int fillMaxY = clamp(chunkYMin, chunkYMax, noiseHeight);

            int fillMinYOff = fillMinY - (c.y * CHUNK_Y);
            int fillMaxYOff = fillMaxY - (c.y * CHUNK_Y);

            // Fill with dirt and grass
            for (int y = fillMinYOff; y < fillMaxYOff; y++) {
                chunk->setChunkBlock(x, y, z, 1); // Dirt
            }
            if (fillMinYOff != fillMaxYOff) {
                chunk->setChunkBlock(x, fillMaxYOff, z, 2); // Grass
            }

            // Generate tree if we should
            if (tree > 0.999) {
                int trunkBaseY = noiseHeight + 1;
                int trunkTopY = trunkBaseY + 2; // Trunk is 3 blocks tall

                // Generate trunk (only in the current chunk)
                for (int y = trunkBaseY; y <= trunkTopY; y++) {
                    int localY = y - (c.y * CHUNK_Y);
                    if (localY >= 0 && localY < CHUNK_Y) {
                        chunk->setChunkBlock(x, localY, z, 3); // Logs
                    }
                }

                // Generate leaves (only in the current chunk)
                int leavesBaseY = trunkTopY + 1;
                for (int lx = -1; lx <= 1; lx++) {
                    for (int lz = -1; lz <= 1; lz++) {
                        for (int ly = 0; ly <= 2; ly++) {
                            int localX = x + lx;
                            int localY = (leavesBaseY + ly) - (c.y * CHUNK_Y);
                            int localZ = z + lz;

                            // Check if the block is within the current chunk
                            if (localX >= 0 && localX < CHUNK_X &&
                                localY >= 0 && localY < CHUNK_Y &&
                                localZ >= 0 && localZ < CHUNK_Z) {
                                chunk->setChunkBlock(localX, localY, localZ, 4); // Leaves
                            }
                        }
                    }
                }
            }
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
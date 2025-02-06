#include "ServerWorld.hpp"

#include <iostream>
#include <utility>

#include "Common/Logger/Logger.hpp"

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

void ServerWorld::addUnplacedBlock(ChunkCoordinate c, BlockCoordinate b, std::uint8_t bt) {
    unplacedBlocks[c].push_back(std::pair<BlockCoordinate, std::uint8_t>(b, bt));
}

void ServerWorld::fillUnplacedBlocks(ChunkCoordinate location, std::shared_ptr<ServerChunk> chunk) {
    if (unplacedBlocks.count(location) < 1) return;
    
    auto& blocks = unplacedBlocks[location];

    for (auto& a : blocks) {
        chunk->setChunkBlock(a.first.x, a.first.y, a.first.z, a.second);
    }

    // Instead of erasing immediately, keep it until we're sure all are placed
    unplacedBlocks.erase(location);
}

void ServerWorld::generateWorld() {
    Logger logger("WorldGeneration", Logger::Color::DARK_BLUE);

    for (int x=-20; x<=20; x++) {
        for (int y=-20; y<=20; y++) {
            for (int z=-20; z<=20; z++) {
                generateChunk(ChunkCoordinate { x, y, z });
            }
        }

        float percent = 100.f * (float)(x + 20) / (20 + 20);

        logger.log("Generating world.. " + std::to_string(percent) + "%");
    }
}

void ServerWorld::generateChunk(ChunkCoordinate c) {
    auto chunk = std::make_shared<ServerChunk>();

    fillUnplacedBlocks(c, chunk);

    // Save on processing for chunks we know wont have terrain in them
    if (c.y*CHUNK_Y >= 100 || c.y*CHUNK_Y < 0) {
        worldChunks.insert({ c, chunk });
        return;
    }

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
            if (tree > .995) {
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
                            int leafY = (leavesBaseY + ly);
                            int localZ = z + lz;

                            // Clamp leaf Y value within the chunk height
                            if (leafY < c.y * CHUNK_Y || leafY >= (c.y + 1) * CHUNK_Y) {
                                continue; // Skip leaves above the chunk
                            }

                            int localY = leafY - (c.y * CHUNK_Y);

                            // Check if the block is within the current chunk
                            if (localX >= 0 && localX < CHUNK_X &&
                                localY >= 0 && localY < CHUNK_Y &&
                                localZ >= 0 && localZ < CHUNK_Z) {
                                chunk->setChunkBlock(localX, localY, localZ, 4); // Leaves
                            } else {
                                int chunkOffsetX = (localX < 0) ? -1 : (localX >= CHUNK_X ? 1 : 0);
                                int chunkOffsetY = (localY < 0) ? -1 : (localY >= CHUNK_Y ? 1 : 0);
                                int chunkOffsetZ = (localZ < 0) ? -1 : (localZ >= CHUNK_Z ? 1 : 0);

                                int cLocalX = mod(localX, CHUNK_X);
                                int cLocalY = mod(localY, CHUNK_Y);
                                int cLocalZ = mod(localZ, CHUNK_Z);

                                ChunkCoordinate chunkCoord{ c.x + chunkOffsetX, c.y + chunkOffsetY, c.z + chunkOffsetZ };
                                BlockCoordinate blockCoord{ cLocalX, cLocalY, cLocalZ };

                                //  Does the chunk exist?
                                if (worldChunks.count(chunkCoord)) {
                                    auto c = worldChunks.at(chunkCoord);
                                    c->setChunkBlock(cLocalX, cLocalY, cLocalZ, 4);
                                    dirtyChunks[chunkCoord].push_back(
                                        ChunkUpdate::Change{
                                            ChunkUpdate::ChangeType::PLACE,
                                            blockCoord,
                                            4
                                        }
                                    );

                                } else {
                                    addUnplacedBlock(chunkCoord, blockCoord, 4);
                                }
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

std::map<ChunkCoordinate, std::vector<ChunkUpdate::Change>>& ServerWorld::getDirtyChunks() {
    return dirtyChunks;
}
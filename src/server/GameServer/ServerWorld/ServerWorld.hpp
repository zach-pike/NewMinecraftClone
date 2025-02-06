#pragma once

#include <map>
#include <memory>
#include <random>
#include <set>

#include "Common/Packets/ChunkUpdate/ChunkUpdate.hpp"
#include "Common/Types.hpp"
#include "ServerChunk/ServerChunk.hpp"

#include "PerlinNoise.hpp"

class ServerWorld {
private:
    std::map<ChunkCoordinate, std::shared_ptr<ServerChunk>> worldChunks;
    std::map<ChunkCoordinate, std::vector<std::pair<BlockCoordinate, std::uint8_t>>> unplacedBlocks;

    const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

    void addUnplacedBlock(ChunkCoordinate c, BlockCoordinate b, std::uint8_t bt);
    void fillUnplacedBlocks(ChunkCoordinate location, std::shared_ptr<ServerChunk> chunk);

    std::map<ChunkCoordinate, std::vector<ChunkUpdate::Change>> dirtyChunks;
public:
    ServerWorld() = default;
    ~ServerWorld() = default;

    void generateWorld();

    void generateChunk(ChunkCoordinate c);
    std::shared_ptr<ServerChunk> getChunk(ChunkCoordinate c);

    std::map<ChunkCoordinate, std::vector<ChunkUpdate::Change>>& getDirtyChunks();
};
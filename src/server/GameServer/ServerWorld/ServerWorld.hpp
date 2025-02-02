#pragma once

#include <map>
#include <memory>

#include "Common/Types.hpp"
#include "ServerChunk/ServerChunk.hpp"

#include "PerlinNoise.hpp"

class ServerWorld {
private:
    std::map<ChunkCoordinate, std::shared_ptr<ServerChunk>> worldChunks;

    const siv::PerlinNoise::seed_type seed = 123456u;

	const siv::PerlinNoise perlin{ seed };
public:
    ServerWorld() = default;
    ~ServerWorld() = default;

    void generateWorld();

    void generateChunk(ChunkCoordinate c);
    std::shared_ptr<ServerChunk> getChunk(ChunkCoordinate c);
};
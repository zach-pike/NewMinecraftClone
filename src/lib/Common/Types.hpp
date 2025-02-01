#pragma once

#include <cstdint>

#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16

struct ChunkCoordinate {
    std::int32_t x, y, z;

    bool operator<(const ChunkCoordinate& other) const;
    bool operator==(const ChunkCoordinate& other) const;
};

struct BlockCoordinate {
    std::int32_t x, y, z;
};

enum class BlockFace : std::uint8_t {
    NegativeX = 0,
    PositiveX = 1,

    NegativeY = 2,
    PositiveY = 3,
    
    NegativeZ = 4,
    PositiveZ = 5,
};
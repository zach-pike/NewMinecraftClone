#pragma once

#include <cstdint>

struct ChunkCoordinate {
    std::int32_t x, y, z;

    bool operator<(const ChunkCoordinate& other) const;
};
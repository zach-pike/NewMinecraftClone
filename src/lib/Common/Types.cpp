#include "Types.hpp"

#include <cmath>

bool ChunkCoordinate::operator<(const ChunkCoordinate& other) const {
    if (x != other.x) return x < other.x;
    if (y != other.y) return y < other.y;
    return z < other.z;
}

bool ChunkCoordinate::operator==(const ChunkCoordinate& other) const {
    return x == other.x && y == other.y && z == other.z;
}

ChunkCoordinate chunkCoordinateFromWorldspaceCoords(BlockCoordinate bc) {
    return { 
        (std::uint32_t) std::floor(bc.x / 16.0),
        (std::uint32_t) std::floor(bc.y / 16.0),
        (std::uint32_t) std::floor(bc.z / 16.0)
    };
}
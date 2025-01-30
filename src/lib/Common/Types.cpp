#include "Types.hpp"

bool ChunkCoordinate::operator<(const ChunkCoordinate& other) const {
    if (x != other.x) return x < other.x;
    if (y != other.y) return y < other.y;
    return z < other.z;
}
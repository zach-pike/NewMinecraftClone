#pragma once

#include <array>
#include <cstdint>
#include <vector>

#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16

class ServerChunk {
private:
    std::array<std::uint8_t, CHUNK_X * CHUNK_Y * CHUNK_Z> blockData;

public:
    ServerChunk() = default;
    ~ServerChunk() = default;

    void setChunkBlock(int x, int y, int z, std::uint8_t v);
    std::uint8_t getChunkBlock(int x, int y, int z) const;

    void fillBlocks(int x1, int y1, int z1, int x2, int y2, int z2, std::uint8_t block);

    std::vector<std::uint8_t> getBlockData() const;
};
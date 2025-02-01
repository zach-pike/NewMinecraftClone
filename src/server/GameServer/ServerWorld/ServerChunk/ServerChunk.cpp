#include "ServerChunk.hpp"

#define ChunkIndexFormula(csx, csz, x, y, z) (x + z * csx + y * csx * csz)
#define ReverseChunkIndexFormula(csx, csz, idx) glm::ivec3( \
    (idx % csx), \
    (idx / (csx * csz)), \
    ((idx / csx) % csz) \
)

ServerChunk::ServerChunk() {
    blockData.fill(0);

    for (int i=0; i<10; i++) {
        if (i%2==0) {
            blockData.at(i) = 1;
        }
    }
}

void ServerChunk::setChunkBlock(int x, int y, int z, std::uint8_t v) {
    blockData.at(ChunkIndexFormula(CHUNK_X, CHUNK_Z, x, y, z)) = v;
}

std::uint8_t ServerChunk::getChunkBlock(int x, int y, int z) const {
    return blockData.at(ChunkIndexFormula(CHUNK_X, CHUNK_Z, x, y, z));
}

void ServerChunk::fillBlocks(int x1, int y1, int z1, int x2, int y2, int z2, std::uint8_t block) {
    for (int x=x1; x <= x2; x++) {
        for (int y=y1; y <= y2; y++) {
            for (int z=z1; z <= z2; z++) {
                blockData.at(ChunkIndexFormula(CHUNK_X, CHUNK_Z, x, y, z)) = block;
            }
        }
    }
}

std::vector<std::uint8_t> ServerChunk::getBlockData() const {
    return std::move(std::vector<std::uint8_t>(blockData.begin(), blockData.end()));
}
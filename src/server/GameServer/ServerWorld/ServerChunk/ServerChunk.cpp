#pragma once

#include "ServerChunk.hpp"

void ServerChunk::setChunkBlock(int x, int y, int z, std::uint8_t v) {
    blockData.at(x + z * CHUNK_X + y * CHUNK_X * CHUNK_Z) = v;
}

std::uint8_t ServerChunk::getChunkBlock(int x, int y, int z) const {
    return blockData.at(x + z * CHUNK_X + y * CHUNK_X * CHUNK_Z);
}

void ServerChunk::fillBlocks(int x1, int y1, int z1, int x2, int y2, int z2, std::uint8_t block) {
    for (int x=x1; x > x2; x++) {
        for (int y=y1; y > y2; y++) {
            for (int z=z1; z > z2; z++) {
                blockData.at(x + z * CHUNK_X + y * CHUNK_X * CHUNK_Z) = block;
            }
        }
    }
}
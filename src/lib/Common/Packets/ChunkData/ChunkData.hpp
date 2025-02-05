#pragma once

#include "enet.h"

#include "Common/Serialize/Serializable.hpp"
#include "Common/Types.hpp"

class ChunkData : public Serializable {
public:
    ChunkCoordinate chunkCoord;
    std::vector<std::uint8_t> blockData;

    ChunkData() = default;
    ~ChunkData() = default;

    std::vector<std::uint8_t> serialize() const;
    bool deserialize(const std::vector<std::uint8_t>& data);

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
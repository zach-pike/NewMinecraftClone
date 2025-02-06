#pragma once

#include "enet.h"
#include "Common/Types.hpp"
#include "Common/Serialize/Serializable.hpp"

class ChunkUpdate : public Serializable {
public:
    enum class ChangeType {
        BREAK = 0,
        PLACE = 1
    };

    struct Change {
        ChangeType type;
        BlockCoordinate coord;
        std::uint8_t blockID;
    };

    ChunkCoordinate chunkCoord;
    std::vector<Change> changes;

    ChunkUpdate() = default;
    ~ChunkUpdate() = default;

    std::vector<std::uint8_t> serialize() const;
    bool deserialize(const std::vector<std::uint8_t>& data);

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
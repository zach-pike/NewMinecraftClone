#pragma

#include "enet.h"

#include "Common/Serialize/Serializable.hpp"
#include "Common/Types.hpp"

class ChunkResponse : public Serializable {
public:
    ChunkCoordinate requestedChunk;
    std::vector<std::uint8_t> blockData;

    ChunkResponse() = default;
    ~ChunkResponse() = default;

    std::vector<std::uint8_t> serialize() const;
    bool deserialize(const std::vector<std::uint8_t>& data);

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
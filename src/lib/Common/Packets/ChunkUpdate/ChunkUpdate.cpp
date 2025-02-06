#include "ChunkUpdate.hpp"

#include "Common/Packets/PacketType.hpp"

std::vector<std::uint8_t> ChunkUpdate::serialize() const {
    std::vector<std::uint8_t> bytes;
    bytes.push_back((std::uint8_t)PacketType::ChunkUpdate);

    bytes.insert(
        bytes.end(), 
        (std::uint8_t*)&chunkCoord, 
        (std::uint8_t*)&chunkCoord + sizeof(ChunkCoordinate)
    );
    
    std::uint32_t changesSize = changes.size();

    bytes.insert(
        bytes.end(),
        (std::uint8_t*)&changesSize,
        (std::uint8_t*)&changesSize + sizeof(std::uint32_t)
    );

    bytes.insert(
        bytes.end(),
        (std::uint8_t*)changes.data(),
        (std::uint8_t*)changes.data() + sizeof(Change)*changes.size()
    );

    return std::move(bytes);
}

bool ChunkUpdate::deserialize(const std::vector<std::uint8_t>& data) {
    assert(data.size() > 0);
    assert((PacketType)data[0] == PacketType::ChunkUpdate);

    const std::uint8_t* dataPtr = data.data();

    assert(data.size() >= sizeof(ChunkCoordinate));
    chunkCoord = *(ChunkCoordinate*)(dataPtr + 1);

    assert(data.size() >= (sizeof(ChunkCoordinate) + sizeof(std::uint32_t)));
    std::uint32_t numberOfUpdates = *(std::uint32_t*)(dataPtr + 1 + sizeof(ChunkCoordinate));

    assert(data.size() >= sizeof(ChunkCoordinate) + sizeof(std::uint32_t) + sizeof(Change)*numberOfUpdates);
    const Change* changeBeginPtr = (Change*)(dataPtr + 1 + sizeof(ChunkCoordinate) + sizeof(std::uint32_t));
    const Change* changeEndPtr = &changeBeginPtr[numberOfUpdates];
    
    changes = std::vector<Change>(changeBeginPtr, changeEndPtr);

    return true;
}

ENetPacket* ChunkUpdate::convToPacket() const {
    auto bytes = serialize();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

// Assumes ownership of packet
void ChunkUpdate::decodePacket(ENetPacket* packet) {
    deserialize(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    enet_packet_destroy(packet);
}
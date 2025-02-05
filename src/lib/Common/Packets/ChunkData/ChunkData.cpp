#include "ChunkData.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/Packets/PacketType.hpp"

std::vector<std::uint8_t> ChunkData::serialize() const {
    std::vector<std::uint8_t> bytes;
    bytes.push_back((std::uint8_t)PacketType::ChunkData);
    bytes.insert(bytes.end(), (std::uint8_t*)&chunkCoord, (std::uint8_t*)&chunkCoord + sizeof(ChunkCoordinate));
    bytes.insert(bytes.end(), blockData.begin(), blockData.end());

    return std::move(bytes);
}

bool ChunkData::deserialize(const std::vector<std::uint8_t>& data) {
    assert(data.size() == ( 1 + sizeof(ChunkCoordinate) + (CHUNK_X * CHUNK_Y * CHUNK_Z)));
    assert((PacketType)data[0] == PacketType::ChunkData);

    chunkCoord = *(ChunkCoordinate*)(data.data() + 1);

    blockData.insert(blockData.begin(), data.begin() + 1 + sizeof(ChunkCoordinate), data.end());

    return true;
}

ENetPacket* ChunkData::convToPacket() const {
    auto bytes = serialize();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

// Assumes ownership of packet
void ChunkData::decodePacket(ENetPacket* packet) {
    deserialize(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    enet_packet_destroy(packet);
}
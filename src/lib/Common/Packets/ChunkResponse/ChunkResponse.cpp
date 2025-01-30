#include "ChunkResponse.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/Packets/PacketType.hpp"

std::vector<std::uint8_t> ChunkResponse::serialize() const {
    std::vector<std::uint8_t> bytes;
    bytes.push_back((std::uint8_t)PacketType::ChunkResponse);
    bytes.insert(bytes.end(), blockData.begin(), blockData.end());

    return std::move(bytes);
}

bool ChunkResponse::deserialize(const std::vector<std::uint8_t>& data) {
    assert(data.size() == ( 1 + (CHUNK_X * CHUNK_Y * CHUNK_Z)));
    assert((PacketType)data[0] == PacketType::ChunkResponse);

    blockData.insert(blockData.begin(), data.begin() + 1, data.end());

    return true;
}

ENetPacket* ChunkResponse::convToPacket() const {
    auto bytes = serialize();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

// Assumes ownership of packet
void ChunkResponse::decodePacket(ENetPacket* packet) {
    deserialize(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    enet_packet_destroy(packet);
}
#include "ChunkResponse.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/Packets/PacketType.hpp"

std::vector<std::uint8_t> ChunkResponse::serialize() const {
    Serializer s;

    s << PacketType::ChunkResponse;
    s << blockData;

    return s.getBytes();
}

bool ChunkResponse::deserialize(const std::vector<std::uint8_t>& data) {
    Serializer s(data);

    s >> blockData;
    
    PacketType pt;
    s >> pt;

    assert(pt == PacketType::ChunkResponse);

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
#include "ChunkRequest.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/Packets/PacketType.hpp"

std::vector<std::uint8_t> ChunkRequest::serialize() const {
    Serializer s;

    s << PacketType::ChunkRequest;
    s << requestedChunk;

    return s.getBytes();
}

bool ChunkRequest::deserialize(const std::vector<std::uint8_t>& data) {
    Serializer s(data);

    s >> requestedChunk;

    PacketType pt;
    s >> pt;
    assert(pt == PacketType::ChunkRequest);

    return true;
}

ENetPacket* ChunkRequest::convToPacket() const {
    auto bytes = serialize();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

// Assumes ownership of packet
void ChunkRequest::decodePacket(ENetPacket* packet) {
    deserialize(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    enet_packet_destroy(packet);
}
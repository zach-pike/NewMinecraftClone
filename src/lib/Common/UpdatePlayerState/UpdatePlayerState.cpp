#include "UpdatePlayerState.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/PacketType.hpp"

std::vector<std::uint8_t> UpdatePlayerState::serialize() const {
    Serializer s;

    s << PacketType::UpdatePlayerState;
    s << userToUpdate;
    s << playerState;

    return s.getBytes();
}

bool UpdatePlayerState::deserialize(const std::vector<std::uint8_t>& data) {
    Serializer s(data);

    s >> playerState;
    s >> userToUpdate;

    PacketType p;
    s >> p;
    assert(p == PacketType::UpdatePlayerState);

    return true;
}

ENetPacket* UpdatePlayerState::convToPacket() const {
    auto bytes = serialize();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

// Assumes ownership of packet
void UpdatePlayerState::decodePacket(ENetPacket* packet) {
    deserialize(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    enet_packet_destroy(packet);
}
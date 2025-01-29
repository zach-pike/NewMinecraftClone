#include "PlayerState.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/PacketType.hpp"

ENetPacket* PlayerState::convToPacket() const {
    Serializer s;

    s << PacketType::PlayerState;
    s << playerPosition.x << playerPosition.y << playerPosition.z;
    s << cameraPitch << cameraYaw;


    auto& bytes = s.getBytes();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

void PlayerState::decodePacket(ENetPacket* packet) {
    Serializer s(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    
    s >> cameraYaw >> cameraPitch;
    s >> playerPosition.z >> playerPosition.y >> playerPosition.x;
     
    PacketType p;
    s >> p;

    assert(p == PacketType::PlayerState);

    enet_packet_destroy(packet);
}
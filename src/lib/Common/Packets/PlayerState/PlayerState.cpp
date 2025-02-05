#include "PlayerState.hpp"

#include "Common/Serialize/Serializer.hpp"
#include "Common/Packets/PacketType.hpp"

static inline int worldToChunkCoord(int worldCoord, int chunkSize) {
    return (worldCoord < 0) ? ((worldCoord + 1) / chunkSize) - 1 : (worldCoord / chunkSize);
}

ChunkCoordinate PlayerState::getChunkCoordinate() const {
    int chunkX = worldToChunkCoord(playerPosition.x, CHUNK_X);
    int chunkY = worldToChunkCoord(playerPosition.y, CHUNK_Y);
    int chunkZ = worldToChunkCoord(playerPosition.z, CHUNK_Z);

    return ChunkCoordinate { chunkX, chunkY, chunkZ };
}

std::vector<std::uint8_t> PlayerState::serialize() const {
    Serializer s;

    s << PacketType::PlayerState;
    s << playerPosition.x << playerPosition.y << playerPosition.z;
    s << cameraPitch << cameraYaw;

    return s.getBytes();
}

bool PlayerState::deserialize(const std::vector<std::uint8_t>& data) {
    Serializer s(data);
    
    s >> cameraYaw >> cameraPitch;
    s >> playerPosition.z >> playerPosition.y >> playerPosition.x;
     
    PacketType p;
    s >> p;

    assert(p == PacketType::PlayerState);

    return true;
}

ENetPacket* PlayerState::convToPacket() const {
    auto bytes = serialize();

    return enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);
}

void PlayerState::decodePacket(ENetPacket* packet) {
    deserialize(std::vector<std::uint8_t>(packet->data, packet->data + packet->dataLength));
    enet_packet_destroy(packet);
}
#pragma once

#include "glm.hpp"
#include "enet.h"

#include "Common/Serialize/Serializable.hpp"
#include "Common/Types.hpp"

class PlayerState : public Serializable {
public:
    glm::vec3 playerPosition;
    double cameraPitch, cameraYaw;

    PlayerState() = default;
    ~PlayerState() = default;

    ChunkCoordinate getChunkCoordinate() const;

    std::vector<std::uint8_t> serialize() const;
    bool deserialize(const std::vector<std::uint8_t>& data);

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
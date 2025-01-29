#pragma once

#include "glm.hpp"

#include "enet.h"

class PlayerState {
public:
    glm::vec3 playerPosition;
    double cameraPitch, cameraYaw;

    PlayerState() = default;
    ~PlayerState() = default;

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
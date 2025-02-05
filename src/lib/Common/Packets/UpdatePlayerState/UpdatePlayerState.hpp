#pragma once

#include <cstdint>
#include "enet.h"

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Serialize/Serializable.hpp"
#include "Common/Types.hpp"

class UpdatePlayerState : public Serializable {
public:
    PlayerUUID userToUpdate;
    PlayerState playerState;

    std::vector<std::uint8_t> serialize() const;
    bool deserialize(const std::vector<std::uint8_t>& data);

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
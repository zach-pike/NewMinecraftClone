#pragma once

#include <cstdint>
#include "enet.h"

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Serialize/Serializable.hpp"

class UpdatePlayerState : public Serializable {
public:
    std::uint64_t userToUpdate;
    PlayerState playerState;

    std::vector<std::uint8_t> serialize() const;
    bool deserialize(const std::vector<std::uint8_t>& data);

    ENetPacket* convToPacket() const;

    // Assumes ownership of packet
    void decodePacket(ENetPacket* packet);
};
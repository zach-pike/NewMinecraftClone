#pragma once

#include <cstdint>

#include "enet.h"

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Types.hpp"

class Player {
private:
    ENetPeer* peer;
    PlayerState playerState;
public:
    Player(ENetPeer* peer);
    ~Player();

    ENetPeer* getPeer();
    PlayerState& getPlayerState();

    PlayerUUID getPlayerUUID() const;

    static PlayerUUID getPlayerUUIDFromPeer(const ENetPeer* p);
};
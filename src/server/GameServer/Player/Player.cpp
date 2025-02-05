#include "Player.hpp"

#include "Common/Utility/Hash.hpp"

Player::Player(ENetPeer* p):
    peer(p) {}

Player::~Player() {
    enet_peer_reset(peer);
}

ENetPeer* Player::getPeer() {
    return peer;
}

PlayerState& Player::getPlayerState() {
    return playerState;
}

PlayerUUID Player::getPlayerUUID() const {
    return hashBytes(&peer->address, sizeof(ENetAddress));
}

PlayerUUID Player::getPlayerUUIDFromPeer(const ENetPeer* p) {
    return hashBytes(&p->address, sizeof(ENetAddress));
}
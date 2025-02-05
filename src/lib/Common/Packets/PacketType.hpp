#pragma once

#include <cstdint>

enum class PacketType : std::uint8_t {
    PlayerState = 0,
    UpdatePlayerState = 1,

    ChunkData = 2
};
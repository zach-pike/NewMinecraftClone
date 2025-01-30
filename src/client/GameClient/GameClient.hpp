#pragma once

#include <atomic>
#include <thread>
#include <memory>
#include <map>

#include "ChunkManager/ChunkManager.hpp"

#include "GameNetworkClient/GameNetworkClient.hpp"
#include "Common/Packets/PlayerState/PlayerState.hpp"

class GameClient {
private:
    GameNetworkClient networkClient;

    std::atomic_bool renderThreadRunning = false;
    std::thread renderThread;
    void _renderThread();

    std::shared_ptr<ChunkManager> chunkManager = nullptr;
    PlayerState playerData;

    std::map<std::uint64_t, PlayerState> players;
public:
    GameClient();
    ~GameClient();

    void startGame();
    void stopGame();

    bool isRenderThreadRunning() const;
};
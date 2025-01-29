#pragma once

#include <atomic>
#include <thread>

#include "GameNetworkClient/GameNetworkClient.hpp"

#include "Common/PlayerState/PlayerState.hpp"

class GameClient {
private:
    GameNetworkClient networkClient;

    std::atomic_bool renderThreadRunning = false;
    std::thread renderThread;
    void _renderThread();

    PlayerState playerData;
public:
    GameClient();
    ~GameClient();

    void startGame();
    void stopGame();

    bool isRenderThreadRunning() const;
};
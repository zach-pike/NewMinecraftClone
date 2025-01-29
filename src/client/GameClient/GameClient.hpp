#pragma once

#include <atomic>
#include <thread>
#include <map>

#include "GameNetworkClient/GameNetworkClient.hpp"

#include "Common/PlayerState/PlayerState.hpp"

class GameClient {
private:
    GameNetworkClient networkClient;

    std::atomic_bool renderThreadRunning = false;
    std::thread renderThread;
    void _renderThread();

    PlayerState playerData;

    std::map<std::uint64_t, PlayerState> players;
public:
    GameClient();
    ~GameClient();

    void startGame();
    void stopGame();

    bool isRenderThreadRunning() const;
};
#pragma once

#include <atomic>
#include <thread>

class GameServer {
private:
    std::atomic_bool networkThreadRunning = false;
    std::thread networkThread;
    void _networkThreadFunc();
public:
    GameServer();
    ~GameServer();

    void startServer();
    void stopServer();
};
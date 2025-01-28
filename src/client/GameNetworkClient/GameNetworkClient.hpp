#pragma once

#include <string>
#include <cstdint>
#include <atomic>
#include <thread>

class GameNetworkClient {
private:
    std::atomic_bool networkThreadRunning = false;
    std::atomic_bool workerConnected = false;
    std::thread networkThread;
    void _networkThreadFunc(std::string host, std::uint16_t port);
public:
    GameNetworkClient();
    ~GameNetworkClient();

    void connectToHost(std::string host, std::uint16_t port);
    void disconnect();

    bool isConnected() const;
};
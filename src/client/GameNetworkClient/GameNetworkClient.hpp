#pragma once

#include "enet.h"

#include <string>
#include <cstdint>
#include <atomic>
#include <thread>
#include <deque>

class GameNetworkClient {
private:
    std::atomic_bool networkThreadRunning = false;
    std::atomic_bool workerConnected = false;
    std::thread networkThread;
    void _networkThreadFunc(std::string host, std::uint16_t port);

    std::mutex inQueueLock;
    std::deque<ENetPacket*> inQueue;

    std::mutex outQueueLock;
    std::deque<ENetPacket*> outQueue;
public:
    GameNetworkClient();
    ~GameNetworkClient();

    void connectToHost(std::string host, std::uint16_t port);
    void disconnect();

    bool isConnected() const;

    void addToOutQueue(ENetPacket* p);

    bool messagesAvailable();
    ENetPacket* popMessage();
};
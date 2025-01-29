#pragma once

#include <atomic>
#include <thread>
#include <deque>
#include <map>
#include <optional>

#include "Common/PlayerState/PlayerState.hpp"

class GameServer {
public:
    struct IdentifiedPacket {
        ENetPeer* peer;
        ENetPacket* packet;
    };
private:
    std::mutex connectedPlayersLock;
    std::map<ENetPeer*, std::optional<PlayerState>> connectedPlayers;

    std::atomic_bool networkThreadRunning = false;
    std::thread networkThread;
    void _networkThreadFunc();

    std::mutex outQueueLock;
    std::deque<IdentifiedPacket> outQueue;

public:
    GameServer();
    ~GameServer();

    void startServer();
    void stopServer();

    void addToOutQueue(ENetPeer* peer, ENetPacket* p);

    void printPlayerList();
};
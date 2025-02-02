#pragma once

#include <atomic>
#include <thread>
#include <deque>
#include <map>
#include <optional>

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Logger/Logger.hpp"

#include "ServerWorld/ServerWorld.hpp"

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

    std::atomic_bool gameThreadRunning = false;
    std::thread gameThread;
    void _gameThreadFunc();

    std::mutex outQueueLock;
    std::deque<IdentifiedPacket> outQueue;

    std::mutex broadcastQueueLock;
    std::deque<ENetPacket*> broadcastQueue;

    std::mutex inQueueLock;
    std::deque<IdentifiedPacket> inQueue;

    void addToOutQueue(ENetPeer* peer, ENetPacket* p);
    void addToBroadcastQueue(ENetPacket* p);

    bool messagesAvailable();
    IdentifiedPacket popMessage();

    Logger logger;

    ServerWorld world;
public:
    GameServer();
    ~GameServer();

    void startServer();
    void stopServer();
    
    void printPlayerList();
};
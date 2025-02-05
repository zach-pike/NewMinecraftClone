#pragma once

#include <atomic>
#include <thread>
#include <deque>
#include <map>
#include <set>
#include <optional>

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Logger/Logger.hpp"

#include "ServerWorld/ServerWorld.hpp"

#include "Player/Player.hpp"

class GameServer {
public:
    struct IdentifiedPacket {
        ENetPeer* peer;
        ENetPacket* packet;
    };
private:
    // Accessed by network and game thread
    std::mutex connectedPlayersLock;
    std::map<ENetPeer*, std::shared_ptr<Player>> connectedPlayers;

    std::mutex playerIDToPlayerMapLock;
    std::map<PlayerUUID, std::shared_ptr<Player>> playerIDToPlayerMap;

    std::mutex newPlayersLock;
    std::vector<std::shared_ptr<Player>> newPlayers;

    std::mutex disconnectingPlayersLock;
    std::vector<std::shared_ptr<Player>> disconnectingPlayers;

    std::atomic_bool networkThreadRunning = false;
    std::thread networkThread;
    void _networkThreadFunc();

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
    
    std::atomic_bool gameThreadRunning = false;
    std::thread gameThread;
    void _gameThreadFunc();

    // Only accessed by game thread
    std::map<PlayerUUID, std::set<ChunkCoordinate>> playerLoadedChunks;
    std::map<ChunkCoordinate, std::set<PlayerUUID>> chunkSubscribers;

    Logger logger;

    ServerWorld world;

    void updatePlayerChunks(PlayerUUID player);
public:
    GameServer();
    ~GameServer();

    void startServer();
    void stopServer();
    
    void printPlayerList();
};
#include "GameServer.hpp"

#include "enet.h"
#include "Common/Packets/PacketType.hpp"

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Packets/UpdatePlayerState/UpdatePlayerState.hpp"
#include "Common/Packets/ChunkRequest/ChunkRequest.hpp"
#include "Common/Packets/ChunkResponse/ChunkResponse.hpp"

#include <stdexcept>
#include <functional>
#include <iostream>
#include <cstring>


#define MAX_CLIENTS 32

void GameServer::_networkThreadFunc() {
    enet_initialize();

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = 7777; /* Bind the server to port 7777. */

    /* create a server */
    ENetHost* server = enet_host_create(&address, MAX_CLIENTS, 1, 0, 0);

    if (server == nullptr) {
        throw std::runtime_error("Failed to create server!");
    }

    std::cout << "Server running!\n";

    while (networkThreadRunning) {
        ENetEvent event;

        int rc = enet_host_service(server, &event, 0);
        if (rc < 0) {
            // Stop server in future
        }

        // Send queued messages
        outQueueLock.lock();
        for (auto& p : outQueue) {
            if (enet_peer_send(p.peer, 0, p.packet) < 0) {
                enet_packet_destroy(p.packet);
            }
        }
        outQueue.clear();
        outQueueLock.unlock();

        // Send broadcast messages
        broadcastQueueLock.lock();
        for (auto& p : broadcastQueue) {
            enet_host_broadcast(server, 0, p);
        }
        broadcastQueue.clear();
        broadcastQueueLock.unlock();

        if (rc == 0) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms);
        }

        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                std::cout << "A new client connected!\n";

                connectedPlayersLock.lock();
                connectedPlayers.insert({ event.peer, std::nullopt });
                connectedPlayersLock.unlock();
            } break;

            case ENET_EVENT_TYPE_RECEIVE:{
                ENetPacket* packet = event.packet;

                inQueueLock.lock();
                inQueue.push_back(IdentifiedPacket{
                    .peer = event.peer,
                    .packet = packet
                });
                inQueueLock.unlock();

            } break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                std::cout << "User disconnected.\n";
                connectedPlayersLock.lock();
                connectedPlayers.erase(event.peer);
                connectedPlayersLock.unlock();
            } break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                std::cout << "User timed out.\n";
                connectedPlayersLock.lock();
                connectedPlayers.erase(event.peer);
                connectedPlayersLock.unlock();
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }

    std::cout << "Network Server closed!\n";

    enet_host_destroy(server);
    enet_deinitialize();
}

void GameServer::_gameThreadFunc() {
    // Generate world


    int tickC = 0;

    while(gameThreadRunning) {
        auto tickBegin = std::chrono::steady_clock::now();

        // Do work here
        // Process incoming messages
        inQueueLock.lock();

        for (auto& msgs : inQueue) {
            // Get first byte of packet to determine type

            assert(msgs.packet->dataLength > 0);
            PacketType pt = (PacketType)msgs.packet->data[0];

            switch(pt) {
                case PacketType::PlayerState: {
                    PlayerState ps;
                    ps.decodePacket(msgs.packet);

                    connectedPlayersLock.lock();
                    connectedPlayers.at(msgs.peer) = ps;
                    connectedPlayersLock.unlock();
                } break;

                case PacketType::ChunkRequest: {
                    ChunkRequest cr;
                    cr.decodePacket(msgs.packet);
                    
                    std::shared_ptr<ServerChunk> chunk = world.getChunk(cr.requestedChunk);

                    ChunkResponse cresp;
                    cresp.requestedChunk = cr.requestedChunk;
                    cresp.blockData = chunk->getBlockData();

                    addToOutQueue(msgs.peer, cresp.convToPacket());
                } break;
            }
        }

        inQueue.clear();
        inQueueLock.unlock();

        if (tickC % 2 == 0) {
            // Send out player position data
            connectedPlayersLock.lock();
            for (auto& player : connectedPlayers) {
                if (!player.second.has_value()) continue;

                std::uint64_t id = *(std::uint64_t*)(player.first->address.host.u.Byte) + player.first->address.port;

                UpdatePlayerState ups;
                ups.userToUpdate = id;
                ups.playerState = player.second.value();
                
                ENetPacket* p = ups.convToPacket();

                addToBroadcastQueue(p);
            }
            connectedPlayersLock.unlock();
        }

        tickC ++;

        // End of tick
        auto tickEnd = std::chrono::steady_clock::now();
        std::int64_t diff = (tickEnd - tickBegin).count();

        // If tick took less than 50ms to complete than sleep for rest of time
        if (diff < (1e6 * 50)) {
            std::int64_t sleepTime = (1e6 * 50) - diff;
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleepTime));
        }
    }

    std::cout << "Game thread quit!\n";
}

GameServer::GameServer() {}
GameServer::~GameServer() {}

void GameServer::startServer() {
    networkThreadRunning = true;
    networkThread = std::thread(std::bind(&GameServer::_networkThreadFunc, this));

    gameThreadRunning = true;
    gameThread = std::thread(std::bind(&GameServer::_gameThreadFunc, this));
}

void GameServer::stopServer() {
    networkThreadRunning = false;
    networkThread.join();

    gameThreadRunning = false;
    gameThread.join();
}

void GameServer::addToOutQueue(ENetPeer* peer, ENetPacket* p) {
    outQueueLock.lock();

    outQueue.push_back(IdentifiedPacket{
        .peer = peer,
        .packet = p
    });

    outQueueLock.unlock();
}

void GameServer::addToBroadcastQueue(ENetPacket* p) {
    broadcastQueueLock.lock();

    broadcastQueue.push_back(p);

    broadcastQueueLock.unlock();
}

void GameServer::printPlayerList() {
    connectedPlayersLock.lock();

    for (auto& p : connectedPlayers) {
        printf("Player %x : ", p.first->address.host);

        if (p.second.has_value()) {
            auto& ps = p.second.value();

            std::cout << "XYZ: " << ps.playerPosition.x << ", " << ps.playerPosition.y << ", " << ps.playerPosition.z << "\n"
                      << "CameraPitch: " << ps.cameraPitch << " CameraYaw: " << ps.cameraYaw << "\n";
        } else {
            std::cout << "No player state available!\n";
        }
    }

    connectedPlayersLock.unlock();
}

bool GameServer::messagesAvailable() {
    inQueueLock.lock();
    bool b = inQueue.size() > 0;
    inQueueLock.unlock();
    return b;
}

GameServer::IdentifiedPacket GameServer::popMessage() {
    inQueueLock.lock();

    IdentifiedPacket b = inQueue.back();
    inQueue.pop_back();

    inQueueLock.unlock();

    return b;
}
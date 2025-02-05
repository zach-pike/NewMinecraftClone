#include "GameServer.hpp"

#include "enet.h"
#include "Common/Packets/PacketType.hpp"

#include "Common/Packets/PlayerState/PlayerState.hpp"
#include "Common/Packets/UpdatePlayerState/UpdatePlayerState.hpp"
#include "Common/Packets/ChunkData/ChunkData.hpp"

#include <stdexcept>
#include <functional>
#include <iostream>
#include <cstring>

#define MAX_CLIENTS 32

#define SERVER_RENDER_DISTANCE 4

void GameServer::_networkThreadFunc() {
    Logger networkThreadLogger("NetworkThread", Logger::Color::PURPLE);

    networkThreadLogger.log("Network Thread Started!");

    enet_initialize();

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY; /* Bind the server to the default localhost.     */
    address.port = 7777; /* Bind the server to port 7777. */

    /* create a server */
    ENetHost* server = enet_host_create(&address, MAX_CLIENTS, 1, 0, 0);

    if (server == nullptr) {
        networkThreadLogger.error("Failed to create server!");
        throw std::runtime_error("create server!");
    }

    networkThreadLogger.log("Network server created!");

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
                networkThreadLogger.log("A new client connected!");

                std::shared_ptr<Player> player = std::make_shared<Player>(event.peer);

                connectedPlayersLock.lock();
                connectedPlayers.insert({ event.peer, player });
                connectedPlayersLock.unlock();

                newPlayersLock.lock();
                newPlayers.push_back(player);
                newPlayersLock.unlock();

                playerIDToPlayerMapLock.lock();
                playerIDToPlayerMap.insert({ player->getPlayerUUID(), player });
                playerIDToPlayerMapLock.unlock();
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
                networkThreadLogger.log("User disconnected.");

                disconnectingPlayersLock.lock();
                disconnectingPlayers.push_back(connectedPlayers.at(event.peer));
                disconnectingPlayersLock.unlock();

                connectedPlayersLock.lock();
                connectedPlayers.erase(event.peer);
                connectedPlayersLock.unlock();

                playerIDToPlayerMapLock.lock();
                playerIDToPlayerMap.erase(Player::getPlayerUUIDFromPeer(event.peer));
                playerIDToPlayerMapLock.unlock();
            } break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                networkThreadLogger.log("User timed out.");

                disconnectingPlayersLock.lock();
                disconnectingPlayers.push_back(connectedPlayers.at(event.peer));
                disconnectingPlayersLock.unlock();

                connectedPlayersLock.lock();
                connectedPlayers.erase(event.peer);
                connectedPlayersLock.unlock();

                playerIDToPlayerMapLock.lock();
                playerIDToPlayerMap.erase(Player::getPlayerUUIDFromPeer(event.peer));
                playerIDToPlayerMapLock.unlock();
            } break;

            case ENET_EVENT_TYPE_NONE: break;
        }
    }


    enet_host_destroy(server);
    enet_deinitialize();
    networkThreadLogger.log("Network Thread stopped!");
}

void GameServer::_gameThreadFunc() {
    Logger gameThreadLogger("GameThread", Logger::Color::CYAN);

    gameThreadLogger.log("Game Thread Started!");

    int tickC = 0;

    while(gameThreadRunning) {
        auto tickBegin = std::chrono::steady_clock::now();

        // Handle new players
        newPlayersLock.lock();

        for (auto& p : newPlayers) {
            // Load initial chunks
            auto playerID = p->getPlayerUUID();
            auto center = p->getPlayerState().getChunkCoordinate();
            std::set<ChunkCoordinate> loadedChunks;

            for (int y = -SERVER_RENDER_DISTANCE; y <= SERVER_RENDER_DISTANCE; y++) {
                for (int x = -SERVER_RENDER_DISTANCE; x <= SERVER_RENDER_DISTANCE; x++) {
                    for (int z = -SERVER_RENDER_DISTANCE; z <= SERVER_RENDER_DISTANCE; z++) {
                        ChunkCoordinate chunkPos{ center.x + x, center.y + y, center.z + z };
                        loadedChunks.insert(chunkPos);
                        chunkSubscribers[chunkPos].insert(playerID);

                        auto chunk = world.getChunk(chunkPos);

                        // Add chunk to out queue for player
                        ChunkData cd;
                        cd.chunkCoord = chunkPos;
                        cd.blockData = std::move(chunk->getBlockData());

                        addToOutQueue(p->getPeer(), cd.convToPacket());
                    }
                }
            }

            playerLoadedChunks[playerID] = std::move(loadedChunks);
        }
        
        newPlayers.clear();
        newPlayersLock.unlock();

        // Handle disconnecting players
        disconnectingPlayersLock.lock();

        for (auto& p : disconnectingPlayers) {
            // Erase all chunk subscriptions
            PlayerUUID playerID = p->getPlayerUUID();

            auto& loadedChunks = playerLoadedChunks.at(playerID);

            for (auto& c : loadedChunks) {
                if (!chunkSubscribers.count(c)) continue;

                if (chunkSubscribers.at(c).count(playerID))
                    chunkSubscribers.at(c).erase(playerID);

                if (chunkSubscribers.at(c).empty())
                    chunkSubscribers.erase(c);  // Remove empty sets to free memory
            }

            // Now remove entry from playerLoadedChunks
            playerLoadedChunks.erase(playerID);
        }

        disconnectingPlayers.clear();
        disconnectingPlayersLock.unlock();

        // Do work here
        // Process incoming messages
        inQueueLock.lock();

        for (auto& msgs : inQueue) {
            // Get first byte of packet to determine type

            assert(msgs.packet->dataLength > 0);
            PacketType pt = (PacketType)msgs.packet->data[0];
            PlayerUUID playerID = Player::getPlayerUUIDFromPeer(msgs.peer);

            switch(pt) {
                case PacketType::PlayerState: {
                    PlayerState ps;
                    ps.decodePacket(msgs.packet);


                    connectedPlayersLock.lock();
                    PlayerState& p = connectedPlayers.at(msgs.peer)->getPlayerState();
                    bool updateChunks = p.getChunkCoordinate() != ps.getChunkCoordinate();
                    p = ps;
                    connectedPlayersLock.unlock();

                    // If we have entered new chunks then load them
                    if (updateChunks)
                        updatePlayerChunks(playerID);
                } break;

            }
        }

        inQueue.clear();
        inQueueLock.unlock();


        if (tickC % 2 == 0) {
            // Send out player position data
            connectedPlayersLock.lock();
            for (auto& player : connectedPlayers) {
                UpdatePlayerState ups;
                ups.userToUpdate = player.second->getPlayerUUID();
                ups.playerState = player.second->getPlayerState();
                
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

    gameThreadLogger.log("Game Thread stopped!");
}

void GameServer::updatePlayerChunks(PlayerUUID playerID) {
    auto& trackedChunks = playerLoadedChunks[playerID];

    playerIDToPlayerMapLock.lock();
    auto player = playerIDToPlayerMap.at(playerID);
    playerIDToPlayerMapLock.unlock();

    auto center = player->getPlayerState().getChunkCoordinate();

    std::set<ChunkCoordinate> newChunks;

    // Calculate the new set of visible chunks
    for (int y = -SERVER_RENDER_DISTANCE; y <= SERVER_RENDER_DISTANCE; y++) {
        for (int x = -SERVER_RENDER_DISTANCE; x <= SERVER_RENDER_DISTANCE; x++) {
            for (int z = -SERVER_RENDER_DISTANCE; z <= SERVER_RENDER_DISTANCE; z++) {
                ChunkCoordinate chunkPos{ center.x + x, center.y + y, center.z + z };
                newChunks.insert(chunkPos);

                if (!trackedChunks.count(chunkPos)) {  // Player is seeing this chunk for the first time
                    chunkSubscribers[chunkPos].insert(playerID);
                    
                    auto chunk = world.getChunk(chunkPos);

                    // Add chunk to out queue for player
                    ChunkData cd;
                    cd.chunkCoord = chunkPos;
                    cd.blockData = std::move(chunk->getBlockData());
                    addToOutQueue(player->getPeer(), cd.convToPacket());
                }        
            }
        }
    }

    // Find and remove chunks that are no longer in view
    for (const auto& oldChunk : trackedChunks) {
        if (!newChunks.count(oldChunk)) {  // This chunk is now outside the render distance
            chunkSubscribers[oldChunk].erase(playerID);
            if (chunkSubscribers[oldChunk].empty()) {
                chunkSubscribers.erase(oldChunk);  // Remove empty sets to free memory
            }
        }
    }

    // Update the player's tracked chunks
    playerLoadedChunks[playerID] = std::move(newChunks);
}

GameServer::GameServer():
    logger("GameServer", Logger::Color::GREEN) {}

GameServer::~GameServer() {}

void GameServer::startServer() {
    logger.log("Starting Server...");
    networkThreadRunning = true;
    networkThread = std::thread(std::bind(&GameServer::_networkThreadFunc, this));

    gameThreadRunning = true;
    gameThread = std::thread(std::bind(&GameServer::_gameThreadFunc, this));
    logger.log("Server started!");
}

void GameServer::stopServer() {
    logger.log("Stopping Server...");
    networkThreadRunning = false;
    networkThread.join();

    gameThreadRunning = false;
    gameThread.join();
    logger.log("Server stopped!");
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
        printf("Player %x : ", p.second->getPlayerUUID());

        auto playerState = p.second->getPlayerState();

        std::cout << "XYZ: " << playerState.playerPosition.x << ", " << playerState.playerPosition.y << ", " << playerState.playerPosition.z << "\n"
                      << "CameraPitch: " << playerState.cameraPitch << " CameraYaw: " << playerState.cameraYaw << "\n";
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
#include "GameServer.hpp"

#include "enet.h"
#include "Common/PacketType.hpp"

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

                switch ((PacketType)packet->data[0]) {
                    case PacketType::PlayerState: {
                        PlayerState p;
                        p.decodePacket(packet);

                        connectedPlayers.at(event.peer) = p;
                    } break;

                    default: {
                        enet_packet_destroy(packet);
                    } break;
                }

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

    std::cout << "Server closed!\n";

    enet_host_destroy(server);
    enet_deinitialize();
}

GameServer::GameServer() {}
GameServer::~GameServer() {}

void GameServer::startServer() {
    networkThreadRunning = true;
    networkThread = std::thread(std::bind(&GameServer::_networkThreadFunc, this));
}

void GameServer::stopServer() {
    networkThreadRunning = false;
    networkThread.join();
}

void GameServer::addToOutQueue(ENetPeer* peer, ENetPacket* p) {
    outQueueLock.lock();

    outQueue.push_back(IdentifiedPacket{
        .peer = peer,
        .packet = p
    });

    outQueueLock.unlock();
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
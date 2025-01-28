#include "GameServer.hpp"

#include "enet.h"

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

        int rc = enet_host_service(server, &event, 1000);
        if (rc == 0) continue; // No event in the last second

        if (rc < 0) {
            // Stop server in future
        }

        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                std::cout << "A new client connected!\n";

                const char* serverMessage = "Hello from server!";

                ENetPacket* packet = enet_packet_create(serverMessage, std::strlen(serverMessage), ENET_PACKET_FLAG_RELIABLE);

                if (enet_peer_send(event.peer, 0, packet) != 0) {
                    std::cerr << "Failed to send packet!" << std::endl;
                } else {
                    std::cout << "Packet sent successfully!" << std::endl;
                }

                enet_host_flush(server);

                // /* Store any relevant client information here. */
                // event.peer->data = (void*)"Client information";
            } break;

            case ENET_EVENT_TYPE_RECEIVE:{
                
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
            } break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "User disconnected.\n";
                /* Reset the peer's client information. */
                event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                std::cout << "User timed out.\n";

                /* Reset the peer's client information. */
                // event.peer->data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
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
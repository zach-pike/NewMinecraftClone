#include "GameNetworkClient.hpp"

#include "enet.h"

#include <stdexcept>
#include <iostream>
#include <functional>

void GameNetworkClient::_networkThreadFunc(std::string host, std::uint16_t port) {
    enet_initialize();
    
    // Create client
    ENetHost* client = enet_host_create(NULL, 1, 1, 0, 0);
    
    if (client == NULL) {
        throw std::runtime_error("Failed to create client!");
    }

    // Set address and port
    ENetAddress address = { 0 };
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) {
        throw std::runtime_error("Failed to connect to peer!");
    }

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        workerConnected = true;
    } else {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        networkThreadRunning = false;
        workerConnected = false;

        enet_peer_reset(peer);
        enet_host_destroy(client);
        enet_deinitialize();

        std::cout << "Connection to peer failed.\n";

        return;
    }

    while (networkThreadRunning) {
        int rc = enet_host_service(client, &event, 1000);

        if (rc == 0) continue;

        if (rc < 0) {
            // Disconnect user
            // Error in connection
            std::cout << "Error in connection!\n";

            networkThreadRunning = false;
            workerConnected = false;

            enet_peer_reset(peer);
            enet_host_destroy(client);
            enet_deinitialize();

            return;
        }

        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:{
                ENetPacket* p = event.packet;

                auto a = std::string(p->data, p->data + p->dataLength);

                std::cout << "Message: \"" << a << "\"\n";

                enet_packet_destroy(p);
            } break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                std::cout << "Disconnected!";

                networkThreadRunning = false;
                workerConnected = false;

                enet_peer_reset(peer);
                enet_host_destroy(client);
                enet_deinitialize();

                return;
            } break;
        }
    }

    enet_peer_disconnect(peer, 0);

    uint8_t disconnected = false;

    /* Allow up to 3 seconds for the disconnect to succeed
    * and drop any packets received packets.
    */
    while (enet_host_service(client, &event, 3000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE: {
                enet_packet_destroy(event.packet);
            } break;
            case ENET_EVENT_TYPE_DISCONNECT: {
                std::cout << "Graceful disconnection succeeded.\n";
                disconnected = true;
            } break;
        }
    }

    // Drop connection, since disconnection didn't successed
    if (!disconnected) {
        std::cout << "Ungracefully disconnected unresponsive server\n";
        enet_peer_reset(peer);
    }

    workerConnected = false;
    enet_host_destroy(client);
    enet_deinitialize();
}

GameNetworkClient::GameNetworkClient() {}
GameNetworkClient::~GameNetworkClient() {}

void GameNetworkClient::connectToHost(std::string host, std::uint16_t port) {
    networkThreadRunning = true;
    networkThread = std::thread(std::bind(&GameNetworkClient::_networkThreadFunc, this, host, port));
}

void GameNetworkClient::disconnect() {
    networkThreadRunning = false;
    networkThread.join();
}

bool GameNetworkClient::isConnected() const {
    return workerConnected;
}
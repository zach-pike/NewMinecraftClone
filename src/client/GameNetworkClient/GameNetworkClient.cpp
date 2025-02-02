#include "GameNetworkClient.hpp"

#include <stdexcept>
#include <iostream>
#include <functional>

void GameNetworkClient::_networkThreadFunc(std::string host, std::uint16_t port) {
    Logger networkThreadLogger("NetworkThread", Logger::Color::DARK_CYAN);

    networkThreadLogger.log("Network Thread Started!");

    enet_initialize();
    
    // Create client
    ENetHost* client = enet_host_create(NULL, 1, 1, 0, 0);
    
    if (client == NULL) {
        networkThreadLogger.error("Failed to create client!");
        throw std::runtime_error("create client!");
    }

    // Set address and port
    ENetAddress address = { 0 };
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    ENetPeer* peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) {
        networkThreadLogger.warn("Failed to connect to peer!");

        enet_host_destroy(client);
        enet_deinitialize();

        networkThreadRunning = false;

        networkThreadLogger.log("Network Thread Stopped!");
        return;
    }

    networkThreadLogger.log("ENet Peer Created!");

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

        networkThreadLogger.warn("Failed to connect to peer!");
        networkThreadLogger.log("Network Thread Stopped!");

        return;
    }

    networkThreadLogger.log("Connected!");
    while (networkThreadRunning) {
        int rc = enet_host_service(client, &event, 0);

        if (rc < 0) {
            // Disconnect user
            // Error in connection
            networkThreadLogger.error("Error in connection!");

            networkThreadRunning = false;
            workerConnected = false;

            enet_peer_reset(peer);
            enet_host_destroy(client);
            enet_deinitialize();

            networkThreadLogger.log("Network Thread Stopped!");

            return;
        }

        // Send queued messages
        outQueueLock.lock();
        for (auto& p : outQueue) {
            if (enet_peer_send(peer, 0, p) < 0) {
                enet_packet_destroy(p);
            }
        }
        outQueue.clear();
        outQueueLock.unlock();

        if (rc == 0) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms);
        }

        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:{
                ENetPacket* p = event.packet;

                inQueueLock.lock();
                inQueue.push_back(p);
                inQueueLock.unlock();
            } break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                networkThreadLogger.log("Disconnected!");

                networkThreadRunning = false;
                workerConnected = false;

                enet_peer_reset(peer);
                enet_host_destroy(client);
                enet_deinitialize();

                networkThreadLogger.log("Network Thread Stopped!");

                return;
            } break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                networkThreadLogger.log("Timed out!");

                networkThreadRunning = false;
                workerConnected = false;

                enet_peer_reset(peer);
                enet_host_destroy(client);
                enet_deinitialize();

                networkThreadLogger.log("Network Thread Stopped!");

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
                networkThreadLogger.log("Graceful disconnection succeeded.");
                disconnected = true;
            } break;
        }
    }

    // Drop connection, since disconnection didn't successed
    if (!disconnected) {
        networkThreadLogger.warn("Ungracefully disconnected unresponsive server");
        enet_peer_reset(peer);
    }

    workerConnected = false;
    enet_host_destroy(client);
    enet_deinitialize();

    networkThreadLogger.log("Network Thread Stopped!");
}

GameNetworkClient::GameNetworkClient():
    logger("GameNetworkClient", Logger::Color::DARK_BLUE) {}

GameNetworkClient::~GameNetworkClient() {
    disconnect();
    
    if (networkThread.joinable())
        networkThread.join();
}

void GameNetworkClient::connectToHost(std::string host, std::uint16_t port) {
    if (networkThread.joinable())
        networkThread.join();

    logger.log("Connecting to " + host + ":" + std::to_string(port));

    networkThreadRunning = true;
    networkThread = std::thread(std::bind(&GameNetworkClient::_networkThreadFunc, this, host, port));
}

void GameNetworkClient::disconnect() {
    logger.log("Disconnecting...");
    networkThreadRunning = false;
    // networkThread.join();
}

bool GameNetworkClient::isConnected() const {
    return workerConnected;
}

void GameNetworkClient::addToOutQueue(ENetPacket* p) {
    outQueueLock.lock();
    outQueue.push_back(p);
    outQueueLock.unlock();
}

bool GameNetworkClient::messagesAvailable() {
    inQueueLock.lock();
    bool b = inQueue.size() > 0;
    inQueueLock.unlock();

    return b;
}

ENetPacket* GameNetworkClient::popMessage() {
    inQueueLock.lock();
    ENetPacket* p = inQueue.back();
    inQueue.pop_back();
    inQueueLock.unlock();

    return p;
}
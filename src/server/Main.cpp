#include <bits/stdc++.h>

#include "GameServer/GameServer.hpp"

int main(int argc, char** argv) {
    GameServer gs;

    gs.startServer();

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.find("list") == 0) {
            gs.printPlayerList();
        } else {
            gs.stopServer();
            break;
        }
    }
    

    return 0;
}
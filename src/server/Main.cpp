#include <bits/stdc++.h>

#include "GameServer/GameServer.hpp"

int main(int argc, char** argv) {
    GameServer gs;

    gs.startServer();

    getchar();

    gs.stopServer();

    return 0;
}
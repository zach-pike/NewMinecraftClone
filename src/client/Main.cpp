#include <bits/stdc++.h>

#include "GameClient/GameClient.hpp"

int main(int argc, char** argv) {
    GameClient gc;

    gc.startGame();

    while(true) {
        if (!gc.isRenderThreadRunning()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    gc.stopGame();

    return 0;
}
#include <bits/stdc++.h>

#include "GameServer/GameServer.hpp"

bool is_stdin_ready() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD bytesAvailable = 0;

    if (!hStdin || hStdin == INVALID_HANDLE_VALUE)
        return false;

    if (PeekNamedPipe(hStdin, nullptr, 0, nullptr, &bytesAvailable, nullptr)) {
        return bytesAvailable > 0;
    }
    return false;
}

int main(int argc, char** argv) {
    GameServer gs;

    gs.startServer();

    while (true) {
        gs.printPlayerList();

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

        if (is_stdin_ready()) break;
    }
    gs.stopServer();

    return 0;
}
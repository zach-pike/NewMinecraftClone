#include <bits/stdc++.h>

#include "GameNetworkClient/GameNetworkClient.hpp"

int main(int argc, char** argv) {
    GameNetworkClient gc;

    gc.connectToHost("127.0.0.1", 7777);

    getchar();

    gc.disconnect();

    return 0;
}
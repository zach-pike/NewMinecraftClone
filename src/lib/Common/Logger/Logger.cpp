#include "Logger.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

static std::mutex coutLock;

Logger::Logger(std::string name, Color c):
    loggerName{name},
    color{c} {}

void Logger::log(std::string msg) {
    #ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif

    coutLock.lock();

    #ifdef _WIN32
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    #endif

    printf("[%s] ", loggerName.c_str());

    #ifdef _WIN32
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::WHITE));
    #endif

    printf("[INFO] %s\n", msg.c_str());

    coutLock.unlock();
}

void Logger::warn(std::string msg) {
    #ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif

    coutLock.lock();

    #ifdef _WIN32
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    #endif

    printf("[%s] ", loggerName.c_str());

    #ifdef _WIN32
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::YELLOW));
    #endif

    printf("[WARN] %s\n", msg.c_str());

    coutLock.unlock();
}

void Logger::error(std::string msg) {
    #ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif

    coutLock.lock();

    #ifdef _WIN32
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    #endif

    printf("[%s] ", loggerName.c_str());

    #ifdef _WIN32
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::RED));
    #endif

    printf("[ERROR] %s\n", msg.c_str());

    coutLock.unlock();
}
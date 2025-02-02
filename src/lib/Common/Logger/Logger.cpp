#include "Logger.hpp"

#include <windows.h>

static std::mutex coutLock;

Logger::Logger(std::string name, Color c):
    loggerName{name},
    color{c} {}

void Logger::log(std::string msg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    coutLock.lock();

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    printf("[%s] ", loggerName.c_str());

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::WHITE));
    printf("[INFO] %s\n", msg.c_str());

    coutLock.unlock();
}

void Logger::warn(std::string msg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    coutLock.lock();

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    printf("[%s] ", loggerName.c_str());

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::YELLOW));
    printf("[WARN] %s\n", msg.c_str());

    coutLock.unlock();
}

void Logger::error(std::string msg) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    coutLock.lock();

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    printf("[%s] ", loggerName.c_str());

    SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::RED));
    printf("[ERROR] %s\n", msg.c_str());

    coutLock.unlock();
}
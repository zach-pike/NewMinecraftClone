#pragma once

#include <cstdint>
#include <string>

#include <mutex>

class Logger {
public:
    enum class Color : std::uint8_t {
        DARK_BLUE = 1,
        DARK_GREEN = 2,
        DARK_CYAN = 3,
        DARK_RED = 4,
        DARK_PURPLE =  5,
        DARK_YELLOW = 6,
        DARK_WHITE = 7,
        DARK_GRAY = 8,
        BLUE = 9,
        GREEN = 10,
        CYAN = 11,
        RED = 12,
        PURPLE = 13,
        YELLOW = 14,
        WHITE = 15
    };

private:
    std::string loggerName;
    Color color;

public:
    Logger(std::string _loggerName, Color color);
    ~Logger() = default;

    void log(std::string msg);
    void warn(std::string msg);
    void error(std::string msg);
};
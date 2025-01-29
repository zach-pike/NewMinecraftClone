#pragma once

#include <vector>
#include <cstdint>

class Serializable {
public:
    virtual std::vector<std::uint8_t> serialize() const = 0;
    virtual bool deserialize(const std::vector<std::uint8_t>& data) = 0;
};
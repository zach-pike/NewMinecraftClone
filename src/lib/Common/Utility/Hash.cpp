#include "Hash.hpp"

std::uint64_t hashBytes(const void* data, std::size_t n) {
    constexpr std::uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
    constexpr std::uint64_t FNV_PRIME = 1099511628211ULL;

    std::uint64_t hash = FNV_OFFSET_BASIS;
    const std::uint8_t* bytes = static_cast<const std::uint8_t*>(data);

    for (std::size_t i = 0; i < n; i++) {
        hash ^= bytes[i];
        hash *= FNV_PRIME;
    }

    return hash;
}
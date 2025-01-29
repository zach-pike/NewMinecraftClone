#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

#include "Common/Serialize/Serializable.hpp"

class Serializer {
private:
    std::vector<std::uint8_t> bytes;

public:
    Serializer() {}
    Serializer(const std::vector<std::uint8_t>& arr):
        bytes(arr) {}
    ~Serializer() {}

    void preReserve(std::size_t size) {
        bytes.reserve(size);
    }

    const std::vector<std::uint8_t>& getBytes() {
        return bytes;
    }

    // For basic types
    template <typename T>
    Serializer& operator<<(const T& v) {
        bytes.insert(bytes.end(), (std::uint8_t*)&v, (std::uint8_t*)&v + sizeof(T));
        return *this;
    }

    template <typename T>
    Serializer& operator>>(T& v) {
        // Extract lastmost bytes
        v = *(T*)(bytes.data() + bytes.size() - sizeof(T));
        bytes.erase(bytes.begin() + bytes.size() - sizeof(T), bytes.end());

        return *this;
    }

    // For std::vector
    template <typename T>
    Serializer& operator<<(const std::vector<T>& v) {
        std::uint32_t size = v.size();

        for (std::uint32_t i=0; i < size; i++) *this << v[i];
        *this << size;

        return *this;
    }

    template <typename T>
    Serializer& operator>>(std::vector<T>& v) {
        // Extract lastmost bytes
        std::uint32_t items;
        *this >> items;

        v.reserve(items);

        for (std::uint32_t i=0; i<items; i++) {
            T item;
            *this >> item;
            v.insert(v.begin(), item);
        }

        return *this;
    }

    // For classes that implement Serializable
    Serializer& operator<<(const Serializable* s) {
        // Use std::vector writer
        *this << s->serialize();

        return *this;
    }

    Serializer& operator>>(Serializable* s) {
        std::vector<std::uint8_t> outbytes;
        *this >> outbytes;
        s->deserialize(outbytes);

        return *this;
    }

    // For std::string
    Serializer& operator<<(const std::string& s) {
        // Convert to byte array and push
        *this << std::vector<std::uint8_t>(s.begin(), s.end());

        return *this;
    }

    Serializer& operator>>(std::string& s) {
        std::vector<std::uint8_t> d;
        *this >> d;
        s = std::string(d.begin(), d.end());
        return *this;
    }
};  
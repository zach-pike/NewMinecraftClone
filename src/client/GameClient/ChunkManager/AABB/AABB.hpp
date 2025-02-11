#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <optional>

#include "Common/Types.hpp"

#include "GameClient/ChunkManager/Chunk/Chunk.hpp"

#include "AABBOffsets.hpp"

bool isAABBColliding(glm::vec3 origin1, glm::vec3 origin2, AABBOffsets offset1, AABBOffsets offset2);

struct AABB {
    glm::vec3 origin;
    AABBOffsets offsets;
};

// Broadphase collision detection
std::vector<std::pair<ChunkCoordinate, std::shared_ptr<Chunk>>> getPossibleCollisionChunks(
    glm::vec3 boundingBoxOrigin,
    AABBOffsets offsets,
    const std::map<ChunkCoordinate, std::shared_ptr<Chunk>>& chunks
);

// Narrowphase collision 
std::optional<AABB> getAABBCollidingWithChunks(
    glm::vec3 boundingBoxOrigin,
    AABBOffsets offsets,
    const std::vector<std::pair<ChunkCoordinate, std::shared_ptr<Chunk>>>& chunks
);

// God bless copilot
glm::vec3 resolveAABBCollision(
    glm::vec3 origin1, AABBOffsets offset1,
    glm::vec3 origin2, AABBOffsets offset2,
    glm::vec3 movementDirection
);
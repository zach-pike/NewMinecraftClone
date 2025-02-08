#include "AABB.hpp"

bool isAABBColliding(glm::vec3 origin1, glm::vec3 origin2, AABBOffsets offset1, AABBOffsets offset2) {
    glm::vec3 min1 = origin1 + offset1.minOffset;
    glm::vec3 max1 = origin1 + offset1.maxOffset;

    glm::vec3 min2 = origin2 + offset2.minOffset;
    glm::vec3 max2 = origin2 + offset2.maxOffset;

    // check each axis
    bool xAxisOverlaps = (max1.x >= min2.x) && (min1.x <= max2.x);
    bool yAxisOverlaps = (max1.y >= min2.y) && (min1.y <= max2.y);
    bool zAxisOverlaps = (max1.z >= min2.z) && (min1.z <= max2.z);

    return xAxisOverlaps && yAxisOverlaps && zAxisOverlaps;
}

std::vector<std::pair<ChunkCoordinate, std::shared_ptr<Chunk>>> getPossibleCollisionChunks(
    glm::vec3 boundingBoxOrigin,
    AABBOffsets offsets,
    const std::map<ChunkCoordinate, std::shared_ptr<Chunk>>& chunks
) {
    BlockCoordinate worldspaceCoords{ (int)boundingBoxOrigin.x, (int)boundingBoxOrigin.y, (int)boundingBoxOrigin.z };
    ChunkCoordinate chunkCoordinate = chunkCoordinateFromWorldspaceCoords(worldspaceCoords);

    // Iterate around a 3x3x3 area of chunks where the origin is in the center based around the player
    // and check what chunks are being collided with
    std::vector<std::pair<ChunkCoordinate, std::shared_ptr<Chunk>>> collidingChunks;

    for (int x=-1; x<=1; x++) {
        for (int y=-1; y<=1; y++) {
            for (int z=-1; z<=1; z++) {
                ChunkCoordinate checkChunk { chunkCoordinate.x + x, chunkCoordinate.y + y, chunkCoordinate.z + z };

                // Make a AABB for the chunk
                glm::vec3 chunkAABBOrigin(checkChunk.x * CHUNK_X, checkChunk.y * CHUNK_Y, checkChunk.z * CHUNK_Z);
                AABBOffsets chunkAABBOffsets{ glm::vec3(0, 0, 0), glm::vec3(CHUNK_X, CHUNK_Y, CHUNK_Z) };

                if (
                    isAABBColliding(boundingBoxOrigin, chunkAABBOrigin, offsets, chunkAABBOffsets) &&
                    chunks.count(checkChunk) > 0
                ) {
                    auto chunk = chunks.at(checkChunk);
                    collidingChunks.push_back(std::pair<ChunkCoordinate, std::shared_ptr<Chunk>>(checkChunk, chunk));
                }
            }
        }    
    }

    return collidingChunks;
}

std::vector<AABB> getAABBsCollidingWithChunks(
    glm::vec3 boundingBoxOrigin,
    AABBOffsets offsets,
    const std::vector<std::pair<ChunkCoordinate, std::shared_ptr<Chunk>>>& chunks
) {
    glm::vec3 min = boundingBoxOrigin + offsets.minOffset;
    glm::vec3 max = boundingBoxOrigin + offsets.maxOffset;

    std::vector<AABB> collisions;

    for (auto& chunk : chunks) {
        // Loop over each block in the chunk
        auto chunkCoord = chunk.first;
        for (int cx=0; cx<CHUNK_X; cx++) {
            for (int cy=0; cy<CHUNK_X; cy++) {
                for (int cz=0; cz<CHUNK_X; cz++) {
                    BlockCoordinate chunkBlockCoord{ cx, cy, cz };

                    // Check if block were checking is air
                    if (chunk.second->getChunkBlock(chunkBlockCoord) == 0) continue;

                    // Make AABB for block
                    glm::vec3 blockAABBOrigin(
                        chunkCoord.x * CHUNK_X + cx,
                        chunkCoord.y * CHUNK_Y + cy,
                        chunkCoord.z * CHUNK_Z + cz
                    );
                    AABBOffsets blockAABBOffsets {
                        glm::vec3(0, 0, 0),
                        glm::vec3(1, 1, 1)
                    };

                    // Check for collision
                    if (
                        isAABBColliding(
                            boundingBoxOrigin,
                            blockAABBOrigin,
                            offsets,
                            blockAABBOffsets
                        )
                    ) {
                        collisions.push_back(
                            AABB {
                            .origin = blockAABBOrigin,
                            .offsets = blockAABBOffsets
                            }
                        );
                    }

                }
            }
        }
    }

    return collisions;
}

glm::vec3 resolveAABBCollision(
    glm::vec3 origin1, AABBOffsets offset1,
    glm::vec3 origin2, AABBOffsets offset2,
    glm::vec3 movementDirection
) {
    glm::vec3 min1 = origin1 + offset1.minOffset;
    glm::vec3 max1 = origin1 + offset1.maxOffset;

    glm::vec3 min2 = origin2 + offset2.minOffset;
    glm::vec3 max2 = origin2 + offset2.maxOffset;

    glm::vec3 mtv(0.0f);

    // Calculate overlap on each axis
    float xOverlap = std::min(max1.x, max2.x) - std::max(min1.x, min2.x);
    float yOverlap = std::min(max1.y, max2.y) - std::max(min1.y, min2.y);
    float zOverlap = std::min(max1.z, max2.z) - std::max(min1.z, min2.z);

    // Find the smallest overlap and adjust based on movement direction
    if (xOverlap < yOverlap && xOverlap < zOverlap) {
        mtv.x = (movementDirection.x > 0) ? -xOverlap : xOverlap;
    } else if (yOverlap < xOverlap && yOverlap < zOverlap) {
        mtv.y = (movementDirection.y > 0) ? -yOverlap : yOverlap;
    } else {
        mtv.z = (movementDirection.z > 0) ? -zOverlap : zOverlap;
    }

    return mtv;
}
#include "Block.hpp"


/*
enum class BlockFace : std::uint8_t {
    NegativeX = 0,
    PositiveX = 1,

    NegativeY = 2,
    PositiveY = 3,
    
    NegativeZ = 4,
    PositiveZ = 5,
};

The block face IDs need to be in this order
*/
int blockTextureIDs[][6] = {
    { // Air, never used just here to space out the elements
        0, 0,
        0, 0,
        0, 0
    },
    // Dirt Block ID
    {
        0, 0,
        0, 0,
        0, 0
    },
    { // Grass Block ID
        1, 1,
        0, 2,
        1, 1
    },
    { // Log
        3, 3,
        4, 4,
        3, 3
    },
    {
        5, 5,
        5, 5,
        5, 5
    }
};
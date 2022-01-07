#ifndef __HASHISTATE_H__
#define __HASHISTATE_H__

#include "HashiGrid.hpp"

#define HASHI_MEMORY_WATER 0b00;
#define HASHI_MEMORY_SIMPLE 0b01;
#define HASHI_MEMORY_DOUBLE 0b10;

struct HashiMemoryCell{
    unsigned char island1Bottom : 2;
    unsigned char island1Right : 2;
    unsigned char island2Bottom : 2;
    unsigned char island2Bottom : 2;
}

class HashiState {

    HashiState(HashiGrid* grid);
    Island* GetTopLeftIsland(Island* island1, Island* island2, bool* horizontalBridge);

    // Update functions could be merged, by adding boolean argument for build/destroy
    void UpdateByBuild(Bridge bridge);
    void UpdateByDestroy(Bridge bridge);

    HashiMemoryCell* Memory;
    uint NumberOfCells;
}



#endif
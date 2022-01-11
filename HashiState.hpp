#ifndef __HASHISTATE_H__
#define __HASHISTATE_H__

#include "HashiGrid.hpp"
#include "Island.hpp"

#define HASHI_MEMORY_WATER 0b00
#define HASHI_MEMORY_SIMPLE 0b01
#define HASHI_MEMORY_DOUBLE 0b10

class Island;
class HashiGrid;
struct Bridge;

struct HashiMemoryCell{
    unsigned char island1Bottom : 2;
    unsigned char island1Right : 2;
    unsigned char island2Bottom : 2;
    unsigned char island2Right : 2;

    bool operator==(const HashiMemoryCell& other){
        return island1Bottom == other.island1Bottom
        && island1Right == other.island1Right
        && island2Bottom == other.island2Bottom
        && island2Right == other.island2Right;
    }
};


class HashiState {

public:

    HashiState(HashiGrid* grid);
    ~HashiState();

    // Copy constructor
    HashiState(HashiState* source);

    friend bool operator==(const HashiState& memory1, const HashiState& memory2);

    void PrettyPrint();
    Island* GetTopLeftIsland(Island* island1, Island* island2, bool* horizontalBridge);

    // Update functions could be merged, by adding boolean argument for build/destroy
    void UpdateByBuild(Bridge bridge);
    void UpdateByDestroy(Bridge bridge);

    HashiMemoryCell* Memory;
    uint NumberOfCells;
    
};





#endif
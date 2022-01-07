
#include "HashiState.hpp"


HashiState::HashiState(HashiGrid* hashi){

    // We assume there will never be grid with 0 islands
    uint parity = hashi->NumberOfIslands % 2 == 0 ? 0 : 1;
    NumberOfCells = hashi->NumberOfIslands / 2 + parity;
    Memory = (HashiMemoryCell*) malloc(NumberOfCells);

    for(int i=0; i<NumberOfCells; ++i){
        Memory[i].island1Bottom = HASHI_MEMORY_WATER;
        Memory[i].island1Right = HASHI_MEMORY_WATER;
        Memory[i].island2Bottom = HASHI_MEMORY_WATER;
        Memory[i].island1Right = HASHI_MEMORY_WATER;
    }
}


// Islands have to be either horizontaly aligned or verticaly aligned
Island* HashiState::GetTopLeftIsland(Island* island1, Island* island2, bool* horizontalBridge){

    GridCoords island1Coords = island1->Coords;
    GridCoords island2Coords = island2->Coords;
    *horizontalBridge = island1Coords.i == island2Coords.i;
    if(*horizontalBridge){
        if(island1Coords.i < island2Coords.i) return island1;
        return island2;
    } else {
        if(island1Coords.j < island2Coords.j) return island1;
        return island2;
    }
}


void HashiState::UpdateByBuild(Bridge bridge){

    // We search the most top-left island
    bool horizontalBridge = false;
    Island* islandToUpdate = GetTopLeftIsland(bridge.island1, bridge.island2, &horizontalBridge);
    uint shift = id % 2 == 0;
    if(shift){
        if(horizontalBridge) ++Memory[islandToUpdate->id / 2].island2Right;
        else ++Memory[islandToUpdate->id / 2].island2Bottom;
        
    } else{
        if(horizontalBridge) ++Memory[islandToUpdate->id / 2].island1Right;
        else ++Memory[islandToUpdate->id / 2].island1Bottom;
    }
}

void HashiState::UpdateByDestroy(Bridge bridge){

    // We search the most top-left island
    bool horizontalBridge = false;
    Island* islandToUpdate = GetTopLeftIsland(bridge.island1, bridge.island2, &horizontalBridge);
    uint shift = id % 2 == 0;
    if(shift){
        if(horizontalBridge) --Memory[islandToUpdate->id / 2].island2Right;
        else --Memory[islandToUpdate->id / 2].island2Bottom;
        
    } else{
        if(horizontalBridge) --Memory[islandToUpdate->id / 2].island1Right;
        else --Memory[islandToUpdate->id / 2].island1Bottom;
    }
}
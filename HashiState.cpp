
#include "HashiState.hpp"

#include <iostream>

using namespace std;

bool operator==(const HashiState& memory1, const HashiState& memory2){
        
    bool out;
    for(int i=0; i<memory1.NumberOfCells; ++i){
        out = out && memory1.Memory[i] == memory2.Memory[i];
    }

    return out;
}

HashiState::HashiState(HashiGrid* hashi){

    // We assume there will never be grid with 0 islands
    uint parity = hashi->NumberOfIslands % 2 == 0 ? 0 : 1;
    NumberOfCells = hashi->NumberOfIslands / 2 + parity;
    Memory = (HashiMemoryCell*) malloc(NumberOfCells);

    for(int i=0; i<NumberOfCells; ++i){
        cout << i << endl;
        Memory[i].island1Bottom = HASHI_MEMORY_WATER;
        Memory[i].island1Right = HASHI_MEMORY_WATER;
        Memory[i].island2Bottom = HASHI_MEMORY_WATER;
        Memory[i].island2Right = HASHI_MEMORY_WATER;
    }
}


HashiState::HashiState(HashiState* source){

    NumberOfCells = source->NumberOfCells;
    Memory = (HashiMemoryCell*) malloc(NumberOfCells);

    for(int i=0; i<NumberOfCells; ++i){
        Memory[i] = source->Memory[i];
    }
}


HashiState::~HashiState(){
    free(Memory);
}



void HashiState::PrettyPrint(){

    for(int i=0; i<NumberOfCells; ++i){
        string type[4];
        type[0] = Memory[i].island1Bottom == HASHI_MEMORY_WATER ? "WATER" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_SIMPLE ? "SIMPLE" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_DOUBLE ? "DOUBLE" : "UNKNOWN";
        type[1] = Memory[i].island1Bottom == HASHI_MEMORY_WATER ? "WATER" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_SIMPLE ? "SIMPLE" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_DOUBLE ? "DOUBLE" : "UNKNOWN";
        type[2] = Memory[i].island1Bottom == HASHI_MEMORY_WATER ? "WATER" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_SIMPLE ? "SIMPLE" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_DOUBLE ? "DOUBLE" : "UNKNOWN";
        type[3] = Memory[i].island1Bottom == HASHI_MEMORY_WATER ? "WATER" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_SIMPLE ? "SIMPLE" : 
                    Memory[i].island1Bottom == HASHI_MEMORY_DOUBLE ? "DOUBLE" : "UNKNOWN";

        cout << "Island " << i*2 << " has value bottom : " << type[0] << " and right :" << type[1] << endl;
        cout << "Island " << i*2+1 << " has value bottom : " << type[2] << " and right :" << type[3] << endl;
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
    uint shift = islandToUpdate->ID % 2 == 0;
    if(shift){
        if(horizontalBridge) ++Memory[islandToUpdate->ID / 2].island2Right;
        else ++Memory[islandToUpdate->ID / 2].island2Bottom;
        
    } else{
        if(horizontalBridge) ++Memory[islandToUpdate->ID / 2].island1Right;
        else ++Memory[islandToUpdate->ID / 2].island1Bottom;
    }
}

void HashiState::UpdateByDestroy(Bridge bridge){

    // We search the most top-left island
    bool horizontalBridge = false;
    Island* islandToUpdate = GetTopLeftIsland(bridge.island1, bridge.island2, &horizontalBridge);
    uint shift = islandToUpdate->ID % 2 == 0;
    if(shift){
        if(horizontalBridge) --Memory[islandToUpdate->ID / 2].island2Right;
        else --Memory[islandToUpdate->ID / 2].island2Bottom;
        
    } else{
        if(horizontalBridge) --Memory[islandToUpdate->ID / 2].island1Right;
        else --Memory[islandToUpdate->ID / 2].island1Bottom;
    }
}
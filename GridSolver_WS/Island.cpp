
#include "Island.hpp"


Island::Island(uint population, GridCoords coords, HashiGrid* grid, uint id){

    Population = population;
    BridgeLeft = population;
    Coords = coords;
    Grid = grid;
    ID = id;

}


Island::~Island(){

}


void Island::UpdateReachableIslands(){

    ReachableIslands.clear();
    if(BridgeLeft > 0){
        ReachableIslands = Grid->ReachableIslandsFrom(Coords);
    }
    
}
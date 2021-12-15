
#include "Island.hpp"


Island::Island(uint population, GridCoords coords, HashiGrid* grid, uint id){

    Population = population;
    BridgeLeft = population;
    Coords = coords;
    Grid = grid;
    ID = id;

}


void Island::UpdateReachableIslands(){

    ReachableIslands = Grid->ReachableIslandsFrom(Coords);
}
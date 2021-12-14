
#include "Island.hpp"


Island::Island(uint population, GridCoords coords, HashiGrid* grid){

    Population = population;
    BridgeLeft = population;
    Coords = coords;
    Grid = grid;

}


void Island::UpdateReachableIslands(){

    ReachableIslands = Grid->ReachableIslandsFrom(Coords);
}
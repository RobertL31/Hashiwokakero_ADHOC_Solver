#ifndef __ISLAND_H__
#define __ISLAND_H__

#include "HashiGrid.hpp"
#include "json.hpp"

class HashiGrid;

struct GridCoords {
    uint i;
    uint j;
};


class Island {

public:

    Island(uint population, GridCoords coords, HashiGrid* grid, uint id);
    ~Island();

    void UpdateReachableIslands();
    void RemoveFromReachable(uint islandID);


private:

    uint Population;
    HashiGrid* Grid;

public: 

    uint ID;
    GridCoords Coords;
    uint BridgeLeft;
    std::vector<Island*> ReachableIslands;

};






#endif
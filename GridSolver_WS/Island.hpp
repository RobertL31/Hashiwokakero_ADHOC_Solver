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

    Island(uint population, GridCoords coords, HashiGrid* grid);
    ~Island();

    void UpdateReachableIslands();
    void RemoveFromReachable(uint islandID);


private:

    uint Population;
    uint BridgeLeft;
    GridCoords Coords;
    HashiGrid* Grid;

public: 

    std::vector<uint> ReachableIslands;

};






#endif
#ifndef __ISLAND_H__
#define __ISLAND_H__

#include "HashiGrid.hpp"
#include "json.hpp"

struct GridCoords {
    uint i;
    uint j;
};


class Island {

public:

    Island(uint population, GridCoords coords);
    ~Island();

    void UpdateReachableIslands(HashiGrid* hashiGrid);


private:

    uint Population;
    uint BridgeLeft;
    GridCoords Coords;
    std::vector<uint> ReachableIslands;

};






#endif
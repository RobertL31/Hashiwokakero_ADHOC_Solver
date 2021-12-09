#ifndef __ISLAND_H__
#define __ISLAND_H__

#include "json.hpp"

struct GridCoords {
    uint i;
    uint j;
};


class Island {

public:

    Island(uint population, GridCoords coords);
    ~Island();


private:

    uint Population;
    uint BridgeLeft;
    GridCoords Coords;
    std::vector<GridCoords> ReachableIslands;

};






#endif
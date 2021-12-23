#ifndef __ISLAND_H__
#define __ISLAND_H__

#include "HashiGrid.hpp"

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

    HashiGrid* Grid;

public: 

    uint Population;
    uint ID;
    GridCoords Coords;
    uint BridgeLeft;
    std::vector<Island*> ReachableIslands;

};


struct Bridge {
    Island* island1;
    Island* island2;
    uint depth;

    friend bool operator==(const Bridge& b1, const Bridge& b2){
        return b1.island1 == b2.island1 && b1.island2 == b2.island2;
    }

    friend bool operator<(const Bridge& b1, const Bridge& b2){
        return b1.island1->ID < b2.island1->ID || b1.island2->ID < b2.island2->ID;
    }
};






#endif
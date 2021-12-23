#ifndef __HASHIGRID_H__
#define __HASHIGRID_H__

#include "json.hpp"
#include "Island.hpp"

#include <string>
#include <stack>

struct Bridge;
class Island;
struct GridCoords;


class HashiGrid {

public:

    // Defines the tile type for non-island tiles
    enum TileType {WEST = -1, DWEST = -2, NORTH = -3, DNORTH = -4, WATER = -10};

    HashiGrid(const nlohmann::json& jsonGrid);
    HashiGrid(const std::string& filename);
    ~HashiGrid();

    bool Solve(uint depth);
    void Backtrack(uint depth);
    void Build(Bridge bridge);
    void DestroyLast();

    std::vector<Bridge> GetBuildableBridges(uint depth);
    std::vector<Island*> ReachableIslandsFrom(GridCoords coords);
    bool AskForValidation();
    bool SelfValidate();
    void BuildSolution(nlohmann::json& outJson);

    void PrettyPrint(std::ostream& stream) const;

    friend std::ostream & operator<<(std::ostream& stream, const HashiGrid& hashiGrid) { 
        hashiGrid.PrettyPrint(stream);
        return stream;
    }

private:

    // 1 dimension for efficiency purpose
    int* Grid; 
    uint N;
    uint M;
    std::vector<Bridge> BacktrackStack;
    Island** Islands;
    uint NumberOfIslands;

};






#endif
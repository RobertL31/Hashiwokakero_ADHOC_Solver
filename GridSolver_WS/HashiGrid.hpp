#ifndef __HASHIGRID_H__
#define __HASHIGRID_H__

#include "json.hpp"
#include "Island.hpp"

#include <string>
#include <stack>


class Island;
struct GridCoords;

struct Bridge {
    Island* island1;
    Island* island2;
    uint depth;
};


class HashiGrid {

public:

    enum BridgeDirection {WEST = -1, DWEST = -2, NORTH = -3, DNORTH = -4};

    HashiGrid(const nlohmann::json& jsonGrid);
    HashiGrid(const std::string& filename);
    ~HashiGrid();

    bool Solve(uint depth);
    void Backtrack(uint depth);
    void Build(Bridge bridge);
    void Destroy(Bridge bridge);

    std::vector<Bridge> GetBuildableBridges();
    void RemoveBidirectionDuplicates(std::vector<Bridge>& bridges);
    std::vector<Island*> ReachableIslandsFrom(GridCoords coords);
    bool AskForValidation();
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
    std::vector<Bridge> ExplorationStack;
    uint ActualDepth;
    Island** Islands;
    uint NumberOfIslands;

};






#endif
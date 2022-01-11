#ifndef __HASHIGRID_H__
#define __HASHIGRID_H__

#include "json.hpp"
#include "Island.hpp"
#include "HashiState.hpp"

#include <string>
#include <stack>
#include <unordered_set>

struct Bridge;
class Island;
struct GridCoords;
class HashiState;

bool operator==(const HashiState& memory1, const HashiState& memory2);

struct HashiStateEqual{
    bool operator () ( HashiState const * lhs, HashiState const * rhs ) const {
        return *lhs == *rhs;
    }
};


class HashiGrid {

public:

    // Defines the tile type for non-island tiles
    enum TileType {WEST = -1, DWEST = -2, NORTH = -3, DNORTH = -4, WATER = -10};

    HashiGrid(const nlohmann::json& jsonGrid);
    HashiGrid(const std::string& filename);
    ~HashiGrid();

    void CreateAdaptedGrid(const nlohmann::json& jsonGrid);

    bool Solve(uint depth);
    void Backtrack(uint depth);
    void Build(Bridge bridge);
    void DestroyLast();
    void StoreCurrentState();

    std::vector<Bridge> GetBuildableBridges(uint depth);
    std::vector<Island*> ReachableIslandsFrom(Island* island);
    bool AskForValidation();
    bool SelfValidate();
    void BuildSolution(nlohmann::json& outJson);

    bool Simplify(uint depth);
    bool JustEnoughNeighbors(uint depth);
    bool OnlyFewNeighbors(uint depth);

    void PrettyPrint(std::ostream& stream) const;

    friend std::ostream & operator<<(std::ostream& stream, const HashiGrid& hashiGrid) { 
        hashiGrid.PrettyPrint(stream);
        return stream;
    }

public:

    uint NumberOfIslands;

private:

    // 1 dimension for efficiency purpose
    int* Grid; 
    uint N;
    uint M;
    std::vector<Bridge> BacktrackStack;
    Island** Islands;

    std::unordered_set<HashiState*, std::hash<HashiState*>, HashiStateEqual> VisitedStates;
    HashiState* CurrentState;

};






#endif
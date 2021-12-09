#ifndef __HASHIGRID_H__
#define __HASHIGRID_H__

#include "json.hpp"
#include "Island.hpp"

#include <string>
#include <stack>


struct Bridge {
    Island* island1;
    Island* island2;
    uint depth;
};


class HashiGrid {

public:

    HashiGrid(const nlohmann::json& jsonGrid);
    HashiGrid(const string& filename);
    ~HashiGrid();

    void Build(Bridge b);
    void Destroy(Bridge b);
    void Backtrack();
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
    stack<Bridge> BacktrackStack;
    uint ActualDepth;
    Island** Islands;

};






#endif
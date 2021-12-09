#ifndef __HASHIGRID_H__
#define __HASHIGRID_H__

#include "json.hpp"


class HashiGrid {

public:

    HashiGrid(const nlohmann::json& jsonGrid);
    ~HashiGrid();


    void PrettyPrint(std::ostream& stream) const;

    friend std::ostream & operator<<(std::ostream& stream, const HashiGrid& hashiGrid) { 
        hashiGrid.PrettyPrint(stream);
        return stream;
    }

private:

    // 1 dimension for efficiency purpose
    int* m_grid; 
    short m_n;
    short m_m;



};






#endif
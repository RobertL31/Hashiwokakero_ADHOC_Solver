#ifndef __HASHIGRID_H__
#define __HASHIGRID_H__

#include "json.hpp"


class HashiGrid {

public:

    HashiGrid(const nlohmann::json& jsonGrid);
    ~HashiGrid();


    void HashiGrid::PrettyPrint(std::ostream& stream) const;

private:

    // 1 dimension for efficiency purpose
    char* m_grid; 
    short m_n;
    short m_m;


};




#endif
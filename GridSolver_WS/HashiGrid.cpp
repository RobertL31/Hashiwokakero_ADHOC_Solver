
#include "HashiGrid.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

HashiGrid::HashiGrid(const json& jsonGrid){

    m_n = jsonGrid["row_number"];
    m_m = jsonGrid["col_number"];
    m_grid = new int[m_n * m_m];

    json gridDescription = jsonGrid["description"];
    int i = 0;
    int j = 0;

    for(auto& line : gridDescription){
        for(auto& tile : line){
            m_grid[i*m_m +j] = tile.get<int>();
            ++j;
        }
        ++i;
        j=0;
    }
}

HashiGrid::~HashiGrid(){

    free(m_grid);
}


void HashiGrid::PrettyPrint(std::ostream& stream) const{

    for(int i=0; i<m_n; ++i) stream << "#";
    stream << '\n';

    for(int i=0; i<m_n; ++i){
        for(int j=0; j<m_m; ++j){

            int elmt = m_grid[i*m_m + j];

            switch(elmt){
                case -1:
                    stream << "―";
                    break;
                case -2:
                    stream << "═";
                    break;
                case -3:
                    stream << "|";
                    break;
                case -4:
                    stream << "ǁ";
                    break;

                default:
                    stream << elmt;
            }
        }
        stream << endl;
    }

    for(int i=0; i<m_n; ++i) stream << "#";
    
    stream << '\n';
}
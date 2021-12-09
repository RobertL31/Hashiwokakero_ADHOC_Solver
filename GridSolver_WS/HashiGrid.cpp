
#include "HashiGrid.hpp"
#include <iostream>

using namespace std;
using json = nlohmann::json;

HashiGrid::HashiGrid(const json& jsonGrid){

    m_n = jsonGrid["rowNumber"];
    m_m = jsonGrid["colNumber"];
    m_grid = new char[m_n * m_m];

}

HashiGrid::~HashiGrid(){

    free(m_grid);
}


std::ostream & operator<<(std::ostream& stream, HashiGrid const& hashiGrid) { 
    hashiGrid.PrettyPrint(stream);
    return stream;
}


void HashiGrid::PrettyPrint(std::ostream& stream) const{

    for(int i=0; i<m_n; ++i) stream << "#";
    stream << '\n';

    for(int i=0; i<m_n; ++i){
        for(int j=0; j<m_m; ++j){

            char elmt = m_grid[i*m_m + j];

            switch(m_grid[i*m_m + j]){
                case -1:
                    stream << 'u\2015';
                    break;
                case -2:
                    stream << 'u\2550';
                    break;
                case -3:
                    stream << 'u\007C';
                    break;
                case -4:
                    stream << 'u\01C0';
                    break;

                default:
                    //TODO effective print
                    stream << int(elmt);
            }
        }
        stream << endl;
    }

    for(int i=0; i<m_n; ++i) stream << "#";
    
    stream << '\n';
}
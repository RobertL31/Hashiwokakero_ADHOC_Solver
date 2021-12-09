
#include "HashiGrid.hpp"

#include <iostream>



using namespace std;
using json = nlohmann::json;



HashiGrid::HashiGrid(const json& jsonGrid){

    N = jsonGrid["row_number"];
    M = jsonGrid["col_number"];
    Grid = new int[N * M];
    BacktrackStack = new stack<Bridge>();
    ActualDepth = 0;

    json gridDescription = jsonGrid["description"];
    uint i = 0;
    uint j = 0;

    for(auto& line : gridDescription){
        for(auto& tile : line){
            Grid[i*M +j] = tile.get<int>();
            ++j;
        }
        ++i;
        j=0;
    }


    json islands = jsonGrid["islands"];
    Islands = new Island*[islands.size()];
    i = 0;
    for(auto& island : islands){
        uint population = island["population"].get<uint>();
        GridCoords coords = {.i = island["coordinates"]["i"], .j= island["coordinates"]["j"]};
        Islands[i] = new Island(population, coords);
    }

}


HashiGrid::HashiGrid(const string& filename){

    //TODO

}

HashiGrid::~HashiGrid(){

    free(Grid);
}



void HashiGrid::Build(Bridge b){

}



void HashiGrid::PrettyPrint(std::ostream& stream) const{

    for(int i=0; i<N; ++i) stream << "#";
    stream << '\n';

    for(int i=0; i<N; ++i){
        for(int j=0; j<M; ++j){

            int elmt = Grid[i*M + j];

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

    for(int i=0; i<N; ++i) stream << "#";
    
    stream << '\n';
}
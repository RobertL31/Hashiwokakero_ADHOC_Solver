
#include "HashiGrid.hpp"

#include <iostream>


using namespace std;
using json = nlohmann::json;



HashiGrid::HashiGrid(const json& jsonGrid){

    N = jsonGrid["row_number"];
    M = jsonGrid["col_number"];
    Grid = new int[N * M];
    BacktrackStack = stack<Bridge>();
    ActualDepth = 0;

    json gridDescription = jsonGrid["description"];
    uint lineIndex = 0;
    uint colIndex = 0;

    for(auto& line : gridDescription){
        for(auto& tile : line){
            Grid[lineIndex*M + colIndex] = tile.get<int>();
            ++colIndex;
        }
        ++lineIndex;
        colIndex=0;
    }


    json islands = jsonGrid["islands"];
    Islands = new Island*[islands.size()];
    NumberOfIslands = 0;
    for(auto& island : islands){
        uint population = island["population"].get<uint>();
        GridCoords coords = {.i = island["coordinates"]["i"], .j= island["coordinates"]["j"]};
        Islands[NumberOfIslands] = new Island(population, coords, this);
        ++NumberOfIslands;
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



bool HashiGrid::Solve(uint depth){

    for(int i=0; i<NumberOfIslands; ++i){
        Islands[i]->UpdateReachableIslands();
        cout << "Island " << i << ": ";
        for(int j=0; j<Islands[i]->ReachableIslands.size(); ++j){
            cout << Islands[i]->ReachableIslands[j];
        }
        cout << endl;
    }

    return false;

}



std::vector<uint> HashiGrid::ReachableIslandsFrom(GridCoords coords){

    std::vector<uint> result;

    // Explore North
    bool twoPossible = true;
    for(int i=coords.i-1; i>=0; --i){
        int elmt = Grid[i * M + coords.j];
        if(elmt == NORTH) twoPossible = false;
        else {
            if(elmt >= 0){
                result.push_back(elmt);
                if(twoPossible) result.push_back(elmt);
            }
            // If the bridget type is different or we found island
            break;
        }
    }


    // Explore South
    twoPossible = true;
    for(int i=coords.i+1; i<N; ++i){
        int elmt = Grid[i * M + coords.j];
        if(elmt == NORTH) twoPossible = false;
        else {
            if(elmt >= 0){
                result.push_back(elmt);
                if(twoPossible) result.push_back(elmt);
            }
            // If the bridget type is different or we found island
            break;
        }
    }


    // Explore East
    for(int j=coords.j-1; j>=0; --j){
        int elmt = Grid[coords.i * M + j];
        if(elmt == WEST) twoPossible = false;
        else {
            if(elmt >= 0){
                result.push_back(elmt);
                if(twoPossible) result.push_back(elmt);
            }
            // If the bridget type is different or we found island
            break;
        }
    }

    
    // Explore West
    for(int j=coords.j+1; j<M; ++j){
        int elmt = Grid[coords.i * M + j];
        if(elmt == WEST) twoPossible = false;
        else {
            if(elmt >= 0){
                result.push_back(elmt);
                if(twoPossible) result.push_back(elmt);
            }
            // If the bridget type is different or we found island
            break;
        }
    }

    return result;
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
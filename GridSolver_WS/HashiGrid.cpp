
#include "HashiGrid.hpp"


#include <pistache/client.h>
#include <pistache/http.h>
#include <pistache/net.h>


#include <iostream>


using namespace std;
using json = nlohmann::json;
using namespace Pistache;
using namespace Pistache::Http;

const string serverIP = "127.0.0.1:";
const string serverPort = "50500";


HashiGrid::HashiGrid(const json& jsonGrid){

    N = jsonGrid["row_number"];
    M = jsonGrid["col_number"];
    Grid = new int[N * M];
    BacktrackStack = vector<Bridge>();
    ExplorationStack = vector<Bridge>();
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
        Islands[NumberOfIslands] = new Island(population, coords, this, NumberOfIslands);
        ++NumberOfIslands;
    }

}


HashiGrid::HashiGrid(const string& filename){

    //TODO

}

HashiGrid::~HashiGrid(){

    free(Grid);
}



void HashiGrid::Build(Bridge bridge){

    // Add this Bridge to the stack
    BacktrackStack.push_back(bridge);
    GridCoords island1 = bridge.island1->Coords;
    GridCoords island2 = bridge.island2->Coords;

    // Modify the internal representation
    int bridgeType;
    if(island1.i > island2.i){
        // Build north bridge
        if(Grid[(island1.i + 1) * M + island1.j] != NORTH) bridgeType = NORTH;
        else bridgeType = DNORTH;

        for(int i=island1.i-1; i>island2.i; --i){
            Grid[i*M + island1.j] = NORTH;
        }
    } else {
        if(island1.i < island2.i){
            // Build south bridge
            if(Grid[(island1.i - 1) * M + island1.j] != NORTH) bridgeType = NORTH;
            else bridgeType = DNORTH;

            for(int i=island1.i+1; i<island2.i; ++i){
                Grid[i*M + island1.j] = NORTH;
            }
        } else {
            if(island1.j > island2.j){
                // Build west bridge
                if(Grid[island1.i * M + (island1.j - 1)] != WEST) bridgeType = WEST;
                else bridgeType = DWEST;

                for(int j=island1.j-1; j>island2.j; --j){
                    Grid[island1.i*M + j] = WEST;
                }
            } else {
                if(island1.j < island2.j){
                    // Build west bridge
                    if(Grid[island1.i * M + (island1.j + 1)] != WEST) bridgeType = WEST;
                    else bridgeType = DWEST;

                    for(int j=island1.j+1; j>island2.j; ++j){
                        Grid[island1.i*M + j] = WEST; 
                    }
                }
            }
        }
    }
}


void HashiGrid::Backtrack(uint depth){

}


void HashiGrid::Destroy(Bridge bridge){
    
}


bool HashiGrid::Solve(uint depth){


    vector<Bridge> buildableBridges;
    // Computes possible moves
    buildableBridges = GetBuildableBridges();
    
    // We are on a leaf
    if(buildableBridges.size() == 0){
        return AskForValidation();
    }

    // We can still explore
    for(Bridge toBuild : buildableBridges){
        
        Build(toBuild);
        bool isSolved = Solve(depth+1);

        // Subtree has a solution
        if(isSolved){
            return true;
        } 

        // Otherwise, undo the move and try another
        Backtrack(depth);
    }
        
    // If none of the moves lead to a solution, return false to backtrack higher
    return false;
}


vector<Bridge> HashiGrid::GetBuildableBridges(){

    vector<Bridge> buildableBridges;
    for(int i=0; i<NumberOfIslands; ++i){
        Islands[i]->UpdateReachableIslands();
        for(int j=0; j<Islands[i]->ReachableIslands.size(); ++j){
            Bridge b = {.island1 = Islands[i], .island2 = Islands[i]->ReachableIslands[j], .depth = ActualDepth};
            Bridge b_bar = {.island1 = Islands[i]->ReachableIslands[j], .island2 = Islands[i], .depth = ActualDepth};
            // If the opposite bridge is already added
            if( find(buildableBridges.begin(), buildableBridges.end(), b_bar) == buildableBridges.end() ){
                buildableBridges.push_back(b);
            }
            
        }
    }

    return buildableBridges;
}


std::vector<Island*> HashiGrid::ReachableIslandsFrom(GridCoords coords){

    std::vector<Island*> result;

    // Explore North
    bool twoPossible = true;
    for(int i=coords.i-1; i>=0; --i){
        int elmt = Grid[i * M + coords.j];
        if(elmt == NORTH) twoPossible = false;
        else {
            if(elmt >= 0){
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
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
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
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
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
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
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
            }
            // If the bridget type is different or we found island
            break;
        }
    }

    return result;
}

//TODO
bool HashiGrid::AskForValidation(){

    json solution;
    BuildSolution(solution);
   
    const std::string server_address =  serverIP + serverPort + "/";
    std::cout << "Server address: " << server_address << "\n";

    Http::Experimental::Client client;
    auto opts = Http::Experimental::Client::options().maxResponseSize(4096);
    client.init(opts);

    //auto response = client.get(server_address).send();
    auto response = client.post(server_address).body(solution.dump()).send();
    Async::Barrier<Http::Response> barrier(response);
    barrier.wait_for(std::chrono::seconds(2));
    client.shutdown();

    return false;
}


void HashiGrid::BuildSolution(json& outJson){

    for(auto it=BacktrackStack.begin(); it != BacktrackStack.end(); ++it){
        vector<uint> tmpVec;
        tmpVec.push_back(it->island1->ID);
        tmpVec.push_back(it->island2->ID);
        outJson["connections"].push_back(tmpVec);
    }
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
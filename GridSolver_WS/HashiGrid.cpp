
#include "HashiGrid.hpp"


#include <pistache/client.h>
#include <pistache/http.h>
#include <pistache/net.h>

#include <set>
#include <unordered_set>
#include <queue>
#include <iostream>


using namespace std;
using json = nlohmann::json;
using namespace Pistache;
using namespace Pistache::Http;

const string serverIP = "127.0.0.1:";
const string serverPort = "50500";


HashiGrid::HashiGrid(const json& jsonGrid){

    N = jsonGrid["dimension"];
    M = N;
    Grid = new int[N * M];
    BacktrackStack = vector<Bridge>();

    json description = jsonGrid["description"];
    // TODO guard description.size() == N*M
    for(auto& tileValue : description){
        if(tileValue.get<int>() > 0) ++NumberOfIslands;
    }
    Islands = new Island*[NumberOfIslands];
    int i = 0;
    NumberOfIslands = 0;
    for(auto& tileValue : description){
        uint population = tileValue.get<int>();
        if(population > 0){
            Grid[i] = NumberOfIslands;
            GridCoords coords = {.i = i/M, .j= i%M};
            Islands[NumberOfIslands] = new Island(population, coords, this, NumberOfIslands);
            ++NumberOfIslands;
        } else Grid[i] = WATER;

        ++i;
    }
}


HashiGrid::HashiGrid(const string& filename){

    //TODO

}

HashiGrid::~HashiGrid(){

    delete Grid;
    for(int i=0; i<NumberOfIslands; ++i){
        delete Islands[i];
    }
    delete[] Islands;
}



void HashiGrid::Build(Bridge bridge){

    // Add this Bridge to the stack
    BacktrackStack.push_back(bridge);

    // Update islands informations
    bridge.island1->BridgeLeft--;
    bridge.island2->BridgeLeft--;

    // Modify the internal representation
    GridCoords island1 = bridge.island1->Coords;
    GridCoords island2 = bridge.island2->Coords;
    int bridgeType;
    if(island1.i > island2.i){
        // Build north bridge
        if(Grid[(island1.i - 1) * M + island1.j] == NORTH) bridgeType = DNORTH;
        else bridgeType = NORTH;

        for(int i=island1.i-1; i>island2.i; --i){
            Grid[i*M + island1.j] = bridgeType;
        }
    } else {
        if(island1.i < island2.i){
            // Build south bridge
            if(Grid[(island1.i + 1) * M + island1.j] == NORTH) bridgeType = DNORTH;
            else bridgeType = NORTH;

            for(int i=island1.i+1; i<island2.i; ++i){
                Grid[i*M + island1.j] = bridgeType;
            }
        } else {
            if(island1.j > island2.j){
                // Build west bridge
                if(Grid[island1.i * M + (island1.j - 1)] == WEST) bridgeType = DWEST;
                else bridgeType = WEST;

                for(int j=island1.j-1; j>island2.j; --j){
                    Grid[island1.i*M + j] = bridgeType;
                }
            } else {
                if(island1.j < island2.j){
                    // Build west bridge
                    if(Grid[island1.i * M + (island1.j + 1)] == WEST) bridgeType = DWEST;
                    else bridgeType = WEST;

                    for(int j=island1.j+1; j<island2.j; ++j){
                        Grid[island1.i*M + j] = bridgeType; 
                    }
                }
            }
        }
    }

    #ifdef HASHI_VERBOSE
    cout << "Built bridge from " << bridge.island1->ID << " to " << bridge.island2->ID << " at depth " << bridge.depth << endl; 
    #endif
}


void HashiGrid::Backtrack(uint depth){

    #ifdef HASHI_VERBOSE
    cout << "Backtracking to a depth of " << depth << endl;
    #endif

    while(BacktrackStack.back().depth == depth){
        DestroyLast();
    }
}


void HashiGrid::DestroyLast(){

    // Remove this Bridge from the stack
    Bridge bridge = BacktrackStack.back();
    BacktrackStack.pop_back();

    // Update islands informations
    bridge.island1->BridgeLeft++;
    bridge.island2->BridgeLeft++;
    
    // Modify the internal representation
    GridCoords island1 = bridge.island1->Coords;
    GridCoords island2 = bridge.island2->Coords;
    int bridgeType;
    if(island1.i > island2.i){
        // Build north bridge
        if(Grid[(island1.i - 1) * M + island1.j] == DNORTH) bridgeType = NORTH;
        else bridgeType = WATER;

        for(int i=island1.i-1; i>island2.i; --i){
            Grid[i*M + island1.j] = bridgeType;
        }
    } else {
        if(island1.i < island2.i){
            // Build south bridge
            if(Grid[(island1.i + 1) * M + island1.j] == DNORTH) bridgeType = NORTH;
            else bridgeType = WATER;

            for(int i=island1.i+1; i<island2.i; ++i){
                Grid[i*M + island1.j] = bridgeType;
            }
        } else {
            if(island1.j > island2.j){
                // Build west bridge
                if(Grid[island1.i * M + (island1.j - 1)] == DWEST) bridgeType = WEST;
                else bridgeType = WATER;

                for(int j=island1.j-1; j>island2.j; --j){
                    Grid[island1.i*M + j] = bridgeType;
                }
            } else {
                if(island1.j < island2.j){
                    // Build west bridge
                    if(Grid[island1.i * M + (island1.j + 1)] == DWEST) bridgeType = WEST;
                    else bridgeType = WATER;

                    for(int j=island1.j+1; j<island2.j; ++j){
                        Grid[island1.i*M + j] = bridgeType; 
                    }
                }
            }
        }
    }

    #ifdef HASHI_VERBOSE
    cout << "Removed bridge from " << bridge.island1->ID << " to " << bridge.island2->ID << " at depth " << bridge.depth << endl; 
    #endif

}

long nodes = 0;
long leafs = 0;

bool HashiGrid::Solve(uint depth){

    ++nodes;
    vector<Bridge> buildableBridges;
    // Computes possible moves
    buildableBridges = GetBuildableBridges(depth);
    
    #ifdef HASHI_VERBOSE
    cout << "depth : " << depth << "\n possible bridges :" << buildableBridges.size() << endl;
    cout << *this << endl;
    for(Bridge b : buildableBridges){
        cout << "from " << b.island1->ID << " to " << b.island2->ID << " with depth " << b.depth << endl;
    }
    #endif

    // We are on a leaf
    if(buildableBridges.size() == 0){
        return SelfValidate();
    }

    // Transform into list of unique bridges
    set<Bridge> uniqueBuildableBridges(buildableBridges.begin(), buildableBridges.end());
    // We can still explore
    for(Bridge toBuild : uniqueBuildableBridges){
        
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


vector<Bridge> HashiGrid::GetBuildableBridges(uint depth){

    vector<Bridge> buildableBridges;
    for(int i=0; i<NumberOfIslands; ++i){
        Islands[i]->UpdateReachableIslands();
        for(int j=0; j<Islands[i]->ReachableIslands.size(); ++j){
            Bridge b = {.island1 = Islands[i], .island2 = Islands[i]->ReachableIslands[j], .depth = depth};
            Bridge b_bar = {.island1 = Islands[i]->ReachableIslands[j], .island2 = Islands[i], .depth = depth};
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
        else if(elmt == DNORTH) break;
        else {
            if(elmt >= 0 && Islands[elmt]->BridgeLeft > 0){
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
                break;
            } 
        }
    }


    // Explore South
    twoPossible = true;
    for(int i=coords.i+1; i<N; ++i){
        int elmt = Grid[i * M + coords.j];
        if(elmt == NORTH) twoPossible = false;
        else if(elmt == DNORTH) break;
        else {
            if(elmt >= 0 && Islands[elmt]->BridgeLeft > 0){
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
                break;
            }
        }
    }


    // Explore West
    for(int j=coords.j-1; j>=0; --j){
        int elmt = Grid[coords.i * M + j];
        if(elmt == WEST) twoPossible = false;
        else if(elmt == DWEST) break;
        else {
            if(elmt >= 0 && Islands[elmt]->BridgeLeft > 0){
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
                break;
            }
        }
    }

    
    // Explore East
    for(int j=coords.j+1; j<M; ++j){
        int elmt = Grid[coords.i * M + j];
        if(elmt == WEST) twoPossible = false;
        else if(elmt == DWEST) break;
        else {
            if(elmt >= 0 && Islands[elmt]->BridgeLeft > 0){
                result.push_back(Islands[elmt]);
                if(twoPossible) result.push_back(Islands[elmt]);
                break;
            }
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


bool HashiGrid::SelfValidate(){

    ++leafs;

    struct Node_t{
        vector<int> links;
        bool marked;
    };

    Node_t nodeArray[NumberOfIslands];
    for(Bridge bridge : BacktrackStack){
        nodeArray[bridge.island1->ID].links.push_back(bridge.island2->ID);
        nodeArray[bridge.island2->ID].links.push_back(bridge.island1->ID);
    }

    bool check = true;
    // Check if every island has enough connections
    for(int i=0; i<NumberOfIslands; ++i){
        check = check && nodeArray[i].links.size() == Islands[i]->Population;
        if(!check){
            return false;
        }
    }


    // Check if all island can reach others
    int reached = 0;
    queue<Node_t*> toCheck;
    toCheck.push(&nodeArray[0]);

    while(toCheck.size() != 0){
        Node_t* actual = toCheck.front();
        if( !actual->marked){
            for(const int& next : actual->links){
                toCheck.push(&nodeArray[next]);
            }
            actual->marked = true;
            ++reached;
        }
        toCheck.pop();
    }

    return check && reached == NumberOfIslands;
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
                case WEST:
                    stream << "―";
                    break;
                case DWEST:
                    stream << "═";
                    break;
                case NORTH:
                    stream << "|";
                    break;
                case DNORTH:
                    stream << "ǁ";
                    break;
                case WATER: 
                    stream << ".";
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

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

    BacktrackStack = vector<Bridge>();
    
    CreateAdaptedGrid(jsonGrid);
}



void HashiGrid::CreateAdaptedGrid(const json& jsonGrid){
    
    json description = jsonGrid["description"];
    
    for(auto& tileValue : description){
        if(tileValue.get<int>() > 0) ++NumberOfIslands;
    }

    Islands = new Island*[NumberOfIslands];
    set<uint> rowsToAdd;
    set<uint> colsToAdd;
    NumberOfIslands = 0;

    uint index = 0;
    uint tmpN = jsonGrid["dimension"];
    uint tmpM = tmpN;
    int* tmpGrid = new int[tmpN*tmpM]; 

    for(auto& tileValue : description){
        uint population = tileValue.get<int>();
        if(population > 0){
            tmpGrid[index] = population;
            ++NumberOfIslands;
        } else tmpGrid[index] = WATER;

        ++index;
    }


   for(uint i=0; i<tmpN; ++i){
        for(int j=0; j<tmpM; ++j){
            if(tmpGrid[i*tmpM + j] >= 0){
                if(i != tmpN-1 && tmpGrid[(i+1) * tmpM + j] >= 0) rowsToAdd.insert(i+1);
                if(j != tmpM-1 && tmpGrid[i * tmpM + j + 1] >= 0) colsToAdd.insert(j+1);
            }
        }
    }

    set<uint> arrangedRowsToAdd;
    set<uint> arrangedColsToAdd;
    uint accumulator = 0;
    for(uint row : rowsToAdd){
        arrangedRowsToAdd.insert(row+accumulator);
        ++accumulator;
    }
    accumulator = 0;
    for(uint col : colsToAdd){
        arrangedColsToAdd.insert(col+accumulator);
        ++accumulator;
    }

    #ifdef HASHI_VERBOSE
    cout << "rows to add :";
    for(uint row : arrangedRowsToAdd){
        cout << row << " - ";
    }
    cout << endl;
    cout << "cols to add :";
    for(uint col : arrangedColsToAdd){
        cout << col << " - ";
    }
    cout << endl;
    #endif

    
    M = tmpM + colsToAdd.size();
    N = tmpN + rowsToAdd.size();
    Grid = new int[M*N];

    uint baseIndex = 0;
    NumberOfIslands = 0;
    for(int newIndex=0; newIndex<M*N; ++newIndex){
        if( find(arrangedRowsToAdd.begin(), arrangedRowsToAdd.end(), newIndex/M) != arrangedRowsToAdd.end() ){
            do {
                Grid[newIndex++] = WATER;
            } while(newIndex%M != 0);
            --newIndex;
        } else {
            if( find(arrangedColsToAdd.begin(), arrangedColsToAdd.end(), newIndex%M) != arrangedColsToAdd.end() ){
                Grid[newIndex] = WATER;
            } else {
                if(tmpGrid[baseIndex] > 0){
                    Islands[NumberOfIslands] = new Island(tmpGrid[baseIndex], GridCoords(newIndex/M, newIndex%M), this, NumberOfIslands);
                    Grid[newIndex] = NumberOfIslands++;
                } else Grid[newIndex] = WATER;
                ++baseIndex;
            }
        } 
    }

    free(tmpGrid);
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


void MyError(){
    int* a = (int*) malloc(sizeof(int));
    a[-321321] = 0;
}

void HashiGrid::Build(Bridge bridge){

    // Add this Bridge to the stack
    BacktrackStack.push_back(bridge);

    if(bridge.island1->BridgeLeft > 8 || bridge.island1->BridgeLeft == 0 
    || bridge.island2->BridgeLeft > 8 || bridge.island2->BridgeLeft == 0){
        MyError();
    }

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
    cout << "Backtrack stack is :";
    for(Bridge b : BacktrackStack){
        cout << b.island1->ID << " --> " << b.island2->ID << " | depth : " << b.depth << endl;
    }
    #endif

    while(BacktrackStack.back().depth != depth){
        DestroyLast();
    }

    #ifdef HASHI_VERBOSE
    cout << "Backtrack stack is :";
    for(Bridge b : BacktrackStack){
        cout << b.island1->ID << " --> " << b.island2->ID << " | depth : " << b.depth << endl;
    }
    #endif
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

    

    /////////// SIMPLIFY THE GRID //////////////
    // Many simplifications can be done at a certain depth.
    // It allows backtrack to remove all of them we getting back to previous depth.

    bool couldSimplify;
    do {
        for(int i=0; i<NumberOfIslands; ++i){
            Islands[i]->UpdateReachableIslands();
        }

        couldSimplify = Simplify(depth);
        #ifdef HASHI_VERBOSE
        if(couldSimplify) cout << *this << endl;
        #endif

    } while(couldSimplify);  
    ////////////////////////////////////////////
    
    vector<Bridge> buildableBridges;
    // Computes possible moves
    buildableBridges = GetBuildableBridges(depth+1);
    
    #ifdef HASHI_VERBOSE
    ++nodes;
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



bool HashiGrid::Simplify(uint depth){

    #ifdef HASHI_VERBOSE
    cout << "Trying to simplify the grid ..." << endl;
    bool JEN = JustEnoughNeighbors(depth);
    std::string JEN_str = JEN ? "true" : "false";
    cout << "JustEnoughNeighbors : " << JEN_str << endl;
    if(JEN){
        cout << "Skipping OnlyFewNeighbors" << endl << "Could simplify the grid !" << endl;
        return true;
    } 

    bool OFN = OnlyFewNeighbors(depth);
    std::string OFN_str = OFN ? "true" : "false";
    cout << "OnlyFewNeighbors : " << OFN_str << endl;
    if(OFN){
        cout << "Could simplify the grid !" << endl;
        return true;
    }
    
    return false;

    #else

    // The call order is important as it makes a priority order
    return JustEnoughNeighbors(depth) || OnlyFewNeighbors(depth);

    #endif
}


bool HashiGrid::JustEnoughNeighbors(uint depth){

    Island* source;
    for(int i=0; i<NumberOfIslands; ++i){

        source = Islands[i];

        #ifdef HASHI_VERBOSE
        cout << "Looking at island " << i << " with " << source->BridgeLeft << 
        " bridges left and " <<  source->ReachableIslands.size() << " possibilities" << endl;
        #endif

        // If there is the exact number of bridges to build, build them
        if(source->BridgeLeft > 0 && source->ReachableIslands.size() == source->BridgeLeft){
            for(Island* destination : source->ReachableIslands){
                // We build the bridge here, its destruction is managed through Backtrack() in Solve loop
                Build(Bridge(source ,destination, depth));
            }

            // If we could find an island, return to update grid state
            return true;
        }
    }

    return false;
}

bool HashiGrid::OnlyFewNeighbors(uint depth){

    Island* source;
    for(int i=0; i<NumberOfIslands; ++i){
        source = Islands[i];
        // If we know there is at least one bridge in each direction, build them
        if( source->BridgeLeft > 0 && (source->ReachableIslands.size()*2) - 1 == source->BridgeLeft){
            // We only want once each destination Island, not to build double bridges (JEN can do it)
            set<Island*> oneEachDirection(source->ReachableIslands.begin(), source->ReachableIslands.end());
            for(Island* destination : oneEachDirection){
                // We build the bridge here, its destruction is managed through Backtrack() in Solve loop
                Build(Bridge(source ,destination, depth));
            }

            // If we could find an island, return to update grid state
            return true;
        }
    }

    return false;
}



vector<Bridge> HashiGrid::GetBuildableBridges(uint depth){

    vector<Bridge> buildableBridges;
    set<Bridge> forbiddenBridges;
    Island* source;
    for(int i=0; i<NumberOfIslands; ++i){
        source = Islands[i];
        set<Island*> uniqueReachableIslands(source->ReachableIslands.begin(), source->ReachableIslands.end());
        for(Island* destination : uniqueReachableIslands){
            Bridge forbiddenBridge = Bridge(destination, source, depth);
            if( forbiddenBridges.find(forbiddenBridge) == forbiddenBridges.end()){
                Bridge toBuild(source, destination, depth);
                forbiddenBridges.insert(toBuild);
                buildableBridges.push_back(toBuild);
            }         
        }
    }

    return buildableBridges;
}


std::vector<Island*> HashiGrid::ReachableIslandsFrom(Island* island){

    std::vector<Island*> result;

    // Explore North
    //cout << "coords are " << island->Coords.i << "," << island->Coords.j << endl;
    bool twoPossible = island->BridgeLeft > 1;
    for(int i=island->Coords.i-1; i>=0; --i){
        int elmt = Grid[i * M + island->Coords.j];
        if(elmt == NORTH) twoPossible = false;
        else if(elmt == DNORTH || elmt == WEST || elmt == DWEST) break;
        else {
            Island* destination = Islands[elmt];
            if(elmt != WATER && elmt != NORTH && destination->BridgeLeft > 0){
                if( ! (island->Population == 1 && destination->Population == 1) ){
                    result.push_back(destination);
                    if(twoPossible && destination->BridgeLeft > 1) result.push_back(destination);
                    break;
                }  
            } 
        }
    }


    // Explore South
    twoPossible = island->BridgeLeft > 1;
    for(int i=island->Coords.i+1; i<N; ++i){
        int elmt = Grid[i * M + island->Coords.j];
        if(elmt == NORTH) twoPossible = false;
        else if(elmt == DNORTH || elmt == WEST || elmt == DWEST) break;
        else {
            Island* destination = Islands[elmt];
            if(elmt != WATER && elmt != NORTH && destination->BridgeLeft > 0){
                if( ! (island->Population == 1 && destination->Population == 1) ){
                    result.push_back(destination);
                    if(twoPossible && destination->BridgeLeft > 1) result.push_back(destination);
                    break;
                }
            }
        }
    }


    // Explore West
    twoPossible = island->BridgeLeft > 1;
    for(int j=island->Coords.j-1; j>=0; --j){
        int elmt = Grid[island->Coords.i * M + j];
        if(elmt == WEST) twoPossible = false;
        else if(elmt == DWEST || elmt == NORTH || elmt == DNORTH) break;
        else {
            Island* destination = Islands[elmt];
            if(elmt != WATER && elmt != WEST && destination->BridgeLeft > 0){
                if( ! (island->Population == 1 && destination->Population == 1) ){
                    result.push_back(destination);
                    if(twoPossible && destination->BridgeLeft > 1) result.push_back(destination);
                    break;
                }
            }
        }
    }

    
    // Explore East
    twoPossible = island->BridgeLeft > 1;
    for(int j=island->Coords.j+1; j<M; ++j){
        int elmt = Grid[island->Coords.i * M + j];
        if(elmt == WEST) twoPossible = false;
        else if(elmt == DWEST || elmt == NORTH || elmt == DNORTH) break;
        else {
            Island* destination = Islands[elmt];
            if(elmt != WATER && elmt != WEST && destination->BridgeLeft > 0){
                if( ! (island->Population == 1 && destination->Population == 1) ){
                    result.push_back(destination);
                    if(twoPossible && destination->BridgeLeft > 1) result.push_back(destination);
                    break;
                }
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

    #ifdef HASHI_VERBOSE
    cout << "Trying to validate with construction : " << endl;
    for(Bridge b : BacktrackStack){
        cout << b.island1->ID << " --> " << b.island2->ID << endl;
    }
    ++leafs;
    #endif

    struct Node_t{
        vector<uint> links;
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

    #ifdef HASHI_VERBOSE
    cout << "Created nodeArray is :";
    for(int i=0; i<NumberOfIslands; ++i){
        cout << "For island " << i << " : ";
        for(uint dest : nodeArray[i].links){
            cout << dest << " - "; 
        }
        cout << endl;
    }
    #endif
    // Check if all island can reach others
    int reached = 0;
    queue<Node_t*> toCheck;
    toCheck.push(&nodeArray[0]);

    while(toCheck.size() != 0){
        Node_t* actual = toCheck.front();
        if( !actual->marked){
            for(uint next : actual->links){
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
                    if(elmt < 10)
                        stream << elmt;
                    else stream << (char)('A' + elmt - 10);
            }
        }
        stream << endl;
    }

    for(int i=0; i<N; ++i) stream << "#";
    
    stream << '\n';
}
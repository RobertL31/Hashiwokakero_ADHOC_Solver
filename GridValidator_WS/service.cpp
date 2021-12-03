#include "pistache/endpoint.h"
#include "json.hpp"
#include <iostream>
#include <string>
#include <set>
#include <queue>

using namespace std;
using namespace Pistache;
using json = nlohmann::json;

struct Node_t{
    set<int> links;
    bool marked;
};

void JsonToNodeArray(json& jsonInput, Node_t* nodeArray){
    
    int i = 0;
    for(auto& jsonIsland : jsonInput){
        set<int> a;
        jsonIsland["connections"].get_to(a);
        nodeArray[i++] = {.links = a, .marked = false};
    }
}


int ExploreNodeArray(Node_t* nodeArray){

    int checked = 0;
    queue<Node_t*> toCheck;
    toCheck.push(&nodeArray[0]);

    while(toCheck.size() != 0){
        Node_t* actual = toCheck.front();
        if( !actual->marked){
            for(const int& next : actual->links){
                //cout << next << endl;
                toCheck.push(&nodeArray[next]);
            }
            actual->marked = true;
            ++checked;
        }
        toCheck.pop();
    }

    return checked;
}



class BasicHandler : public Http::Handler
{
public:
    HTTP_PROTOTYPE(BasicHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override
    {
        if(request.method() != Http::Method::Post){
            response.send(Pistache::Http::Code::Bad_Request, "Only POST operations with JSON body are accepted\n");
            return;
        }

        // false in json::parse indicates that it should not throw exceptions
        json jsonInput = json::parse(request.body(), nullptr, false);
        if(jsonInput.is_discarded()){
            response.send(Pistache::Http::Code::Bad_Request, "Given JSON is not well-formed\n");
            cout << request.body() << endl;
            return;
        }


        json jsonIslands = jsonInput["grid"]["islands"];

        // Check if connections number is the same as population for every island   
        bool check = true;
        for(auto& jsonIsland : jsonIslands){
            if(jsonIsland["population"].get<int>() != jsonIsland["connections"].size()){
                check = false;
                //cout << "Island " << jsonIsland["uuid"] << " is not valid !" << endl;
                break;
            } else {
                //cout << "Island " << jsonIsland["uuid"] << " has " << jsonIsland["population"].get<int>() << " needs and " << jsonIsland["connections"].size() << " connection !" << endl;
            }
        }


        // Check if all island can reach others
        Node_t* nodeArray = new Node_t[jsonIslands.size()];
        JsonToNodeArray(jsonIslands, nodeArray);

        /*
        for(int i=0; i<jsonIslands.size(); ++i){
           cout << "ISLAND " << i << "[" << endl; 
           for(const int& el : nodeArray[i].links){
               cout << el << ";" << endl;
           }
           cout << "MARKED " << nodeArray[i].marked << endl;
        }
        */

        //cout << "size : " << jsonIslands.size() << endl;
        check = check && (ExploreNodeArray(nodeArray) == jsonIslands.size());

        //cout << "received json :\n" << json_input.dump(4) << endl;
        response.send(Pistache::Http::Code::Ok, check ? "0" : "1" );
    }
};

int main()
{   
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(50500));
    auto opts = Pistache::Http::Endpoint::options()
                    .threads(1);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(Http::make_handler<BasicHandler>());
    
    cout << "Server listening on PORT 50500 : " << endl;
    server.serve();
}

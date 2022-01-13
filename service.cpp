#include "pistache/endpoint.h"
#include "json.hpp"

#include <iostream>
#include <experimental/filesystem>
#include <string>


#include "HashiGrid.hpp"

using namespace std;
using namespace Pistache;
using json = nlohmann::json;


class BasicHandler : public Http::Handler
{
public:
    HTTP_PROTOTYPE(BasicHandler)

    bool CheckJSONContent(const Http::Request& request, json& parsedJson, string& errorMessage){

        // false in json::parse indicates that it should not throw exceptions
        parsedJson = json::parse(request.body(), nullptr, false);
        if(parsedJson.is_discarded()){
            errorMessage = "Delimiter miss-match in JSON";
            return false;
        }

        // Check if the requiered fields are in JSON
        if(!parsedJson.contains("grid")){
            errorMessage = "No field 'grid' found in JSON";
            return false; 
        }
        if(!parsedJson["grid"].contains("dimension")){
            errorMessage = "No field 'dimension' found in JSON";
            return false;
        }
        if(!parsedJson["grid"].contains("description")){
            errorMessage = "No field 'description' found in JSON";
            return false;
        }

        // Check if requiered fields have legal values
        uint dimension = parsedJson["grid"]["dimension"].get<int>();
        if(dimension <= 0){
            errorMessage = "Field 'dimension' must have a positive value";
            return false;
        }
        if(parsedJson["grid"]["description"].size() != dimension*dimension){
            errorMessage = "Field 'description' must contains " + to_string(dimension) + "^2 values";
            return false;
        }

        for(json& tile : parsedJson["grid"]["description"]){
            if(tile.get<int>() < 0 || tile.get<int>() > 8) return false;
        }

        return true;
    }

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override
    {

        cout << "Request handles !!" << endl;
        if(request.method() != Http::Method::Post){
            response.send(Pistache::Http::Code::Bad_Request, "Only POST operations with JSON body are accepted\n");
            return;
        }

        json jsonInput;
        string errorMessage;
        bool JsonOK = CheckJSONContent(request, jsonInput, errorMessage);
        
        if(!JsonOK){
            response.send(Pistache::Http::Code::Bad_Request, errorMessage + "\nGiven JSON is not well-formed, aborting resolution\n");
            cout << errorMessage << "\nReceived JSON is :\n" << jsonInput.dump(4) << endl;
            return;
        }
        

        HashiGrid* hashi = new HashiGrid(jsonInput["grid"]);
        cout << *hashi << endl;

        json outJson;
        if(hashi->Solve(0)){
            hashi->BuildSolution(outJson);
            cout << "\n\n\n Solved grid is :" << endl;
            cout << *hashi << endl;
        } else {
            outJson["connections"] = vector<int>();
            cout << "Could not solve the grid" << endl;
        }

        delete hashi;
        response.send(Pistache::Http::Code::Ok , outJson.dump());
        return;
    }
};

int main(int argc, char *argv[])
{   
    if(argc == 2){

        if( !std::experimental::filesystem::is_regular_file(argv[1])){
            cout << "File couldn't be opened : " << argv[1] << endl;
            return 1;
        }

        cout << "A filename has been given, switching to offline mode" << endl;

        HashiGrid* hashi = new HashiGrid(string(argv[1]));
        cout << *hashi << endl;

        if(hashi->Solve(0)){
            cout << "\n\n\n Solved grid is :" << endl;
            cout << *hashi << endl;
        }

        return 0;

    } else{
        Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(50003));
        auto opts = Pistache::Http::Endpoint::options()
                        .threads(1);

        Http::Endpoint server(addr);
        server.init(opts);
        server.setHandler(Http::make_handler<BasicHandler>());

        cout << "Server listening on PORT 50003 : " << endl;
        server.serve();
    }

    
}

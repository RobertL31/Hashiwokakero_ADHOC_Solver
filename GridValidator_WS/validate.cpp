#include "pistache/endpoint.h"
#include "json.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace Pistache;
using json = nlohmann::json;

struct Node_t{
    int nLinks;
    int* links;
    bool marked;
};

Node_t* JsonToNodeArray(json& jsonInput){
    //TODO
    return nullptr;
}


class BasicHandler : public Http::Handler
{
public:
    HTTP_PROTOTYPE(BasicHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override
    {
        if(request.method() != Http::Method::Get){
            response.send(Pistache::Http::Code::Bad_Request, "Only GET operations with JSON body are accepted\n");
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
        Node_t* node_array = JsonToNodeArray(jsonIslands);

        //cout << "received json :\n" << json_input.dump(4) << endl;
        response.send(Pistache::Http::Code::Ok, "Your request body was :\n" + jsonInput.dump(4));
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

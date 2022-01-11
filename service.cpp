#include "pistache/endpoint.h"
#include "json.hpp"

#include <iostream>
#include <string>


#include "HashiGrid.hpp"

using namespace std;
using namespace Pistache;
using json = nlohmann::json;


class BasicHandler : public Http::Handler
{
public:
    HTTP_PROTOTYPE(BasicHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override
    {

        cout << "Request handles !!" << endl;
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

        HashiGrid* hashi = new HashiGrid(jsonInput["grid"]);
        cout << *hashi << endl;

        hashi->Solve(0);

        cout << "\n\n\n Solved grid is :" << endl;

        cout << *hashi << endl;

        json outJson;
        hashi->BuildSolution(outJson);

        delete hashi;
        response.send(Pistache::Http::Code::Ok , outJson.dump());
        return;
    }
};

int main()
{   
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(50501));
    auto opts = Pistache::Http::Endpoint::options()
                    .threads(1);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(Http::make_handler<BasicHandler>());

    cout << "Server listening on PORT 50501 : " << endl;
    server.serve();
}

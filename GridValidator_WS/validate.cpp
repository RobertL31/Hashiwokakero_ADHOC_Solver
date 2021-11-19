#include "pistache/endpoint.h"
#include "json.hpp"
#include <iostream>
#include <string>

using namespace std;
using namespace Pistache;

using json = nlohmann::json;

class BasicHandler : public Http::Handler
{
public:
    HTTP_PROTOTYPE(BasicHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override
    {
        json json_request = json::parse(request.body());
        cout << "received json " << json_request << endl;
        json_request["param0"] = 1;
        response.send(Pistache::Http::Code::Ok, json_request.dump(3));
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

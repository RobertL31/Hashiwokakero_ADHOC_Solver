#include "pistache/endpoint.h"
#include "json.hpp"
#include <iostream>

using namespace std;
using namespace Pistache;

using json = nlohmann::json;

class HelloHandler : public Http::Handler
{
public:
    HTTP_PROTOTYPE(HelloHandler)

    void onRequest(const Http::Request& /*request*/, Http::ResponseWriter response) override
    {
        response.send(Pistache::Http::Code::Ok, "Hello World\n");
    }
};

int main()
{
    json j;
    j["coucou"] = 3;
    
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(50500));
    auto opts = Pistache::Http::Endpoint::options()
                    .threads(1);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(Http::make_handler<HelloHandler>());
    
    cout << "Server listening on PORT 50500 : " << j["coucou"] << endl;
    server.serve();
}

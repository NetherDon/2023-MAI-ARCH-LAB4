#include <iostream>
#include "../web_server/http_web_server.h"
#include "../web_server/http_request_factory.h"
#include "cart_handler.h"
#include <vector>

int main(int argc, char*argv[]) 
{
    HTTPWebServer app(8082, [](const std::string &format)
    {
        database::Cart::init();
        std::vector<std::string> paths = { 
                "/cart", 
                "/cart/put" 
            };

        return new HTTPRequestFactory(
            format, 
            "Cart", 
            [](const std::string &format2)
            {
                return new CartHandler(format2);
            },
            paths);
    });
    return app.run(argc, argv);
}
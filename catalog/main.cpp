#include <iostream>
#include "../web_server/http_web_server.h"
#include "../web_server/http_request_factory.h"
#include "product_handler.h"
#include <vector>

int main(int argc, char*argv[]) 
{
    HTTPWebServer app(8081, [](const std::string &format)
    {
        database::Product::init();
        std::vector<std::string> paths = { 
                "/catalog", 
                "/catalog/add" 
            };

        return new HTTPRequestFactory(
            format, 
            "Product", 
            [](const std::string &format2)
            {
                return new ProductHandler(format2);
            },
            paths);
    });

    return app.run(argc, argv);
}
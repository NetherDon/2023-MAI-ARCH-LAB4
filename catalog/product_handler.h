#ifndef PRODUCTHANDLER_H
#define PRODUCTHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../database/catalog.h"
#include "../helper.h"

class ProductHandler : public HTTPRequestHandler
{
private:
    bool check_price(const int &price, std::string &reason)
    {
        if (price < 0)
        {
            reason = "The price cannot be negative";
            return false;
        }

        return true;
    }

public:
    ProductHandler(const std::string &format) : _format(format)
    {
        
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());

        try
        {
            std::string path = request.getURI();
            int i = path.find('?');
            if (i != -1)
            {
                path = path.substr(0, i);
            }
            
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                if (path == "/catalog")
                {
                    auto results = database::Product::read_all();
                    Poco::JSON::Array arr;
                    for (auto s : results)
                        arr.add(s.toJSON(false));
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(arr, ostr);

                    return;
                }
            }
            else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                if (path == "/catalog/add" && form.has("name") && form.has("type") && form.has("manufactorer") && form.has("price"))
                {
                    database::Product product;
                    product.name() = form.get("name");
                    product.type() = form.get("type");
                    product.manufactorer() = form.get("manufactorer");
                    
                    if (form.has("description"))
                        product.description() = form.get("description");

                    bool flag = true;
                    std::string err_mess;
                    try
                    {
                        product.price() = stoi(form.get("price"));
                        if (!check_price(product.price(), err_mess))
                        {
                            flag = false;
                        }
                    }
                    catch (...)
                    {
                        flag = false;
                        err_mess = "Price must be a whole number";
                    }

                    if (flag)
                    {
                        product.save_to_mysql();
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        ostr << product.get_id();
                        return;
                    }
                    else
                    {
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                        std::ostream &ostr = response.send();
                        ostr << err_mess;
                        response.send();
                        return;
                    }
                }
            }
        }
        catch (...) { throw; }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "Request not found");
        root->set("instance", "/catalog");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

private:
    std::string _format;
};

#endif
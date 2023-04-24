#ifndef USEHANDLER_H
#define USEHANDLER_H

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
#include <exception>

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

#include "../database/cart.h"
#include "../helper.h"

class CartHandler : public HTTPRequestHandler
{
private:
    bool check_id(const int &id, std::string &reason)
    {
        if (id < 0)
        {
            reason = "Product ID cannot be negative";
            return false;
        }

        return true;
    }

    std::optional<std::string> getUser(HTTPServerRequest &request, std::string &login)
    {
        std::string scheme;
        std::string info;
        std::string password;

        bool authorized = false;
        std::cout << "Before get credentials" << std::endl;
        try
        {
            request.getCredentials(scheme, info);
        }
        catch (std::exception &e)
        {
            std::cout << "Creadentials error: " << e.what() << std::endl;
        }
        
        if (scheme == "Basic")
        {
            get_identity(info, login, password);
            std::string host = "localhost";

            char *envhost = std::getenv("SERVICE_HOST");
            if (envhost != nullptr) 
                host = envhost;

            std::string url = "http://" + host + ":8080/account/auth";

            return tryAuthorize(url, login, password);
        }

        return {};
    }

    void handleGet(std::string &login, [[maybe_unused]] HTTPServerRequest &request, HTTPServerResponse &response)
    {
        auto results = database::Cart::readAllFor(login);
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

    void handlePut(std::string login, HTMLForm &form, [[maybe_unused]] HTTPServerRequest &request, HTTPServerResponse &response)
    {
        database::Cart cart;
        cart.user() = login;

        bool flag = true;
        std::string err_mess;
        try
        {
            cart.product() = stoi(form.get("product_id"));
            if (!check_id(cart.product(), err_mess))
            {
                flag = false;
            }
        }
        catch (...)
        {
            flag = false;
            err_mess = "Product ID must be a whole positive number";
        }

        if (flag)
        {
            cart.saveToMySQL();
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            std::ostream &ostr = response.send();
            ostr << cart.getId();
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

public:
    CartHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response)
    {
        std::cout << "Handle Request" << std::endl;
        HTMLForm form(request, request.stream());

        std::string login;
        std::optional<std::string> userIdStr = getUser(request, login);

        if (!userIdStr.has_value())
        {
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("type", "/errors/unauthorized");
            root->set("title", "Unauthorized");
            root->set("status", "401");
            root->set("detail", "invalid login or password");
            root->set("instance", "/account");
            std::ostream &ostr = response.send();
            Poco::JSON::Stringifier::stringify(root, ostr);
            return;
        }

        try
        {
            /*
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(userIdStr.value());
            Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

            long userId = object->getValue<long>("id");
            */
           
            std::string path = request.getURI();
            int i = path.find('?');
            if (i != -1)
            {
                path = path.substr(0, i);
            }
            
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            {
                if (path == "/cart")
                {
                    handleGet(login, request, response);
                }
            }
            else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                if (path == "/cart/put" && form.has("product_id"))
                {
                    handlePut(login, form, request, response);
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request ot found");
        root->set("instance", "/cart");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

private:
    std::string _format;
};

#endif
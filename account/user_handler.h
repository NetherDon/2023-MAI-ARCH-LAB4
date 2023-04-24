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
#include <experimental/optional>

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

#include "../database/user.h"
#include "../helper.h"

class UserHandler : public HTTPRequestHandler
{
private:
    bool check_name(const std::string &name, std::string &reason)
    {
        if (name.length() < 3)
        {
            reason = "Name must be at leas 3 signs";
            return false;
        }

        if (name.find(' ') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        if (name.find('\t') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        return true;
    };

    bool check_email(const std::string &email, std::string &reason)
    {
        if (email.find('@') == std::string::npos)
        {
            reason = "Email must contain @";
            return false;
        }

        if (email.find(' ') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        if (email.find('\t') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        return true;
    };

public:
    UserHandler(const std::string &format) : _format(format)
    {
    }

    Poco::JSON::Object::Ptr remove_password(Poco::JSON::Object::Ptr src)
    {
        if (src->has("password"))
            src->set("password", "*******");
        return src;
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            if (form.has("login") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
            {
                std::string login = form.get("login").c_str();
                bool useCache = !form.has("no_cache");

                if (useCache)
                {
                    std::optional<database::User> result = database::User::readByLogin(login, database::Scope::Cache);
                    if (result)
                    {
                        //std::cout << "from cache" << std::endl;
                        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                        response.setChunkedTransferEncoding(true);
                        response.setContentType("application/json");
                        std::ostream &ostr = response.send();
                        Poco::JSON::Stringifier::stringify(remove_password(result->toJSON()), ostr);
                        return;
                    }
                }
                
                std::optional<database::User> result = database::User::readByLogin(login, database::Scope::Query);
                if (result)
                {
                    if (useCache)
                    {
                        result->saveToCache();
                    }

                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(remove_password(result->toJSON()), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/not_found");
                    root->set("title", "Internal exception");
                    root->set("status", "404");
                    root->set("detail", "User with login '" + login + "' not found");
                    root->set("instance", "/account");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
            }
            else if (hasSubstr(request.getURI(), "/account/auth"))
            {
                if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
                {
                    std::string scheme;
                    std::string info;
                    request.getCredentials(scheme, info);
                    std::cout << "scheme: " << scheme << " identity: " << info << std::endl;

                    std::string login, password;
                    if (scheme == "Basic")
                    {
                        get_identity(info, login, password);
                        if (auto id = database::User::logIn(login, password))
                        {
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            std::ostream &ostr = response.send();
                            ostr << "{ \"id\" : \"" << *id << "\"}" << std::endl;
                            return;
                        }
                    }

                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/unauthorized");
                    root->set("title", "Internal exception");
                    root->set("status", "401");
                    root->set("detail", "not authorized");
                    root->set("instance", "/account/auth");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
                else if(request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
                {
                    if (form.has("first_name") && form.has("last_name") && form.has("email") && form.has("title") && form.has("login") && form.has("password"))
                    {
                        database::User user;
                        user.firstName() = form.get("first_name");
                        user.lastName() = form.get("last_name");
                        user.email() = form.get("email");
                        user.title() = form.get("title");
                        user.login() = form.get("login");
                        user.password() = form.get("password");

                        bool check_result = true;
                        std::string message;
                        std::string reason;

                        if (!check_name(user.getFirstName(), reason))
                        {
                            check_result = false;
                            message += reason;
                            message += "<br>";
                        }

                        if (!check_name(user.getLastName(), reason))
                        {
                            check_result = false;
                            message += reason;
                            message += "<br>";
                        }

                        if (!check_email(user.getEmail(), reason))
                        {
                            check_result = false;
                            message += reason;
                            message += "<br>";
                        }

                        if (check_result)
                        {
                            user.saveToMySQL();
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            std::ostream &ostr = response.send();
                            ostr << user.get_id();
                            return;
                        }
                        else
                        {
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                            std::ostream &ostr = response.send();
                            ostr << message;
                            response.send();
                            return;
                        }
                    }
                }
            }
            else if (hasSubstr(request.getURI(), "/account/search"))
            {

                std::string fn = form.get("first_name");
                std::string ln = form.get("last_name");
                auto results = database::User::search(fn, ln);
                Poco::JSON::Array arr;
                for (auto s : results)
                    arr.add(remove_password(s.toJSON()));
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(arr, ostr);

                return;
            }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request not found");
        root->set("instance", "/account");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

private:
    std::string _format;
};
#endif
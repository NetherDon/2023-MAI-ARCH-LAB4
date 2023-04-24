#include "user.h"
#include "database.h"
#include "cache.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void User::init()
    {
        try
        {
            for (auto &hint : database::Database::get_all_hints())
            {
                Poco::Data::Session session = database::Database::get().create_session();
                Statement create_stmt(session);
                create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                            << "`first_name` VARCHAR(256) NOT NULL,"
                            << "`last_name` VARCHAR(256) NOT NULL,"
                            << "`login` VARCHAR(256) NOT NULL UNIQUE,"
                            << "`password` VARCHAR(256) NOT NULL,"
                            << "`email` VARCHAR(256) NULL,"
                            << "`title` VARCHAR(1024) NULL,"
                            << "PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));"
                            << hint,
                    now;
            }
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl << e.displayText() << std::endl;
            throw;
        }
    }

    void User::saveToCache()
    {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(_login, message);
    }

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("title", _title);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();


        user.id() = object->getValue<long>("id");
        user.firstName() = object->getValue<std::string>("first_name");
        user.lastName() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.title() = object->getValue<std::string>("title");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }

    std::optional<long> User::logIn(std::string &login, std::string &password)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            long id;

            std::string hint = Database::sharding_hint(login);

            select << "SELECT id FROM User where login=? and password=? "
                   << hint,
                into(id),
                use(login),
                use(password),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return id;
            return std::nullopt;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::optional<User> User::readByLogin(std::string &login, Scope scope)
    {
        if (scope == Scope::Cache)
        {
            try
            {
                std::string result;
                if (database::Cache::get().get(login, result))
                    return fromJSON(result);
                else
                    return {};
            }
            catch (std::exception& err)
            {
                std::cerr << "error:" << err.what() << std::endl;
                return {};
            }
        }
        else if (scope == Scope::Query)
        {
            try
            {
                Poco::Data::Session session = database::Database::get().create_session();
                Poco::Data::Statement select(session);
                User a;

                std::string hint = Database::sharding_hint(login);
                select << "SELECT id, first_name, last_name, email, title,login,password FROM User WHERE login=? "
                    << hint,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    use(login),
                    range(0, 1); //  iterate over result set one row at a time

                select.execute();
                Poco::Data::RecordSet rs(select);
                if (rs.moveFirst()) return a;
                return std::nullopt;
            }
            catch (Poco::Data::MySQL::ConnectionException &e)
            {
                std::cout << "connection:" << e.what() << std::endl;
                throw;
            }
            catch (Poco::Data::MySQL::StatementException &e)
            {

                std::cout << "statement:" << e.what() << std::endl;
                throw;
            }
        }

        return {};
    }

    std::optional<User> User::readById(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User a;
            select << "SELECT id, first_name, last_name, email, title,login,password FROM User where id=?",
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                into(a._login),
                into(a._password),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
            return std::nullopt;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<User> User::readAll()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;
            User a;

            for (auto &hint : database::Database::get_all_hints())
            {
                Statement select(session);
                select << "SELECT id, first_name, last_name, email, title, login, password FROM User;"
                       << hint,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
            }
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<User> User::search(std::string first_name, std::string last_name)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;
            User a;
            first_name += "%";
            last_name += "%";

            for (auto &hint : database::Database::get_all_hints())
            {
                Statement select(session);
                std::cout << "Search Hint: " << hint << std::endl;
                select << "SELECT id, first_name, last_name, email, title, login, password FROM User where first_name LIKE ? and last_name LIKE ? "
                       << hint,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    use(first_name),
                    use(last_name),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                    {
                        std::cout << "Result: " << a.toJSON() << std::endl; 
                        result.push_back(a);
                    }
                }
            }

            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void User::saveToMySQL()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            std::string selectStr = "INSERT INTO User (first_name,last_name,email,title,login,password) VALUES(?, ?, ?, ?, ?, ?) ";
            std::string hint = Database::sharding_hint(_login);
            selectStr += hint;

            insert << selectStr,
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_title),
                use(_login),
                use(_password);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID() " + hint,
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "Inserted user id: " << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    const std::string &User::getLogin() const
    {
        return _login;
    }

    const std::string &User::getPassword() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    const std::string &User::getFirstName() const
    {
        return _first_name;
    }

    const std::string &User::getLastName() const
    {
        return _last_name;
    }

    const std::string &User::getEmail() const
    {
        return _email;
    }

    const std::string &User::getTitle() const
    {
        return _title;
    }

    long &User::id()
    {
        return _id;
    }

    std::string &User::firstName()
    {
        return _first_name;
    }

    std::string &User::lastName()
    {
        return _last_name;
    }

    std::string &User::email()
    {
        return _email;
    }

    std::string &User::title()
    {
        return _title;
    }
}
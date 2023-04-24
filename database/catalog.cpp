#include "catalog.h"
#include "database.h"
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
    void Product::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Product` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`name` VARCHAR(256) NOT NULL,"
                        << "`type` VARCHAR(256) NOT NULL,"
                        << "`description` VARCHAR(1024) NULL,"
                        << "`manufactorer` VARCHAR(256) NOT NULL,"
                        << "`price` INT NOT NULL,"
                        << "PRIMARY KEY (`id`),KEY `n` (`name`));",
                now;
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

    std::vector<Product> Product::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Product> result;
            Product a;
            select << "SELECT id, name, type, description, manufactorer, price FROM Product",
                into(a._id),
                into(a._name),
                into(a._type),
                into(a._description),
                into(a._manufactorer),
                into(a._price),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
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

    void Product::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Product (name,type,description,manufactorer,price) VALUES(?, ?, ?, ?, ?)",
                use(_name),
                use(_type),
                use(_description),
                use(_manufactorer),
                use(_price);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "Inserted product id: " << _id << std::endl;
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

    Poco::JSON::Object::Ptr Product::toJSON(bool exclude_id=false) const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        if (!exclude_id)
            root->set("id", _id);
        root->set("name", _name);
        root->set("type", _type);
        root->set("description", _description);
        root->set("manufactorer", _manufactorer);
        root->set("price", _price);

        return root;
    }

    long Product::get_id() const { return _id; }
    const std::string &Product::get_name() const { return _name; }
    const std::string &Product::get_type() const { return _type; }
    const std::string &Product::get_description() const { return _description; }
    const std::string &Product::get_manufactorer() const { return _manufactorer; }
    const int &Product::get_price() const { return _price; }

    long& Product::id() { return _id; }
    std::string &Product::name() { return _name; }
    std::string &Product::type() { return _type; }
    std::string &Product::description() { return _description; }
    std::string &Product::manufactorer() { return _manufactorer; }
    int &Product::price() { return _price; }
}
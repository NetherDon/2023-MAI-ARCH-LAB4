#include "database.h"
#include "../config/config.h"

namespace database{
    Database::Database()
    {
        const Config cfg = Config::get();
        std::stringstream sstream;
        sstream << "host="     << cfg.getHost()     << ";"
                << "port="     << cfg.getPort()     << ";"
                << "db="       << cfg.getDatabase() << ";"
                << "user="     << cfg.getLogin()    << ";"
                << "password=" << cfg.getPassword();

        _connection_string = sstream.str();
        std::cout << "Connection string: " << _connection_string << std::endl;
        Poco::Data::MySQL::Connector::registerConnector();
    }

    Database& Database::get()
    {
        static Database _instance;
        return _instance;
    }

    size_t Database::get_max_shard()
    {
        return 2;
    }

    std::vector<std::string> Database::get_all_hints()
    {
        std::vector<std::string> result;
        for(size_t i = 0; i < get_max_shard(); ++i)
            result.push_back("-- sharding:" + std::to_string(i));
        return result;
    }

    std::string Database::sharding_hint(std::string &login)
    {
        size_t shard_number = std::hash<std::string>{}(login)%get_max_shard();
        return "-- sharding:" + std::to_string(shard_number);
    }

    Poco::Data::Session Database::create_session()
    {
        return Poco::Data::Session(
            Poco::Data::SessionFactory::instance().create(
                Poco::Data::MySQL::Connector::KEY,
                _connection_string
            )
        );
    }

}
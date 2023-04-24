#ifndef CATALOG_H
#define CATALOG_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Product{
        private:
            long _id;
            std::string _name;
            std::string _type;
            std::string _description;
            std::string _manufactorer;
            int _price;

        public:

            static Product fromJSON(const std::string & str);

            long             get_id() const;
            const std::string &get_name() const;
            const std::string &get_type() const;
            const std::string &get_description() const;
            const std::string &get_manufactorer() const;
            const int &get_price() const;

            long&        id();
            std::string &name();
            std::string &type();
            std::string &description();
            std::string &manufactorer();
            int &price();

            static void init();
            static std::vector<Product> read_all();
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON(bool exclude_id) const;

    };
}

#endif
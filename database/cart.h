#ifndef CART_H
#define CART_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>
#include "catalog.h"

namespace database
{
    class Cart{
        private:
            long _id;
            long _product_id;
            std::string _user_login;

        static std::vector<Cart> read_all(const std::string &where);

        public:

            static Cart fromJSON(const std::string & str);

            long getId() const;
            const long &getProductId() const;
            const std::string &getUser() const;

            long& id();
            long &product();
            std::string &user();

            static void init();
            static std::vector<Cart> readAll();
            static std::vector<Product> readAllFor(const std::string &login);
            void saveToMySQL();

            Poco::JSON::Object::Ptr toJSON(bool exclude_id) const;

    };
}

#endif
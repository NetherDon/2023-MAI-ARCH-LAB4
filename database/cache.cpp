#include "cache.h"
#include "../config/config.h"

#include <exception>
#include <mutex>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

namespace database
{
    std::mutex _mtx;
    Cache::Cache()
    {
        std::string cache = Config::get().getCacheServers();
        int sepindex = cache.find(':');

        std::string host = cache.substr(0, sepindex);
        std::string port = cache.substr(sepindex + 1);

        std::cout << "Cache host: " << host << ", Port: " << port << std::endl;
        _stream = rediscpp::make_stream(host, port);
    }

    Cache Cache::get()
    {
        static Cache instance;
        return instance;
    }

    void Cache::put([[maybe_unused]] const std::string &login, [[maybe_unused]] const std::string &val)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "set",
                                                     login,
                                                     val, "ex", "60");
    }

    bool Cache::get([[maybe_unused]] const std::string &login, [[maybe_unused]] std::string &val)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "get", login);

        if (response.is_error_message())
            return false;
        if (response.empty())
            return false;

        val = response.as<std::string>();
        return true;
    }
}
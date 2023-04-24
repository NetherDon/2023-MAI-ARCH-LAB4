cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (EXAMPLE_BINARY "account")
SET (CATALOG "catalog")
SET (CART "cart")

set (STD_CXX "c++17")
set (REDISCPP_FLAGS "-DREDISCPP_HEADER_ONLY=ON")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX} ${REDISCPP_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -Wall -std=${STD_CXX} -DNDEBUG ${REDISCPP_FLAGS}")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)


if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${EXAMPLE_BINARY} account/main.cpp 
                                 config/config.cpp
                                 database/cache.cpp 
                                 database/database.cpp
                                 database/user.cpp)


target_include_directories(${EXAMPLE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${EXAMPLE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused-variable )
set_target_properties(${EXAMPLE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${EXAMPLE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${EXAMPLE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)




add_executable(${CATALOG} catalog/main.cpp
                          config/config.cpp 
                          database/database.cpp
                          database/catalog.cpp)
                          
target_include_directories(${CATALOG} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${CATALOG} PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused-variable )
set_target_properties(${CATALOG} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${CATALOG} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${CATALOG} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)





add_executable(${CART} cart/main.cpp
                             config/config.cpp 
                             database/database.cpp
                             database/catalog.cpp
                             database/cart.cpp)
                            
target_include_directories(${CART} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${CART} PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused-variable )
set_target_properties(${CART} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${CART} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${CART} PRIVATE 
                            ${CMAKE_THREAD_LIBS_INIT} 
                            ${Poco_LIBRARIES}
                            "PocoData"
                            "PocoDataMySQL"
                            "mysqlclient"
                            ZLIB::ZLIB)
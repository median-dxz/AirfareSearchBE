cmake_minimum_required(VERSION 3.22)

project(airfare-search-service)

set(DIR_MODULE_DATABASE ${CMAKE_CURRENT_SOURCE_DIR}/database)
set(DIR_MODULE_SEARCH_SERVICE ${CMAKE_CURRENT_SOURCE_DIR}/search-serivce)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_LINKER "lld")

find_package(Threads REQUIRED)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

find_package(SQLite3 REQUIRED)
find_package(SqliteOrm REQUIRED)

find_package(spdlog CONFIG REQUIRED)

set(database_sources
    ${DIR_MODULE_DATABASE}/database.cpp
    ${DIR_MODULE_DATABASE}/database.h
    ${DIR_MODULE_DATABASE}/models.h
)

set(service_sources
    ${DIR_MODULE_SEARCH_SERVICE}/service.cpp
    ${DIR_MODULE_SEARCH_SERVICE}/service.h
)

add_subdirectory(rpc)
add_subdirectory(test)
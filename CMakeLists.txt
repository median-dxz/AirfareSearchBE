cmake_minimum_required(VERSION 3.22)

project(airfare-search-service)

set(DIR_MODULE_DATABASE ${CMAKE_CURRENT_SOURCE_DIR}/database)
set(DIR_MODULE_SEARCH_SERVICE ${CMAKE_CURRENT_SOURCE_DIR}/search-serivce)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(rpc)
add_subdirectory(database)
add_subdirectory(search-serivce)
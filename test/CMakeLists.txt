project(airfare_search_test)

set(libs
    sqlite_orm::sqlite_orm
    SQLite::SQLite3
    spdlog::spdlog
)

# database test
add_executable(test_db db.cpp)

target_compile_definitions(test_db PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)

target_sources(test_db PRIVATE ${database_sources})
target_include_directories(test_db PRIVATE ${DIR_MODULE_DATABASE})

target_link_libraries(test_db ${libs})

# service test
add_executable(test_search search.cpp)

target_compile_definitions(test_search PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)

target_sources(test_search PRIVATE
    ${database_sources}
    ${service_sources}
)
target_include_directories(test_search PRIVATE ${DIR_MODULE_DATABASE})
target_include_directories(test_search PRIVATE ${DIR_MODULE_SEARCH_SERVICE})

target_link_libraries(test_search ${libs})
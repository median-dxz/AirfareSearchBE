project(airfare_search_test)

# database test
add_executable(test_db db.cpp)

target_compile_definitions(test_db PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)

target_sources(test_db PRIVATE
    ${DIR_MODULE_DATABASE}/database.cpp
    ${DIR_MODULE_DATABASE}/database.h
    ${DIR_MODULE_DATABASE}/models.h
)

target_include_directories(test_db PRIVATE ${DIR_MODULE_DATABASE})

target_link_libraries(test_db 
    sqlite_orm::sqlite_orm
    SQLite::SQLite3
    spdlog::spdlog
)
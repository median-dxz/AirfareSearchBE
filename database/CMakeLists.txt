project(db)

find_package(SQLite3 REQUIRED)
find_package(SqliteOrm REQUIRED)

find_package(spdlog CONFIG REQUIRED)

add_executable(test test.cpp)

target_compile_definitions(test PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)

target_sources(test PRIVATE
    database.cpp
    database.h
    models.h
)

target_link_libraries(test sqlite_orm::sqlite_orm SQLite::SQLite3)
target_link_libraries(test spdlog::spdlog)
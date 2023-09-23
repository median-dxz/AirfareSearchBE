find_package(SQLite3 REQUIRED)
find_package(SqliteOrm REQUIRED)

target_link_libraries(main sqlite_orm::sqlite_orm SQLite::SQLite3)
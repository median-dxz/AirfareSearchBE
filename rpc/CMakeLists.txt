project(rpc)

get_filename_component(rpc_proto "./protos/SearchRequest.proto" ABSOLUTE)
get_filename_component(rpc_proto_path "${rpc_proto}" PATH)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

find_package(SQLite3 REQUIRED)
find_package(SqliteOrm REQUIRED)

find_package(spdlog CONFIG REQUIRED)

add_executable(main main.cpp)

set(protos_dir "protos")
file(GLOB_RECURSE rpc_server_gen
    "${protos_dir}/*.cc"
    "${protos_dir}/*.h"
)
target_sources(main PRIVATE ${rpc_server_gen})
target_sources(main PRIVATE
    ${DIR_MODULE_DATABASE}/database.cpp
    ${DIR_MODULE_DATABASE}/database.h
)
target_compile_definitions(main PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)
target_include_directories(main PRIVATE ${DIR_MODULE_DATABASE})

file(GLOB_RECURSE search_service_source
    "${DIR_MODULE_SEARCH_SERVICE}/*.cpp"
    "${DIR_MODULE_SEARCH_SERVICE}/*.h"
)
target_sources(main PRIVATE search_service_source)
target_include_directories(main PRIVATE ${DIR_MODULE_SEARCH_SERVICE})

target_link_libraries(main 
    absl::flags
    absl::flags_parse
    protobuf::libprotobuf
    gRPC::grpc++_reflection
    gRPC::grpc++
)

target_link_libraries(main 
    sqlite_orm::sqlite_orm
    SQLite::SQLite3
)

target_link_libraries(main spdlog::spdlog)
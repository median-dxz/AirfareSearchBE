project(airfare_search_main)

get_filename_component(rpc_proto "./protos/SearchRequest.proto" ABSOLUTE)
get_filename_component(rpc_proto_path "${rpc_proto}" PATH)

add_executable(main main.cpp)

target_sources(main PRIVATE data_builder.cpp data_builder.h)

# add proto_gen
file(GLOB_RECURSE rpc_server_gen
    "${rpc_proto_path}/*.cc"
    "${rpc_proto_path}/*.h"
)

add_library(grpc_proto ${rpc_server_gen})

target_link_libraries(grpc_proto
    protobuf::libprotobuf
    gRPC::grpc++_reflection
    gRPC::grpc++
)

target_sources(main PRIVATE ${rpc_server_gen})

# add database
file(GLOB_RECURSE database_source
    "${DIR_MODULE_DATABASE}/*.cpp"
    "${DIR_MODULE_DATABASE}/*.h"
)
target_compile_definitions(main PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)
target_sources(main PRIVATE ${database_source})
target_include_directories(main PRIVATE ${DIR_MODULE_DATABASE})

# add search-service
file(GLOB_RECURSE search_service_source
    "${DIR_MODULE_SEARCH_SERVICE}/*.cpp"
    "${DIR_MODULE_SEARCH_SERVICE}/*.h"
)
target_sources(main PRIVATE ${search_service_source})
target_include_directories(main PRIVATE ${DIR_MODULE_SEARCH_SERVICE})

target_link_libraries(main 
    grpc_proto
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
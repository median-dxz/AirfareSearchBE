project(airfare_search_main)

get_filename_component(rpc_proto "./protos/SearchRequest.proto" ABSOLUTE)
get_filename_component(rpc_proto_path "${rpc_proto}" PATH)

add_executable(main main.cpp data_builder.cpp data_builder.h ${utils_sources})
target_include_directories(main PRIVATE ${rpc_proto_path} ${DIR_MODULE_DATABASE} ${DIR_MODULE_SEARCH_SERVICE})

# add grpc_proto
file(GLOB_RECURSE grpc_gen_source "${rpc_proto_path}/*.cc")
file(GLOB_RECURSE grpc_gen_header "${rpc_proto_path}/*.h")

add_library(grpc_proto ${grpc_gen_source} ${grpc_gen_header})

# add search-service
add_library(search_service ${service_sources})
target_include_directories(search_service PRIVATE ${DIR_MODULE_DATABASE})

target_compile_definitions(search_service PRIVATE SQLITE_ORM_OPTIONAL_SUPPORTED=1)
target_sources(search_service PRIVATE ${database_sources} ${utils_sources})

# link shared lib
target_link_libraries(search_service
    sqlite_orm::sqlite_orm
    SQLite::SQLite3
    spdlog::spdlog
)

target_link_libraries(grpc_proto
    protobuf::libprotobuf
    gRPC::grpc++_reflection
    gRPC::grpc++
)

# link main
target_link_libraries(main 
    search_service
    grpc_proto
)

target_link_libraries(main 
    spdlog::spdlog
    absl::flags
    absl::flags_parse
    protobuf::libprotobuf
    gRPC::grpc++_reflection
    gRPC::grpc++
)

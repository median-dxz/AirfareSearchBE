project(rpc)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# 获取proto文件
get_filename_component(rpc_proto "./protos/SearchRequest.proto" ABSOLUTE)
get_filename_component(rpc_proto_path "${rpc_proto}" PATH)
set(protos_dir "./protos")

add_executable(main main.cpp 
    ${protos_dir}/SearchRequest.grpc.pb.cc
    ${protos_dir}/SearchRequest.grpc.pb.h
    ${protos_dir}/SearchRequest.pb.cc
    ${protos_dir}/SearchRequest.pb.h
)

set_target_properties(main PROPERTIES CMAKE_CXX_STANDARD 17)

target_link_libraries(main 
    absl::flags
    absl::flags_parse
    protobuf::libprotobuf
    gRPC::grpc++_reflection
    gRPC::grpc++
)

include("../database/CMakeLists.txt")
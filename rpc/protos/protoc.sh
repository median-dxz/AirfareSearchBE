export PATH=$PATH:/usr/local/vcpkg/installed/x64-linux/tools/protobuf:/usr/local/vcpkg/installed/x64-linux/tools/grpc
protoc --grpc_out . --cpp_out . -I . --plugin=protoc-gen-grpc="/usr/local/vcpkg/installed/x64-linux/tools/grpc/grpc_cpp_plugin" SearchRequest.proto
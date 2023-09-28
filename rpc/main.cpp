#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpc/support/log.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <fmt/std.h>
#include <spdlog/spdlog.h>

#include "protos/SearchRequest.grpc.pb.h"

#include "data_builder.h"
#include "database.h"
#include "service.h"
#include "thread_pool.h"

namespace fs = std::filesystem;

const string DEFAULT_DB_FILE = fs::absolute(fs::path("/var") / "as_data" / "as.db");

ABSL_FLAG(uint16_t, port, 3368, "Server port for the service");
ABSL_FLAG(std::string, db_file, DEFAULT_DB_FILE, "Path of database for server");

using grpc::CallbackServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerUnaryReactor;
using grpc::Status;

using AirfareSearch::FlightsSearchService;
using AirfareSearch::SearchRequest;
using AirfareSearch::SearchResponse;

using std::string;
namespace fs = std::filesystem;

class FlightsSearchServiceImpl final : public AirfareSearch::FlightsSearchService::CallbackService {
    ServerUnaryReactor *search(CallbackServerContext *context, const SearchRequest *request,
                               SearchResponse *reply) override {

        spdlog::info("[main server #{}]: handle a rpc call", std::this_thread::get_id());
        auto &ins = SearchServiceImpl::getInstance();
        DataBuilder::bindResponse(reply, ins.search(DataBuilder::request(*request)));

        ServerUnaryReactor *reactor = context->DefaultReactor();
        reactor->Finish(Status::OK);
        return reactor;
    }
};

void RunServer(uint16_t port, string &&db_file) {
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
    FlightsSearchServiceImpl service;
    // 初始化数据库
    Database::getStorage(db_file);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    spdlog::info("Server listening on {}", server_address);

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    FlightsSearchServiceImpl server;

    RunServer(absl::GetFlag(FLAGS_port), absl::GetFlag(FLAGS_db_file));

    return 0;
}
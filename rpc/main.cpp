#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <spdlog/spdlog.h>

#include "protos/SearchRequest.grpc.pb.h"

#include "data_builder.h"
#include "database.h"
#include "service.h"

namespace fs = std::filesystem;

const string DEFAULT_DB_FILE = fs::absolute(fs::path(getenv("HOME")) / "as_data" / "as.db");

ABSL_FLAG(uint16_t, port, 3368, "Server port for the service");
ABSL_FLAG(std::string, db_file, DEFAULT_DB_FILE, "Path of database for server");

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using AirfareSearch::FlightsSearchService;
using AirfareSearch::SearchRequest;
using AirfareSearch::SearchResponse;

using std::string;
namespace fs = std::filesystem;

class ServerImpl final {
  public:
    ~ServerImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

    // There is no shutdown handling in this code.
    void Run(uint16_t port, std::string db_file) {
        std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
        // 初始化数据库
        Database::getStroage(db_file);

        ServerBuilder builder;

        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);
        // Get hold of the completion queue used for the asynchronous communication
        // with the gRPC runtime.
        cq_ = builder.AddCompletionQueue();
        // Finally assemble the server.
        server_ = builder.BuildAndStart();

        std::cout << "Server listening on " << server_address << std::endl;

        // Proceed to the server's main loop.
        HandleRpcs();
    }

  private:
    // Class encompasing the state and logic needed to serve a request.
    class CallData {
      public:
        // Take in the "service" instance (in this case representing an asynchronous
        // server) and the completion queue "cq" used for asynchronous communication
        // with the gRPC runtime.
        CallData(FlightsSearchService::AsyncService *service, ServerCompletionQueue *cq)
            : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
            // Invoke the serving logic right away.
            Proceed();
        }

        void Proceed() {
            if (status_ == CREATE) {
                // Make this instance progress to the PROCESS state.
                status_ = PROCESS;

                // As part of the initial CREATE state, we *request* that the system
                // start processing SayHello requests. In this request, "this" acts are
                // the tag uniquely identifying the request (so that different CallData
                // instances can serve different requests concurrently), in this case
                // the memory address of this CallData instance.
                service_->Requestsearch(&ctx_, &request_, &responder_, cq_, cq_, this);
            } else if (status_ == PROCESS) {
                // Spawn a new CallData instance to serve new clients while we process
                // the one for this CallData. The instance will deallocate itself as
                // part of its FINISH state.
                new CallData(service_, cq_);
                spdlog::info("[main server]: handle a rpc call");

                // The actual processing.
                auto &ins = AirfareSearch::DataBuilder::getInstance();
                auto reponse = AirfareSearch::search(ins.request(this->request_));

                ins.bindResponse(this->reply_, reponse);
                spdlog::info("[main server]: finish handling");
                // And we are done! Let the gRPC runtime know we've finished, using the
                // memory address of this instance as the uniquely identifying tag for
                // the event.
                status_ = FINISH;
                responder_.Finish(reply_, Status::OK, this);
            } else {
                GPR_ASSERT(status_ == FINISH);
                // Once in the FINISH state, deallocate ourselves (CallData).
                delete this;
            }
        }

      private:
        // The means of communication with the gRPC runtime for an asynchronous
        // server.
        FlightsSearchService::AsyncService *service_;
        // The producer-consumer queue where for asynchronous server notifications.
        ServerCompletionQueue *cq_;
        // Context for the rpc, allowing to tweak aspects of it such as the use
        // of compression, authentication, as well as to send metadata back to the
        // client.
        ServerContext ctx_;

        // What we get from the client.
        SearchRequest request_;
        // What we send back to the client.
        SearchResponse reply_;

        // The means to get back to the client.
        ServerAsyncResponseWriter<SearchResponse> responder_;

        // Let's implement a tiny state machine with the following states.
        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_; // The current serving state.
    };

    // This can be run in multiple threads if needed.
    void HandleRpcs() {
        // Spawn a new CallData instance to serve new clients.
        new CallData(&service_, cq_.get());
        void *tag; // uniquely identifies a request.
        bool ok;
        while (true) {
            // Block waiting to read the next event from the completion queue. The
            // event is uniquely identified by its tag, which in this case is the
            // memory address of a CallData instance.
            // The return value of Next should always be checked. This return value
            // tells us whether there is any kind of event or cq_ is shutting down.
            GPR_ASSERT(cq_->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData *>(tag)->Proceed();
        }
    }

    std::unique_ptr<ServerCompletionQueue> cq_;
    FlightsSearchService::AsyncService service_;
    std::unique_ptr<Server> server_;
};

int main(int argc, char **argv) {
    absl::ParseCommandLine(argc, argv);
    ServerImpl server;

    server.Run(absl::GetFlag(FLAGS_port), absl::GetFlag(FLAGS_db_file));

    return 0;
}
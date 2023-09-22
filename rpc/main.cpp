#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "./protos/SearchRequest.grpc.pb.h"

ABSL_FLAG(uint16_t, port, 3368, "Server port for the service");

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using AirfareSearch::FlightsSearchService;
using AirfareSearch::SearchRequest;
using AirfareSearch::SearchResponse;

class ServerImpl final {
  public:
    ~ServerImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

    // There is no shutdown handling in this code.
    void Run(uint16_t port) {
        std::string server_address = absl::StrFormat("0.0.0.0:%d", port);

        ServerBuilder builder;
        // Listen on the given address without any authentication mechanism.
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        // Register "service_" as the instance through which we'll communicate with
        // clients. In this case it corresponds to an *asynchronous* service.
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

                // The actual processing.
                std::string prefix("Hello ");

                auto r1 = reply_.add_data();
                auto r2 = reply_.add_data();

                r1->add_agencies("BJS001");
                r1->add_agencies("CAN001");

                auto f1 = r1->add_flights();
                auto f2 = r1->add_flights();

                r1->set_price(3200);

                AirfareSearch::City c1;
                AirfareSearch::City c2;

                c1.set_code("AAA");
                c1.set_name("中国");

                c2.set_code("AAC");
                c2.set_code("BBB");

                f1->set_allocated_arrival(&c1);
                f1->set_arrivaldatetime("111122334455");
                f1->set_allocated_departure(&c2);
                f1->set_departuredatetime("111122334455");
                f1->set_carrier("AA");
                f1->set_flightno("24154");
                f1->add_cabins(AirfareSearch::Cabin::F);
                f1->add_cabins(AirfareSearch::Cabin::Y);

                f2->set_allocated_arrival(&c1);
                f2->set_arrivaldatetime("111122334455");
                f2->set_allocated_departure(&c2);
                f2->set_departuredatetime("111122334455");
                f2->set_carrier("AA");
                f2->set_flightno("24154");
                f2->add_cabins(AirfareSearch::Cabin::F);
                f2->add_cabins(AirfareSearch::Cabin::Y);

                r2->add_agencies("BJS001");
                r2->add_agencies("CAN001");

                auto f3 = r2->add_flights();
                f3->set_allocated_arrival(&c1);
                f3->set_arrivaldatetime("111122334455");
                f3->set_allocated_departure(&c2);
                f3->set_departuredatetime("111122334455");
                f3->set_carrier("AA");
                f3->set_flightno("24154");
                f3->add_cabins(AirfareSearch::Cabin::F);
                f3->add_cabins(AirfareSearch::Cabin::Y);

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
    server.Run(absl::GetFlag(FLAGS_port));

    return 0;
}
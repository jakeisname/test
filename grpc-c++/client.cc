#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "pon.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using pon::Onu;
using pon::PonReply;
using pon::PonService;
using pon::Empty;

class PonClient {
public:
    PonClient(std::shared_ptr<Channel> channel)
        : stub_(PonService::NewStub(channel)) {}

    std::string OnuActivate(const Onu& onu) {
        PonReply response;
        ClientContext context;

        Status status = stub_->OnuActivate(&context, onu, &response);
        if (status.ok()) {
            return response.message();
        } else {
            std::cout << "Failed to call OnuActivate: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

    std::string OnuDeactivate(const Onu& onu) {
        Empty response;
        ClientContext context;

        Status status = stub_->OnuDeactivate(&context, onu, &response);
        if (status.ok()) {
            return "ok";
        } else {
            std::cout << "Failed to call OnuDeactivate: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

    void OnuIndication(const Onu& onu) {
        ClientContext context;
        std::unique_ptr<grpc::ClientReader<Onu>> reader(stub_->OnuIndication(&context, onu));

        Onu response;
        while (reader->Read(&response)) {
            std::cout << "Received OnuIndication: Portid " << response.portid()
                      << ", Onuid " << response.onuid() << std::endl;
        }

        Status status = reader->Finish();
        if (!status.ok()) {
            std::cout << "Failed to receive stream response: " << status.error_code()
                      << ": " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<PonService::Stub> stub_;
};

int main(int argc, char **argv) {
    char *host = "172.18.0.9:3000"; // for c

    if (argc < 2) {
	printf("usage) client <ip:port>\n");
    } else {
	host = argv[1];
    }

    std::shared_ptr<Channel> channel = grpc::CreateChannel(host, 
	    grpc::InsecureChannelCredentials());
    PonClient client(channel);

    Onu onu;
    onu.set_portid(22);
    onu.set_onuid(33);

    std::string activateResponse = client.OnuActivate(onu);
    std::cout << "Received message: " << activateResponse << std::endl;

    std::string deactivateResponse = client.OnuDeactivate(onu);
    std::cout << "Received message: " << deactivateResponse << std::endl;

    client.OnuIndication(onu);

    return 0;
}



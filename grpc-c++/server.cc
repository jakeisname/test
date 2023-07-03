#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <random>

#include <grpcpp/grpcpp.h>

#include "pon.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using pon::Onu;
using pon::PonReply;
using pon::PonService;
using pon::Empty;

class PonServiceImpl final : public PonService::Service {
    Status OnuIndication(ServerContext* context, 
const Onu* request, grpc::ServerWriter<Onu>* writer) override {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> portid_dist(0, 7);
	std::uniform_int_distribution<int> onuid_dist(0, 127);

	while (true) {
	    int portid = portid_dist(gen) + 1;
	    int onuid = onuid_dist(gen) + 1;

	    Onu response;
	    response.set_portid(portid);
	    response.set_onuid(onuid);

	    writer->Write(response);

	    std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return Status::OK;
    }

    Status OnuActivate(ServerContext* context, const Onu* request, PonReply* response) override {
	std::string message = "ok. portid=" + std::to_string(request->portid()) + 
	    ", onuid=" + std::to_string(request->onuid());
	response->set_message(message);
	return Status::OK;
    }

    Status OnuDeactivate(ServerContext* context, const Onu* request, Empty* response) override {
	return Status::OK;
    }
};

void RunServer(char *host) {
    std::string server_address(host);
    PonServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv) {
    if (argc < 2) {
	printf("usage) server <ip:port>\n");
	RunServer("172.18.0.7:4000");
    } else {
	RunServer(argv[1]);
    }

    return 0;
}


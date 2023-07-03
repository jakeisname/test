
CXX = g++
PROTOC = protoc

CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
	-pthread\
	-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
	-ldl

all: server client

server: pon.pb.o pon.grpc.pb.o server.o
	$(CXX) $^ $(LDFLAGS) -o $@

client: pon.pb.o pon.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@

pon.pb.cc: pon.proto
	$(PROTOC) -I=. --cpp_out=. $<

pon.pb.h: pon.pb.cc

pon.grpc.pb.cc: pon.proto
	$(PROTOC) -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` $<

pon.grpc.pb.h: pon.grpc.pb.cc

clean:
	rm -f server client pon.pb.* pon.grpc.pb.* *.o


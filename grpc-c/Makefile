CC = gcc
PROTOC = protoc

LDFLAGS += -L/usr/local/lib -lgrpc -lgrpc-c -lprotobuf-c -lgpr -lpthread
CFLAGS += -O0 -g -I/root/workspace/grpc-c/lib/h -I/root/workspace/grpc-c/third_party/protobuf-c

all: proto server client

pon.grpc-c.h: pon.grpc-c.c pon.grpc-c.service.c

server: pon.grpc-c.o pon.grpc-c.service.o server.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

client: pon.grpc-c.o client.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

test: test.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

proto:
	$(PROTOC) -I=. --grpc-c_out=. --plugin=protoc-gen-grpc-c=`which protoc-gen-grpc-c` pon.proto

clean:
	rm -f server client test pon.grpc-c.* *.o


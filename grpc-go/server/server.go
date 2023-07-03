package main

import (
	"context"
	"fmt"
	"log"
	"math/rand"
	"net"
	"os"
	"os/signal"
	"time"

	"google.golang.org/grpc"

	pb "stream/pon"
)

type server struct {
	pb.UnimplementedPonServiceServer
}

func (s *server) OnuIndication(request *pb.Onu, stream pb.PonService_OnuIndicationServer) error {
	for {
		portid := rand.Int31n(8)
		onuid := rand.Int31n(128)

		response := &pb.Onu{
			Portid: portid + 1,
			Onuid:  onuid + 1,
		}

		err := stream.Send(response)
		if err != nil {
			return err
		}

		time.Sleep(1 * time.Second)
	}
}

func (s *server) OnuActivate(ctx context.Context, in *pb.Onu) (*pb.PonReply, error) {
	message := fmt.Sprintf("ok. portid=%d, onuid=%d", in.Portid, in.Onuid)
	return &pb.PonReply{Message: message}, nil
}

func (s *server) OnuDeactivate(ctx context.Context, in *pb.Onu) (*pb.Empty, error) {
	return &pb.Empty{}, nil
}

func main() {
	argCount := len(os.Args)
	host := "172.18.0.8:5000" // for go
	if argCount < 2 {
		fmt.Println("usage) server <ip:port>")
	} else {
		host = os.Args[1]
	}

	fmt.Printf("Server listening on %s\n", host)
	lis, err := net.Listen("tcp", host)
	if err != nil {
		log.Fatalf("Failed to listen: %v", err)
	}

	s := grpc.NewServer()
	pb.RegisterPonServiceServer(s, &server{})

	go func() {
		if err := s.Serve(lis); err != nil {
			log.Fatalf("Failed to serve: %v", err)
		}
	}()

	stop := make(chan os.Signal, 1)
	signal.Notify(stop, os.Interrupt)
	<-stop

	fmt.Println("Shutting down gRPC server...")
	s.GracefulStop()
	fmt.Println("gRPC server stopped.")
}

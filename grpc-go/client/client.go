package main

import (
	"context"
	"fmt"
	"io"
	"log"
	"os"

	"google.golang.org/grpc"

	pb "stream/pon"
)

func main() {
	argCount := len(os.Args)
	host := "172.18.0.8:5000" // for go
	if argCount < 2 {
		fmt.Println("usage) client <ip:port>")
		fmt.Printf("try to %s\n", host)
	} else {
		host = os.Args[1]
	}

	conn, err := grpc.Dial(host, grpc.WithInsecure())
	if err != nil {
		log.Fatalf("Failed to connect to server: %v", err)
	}
	defer conn.Close()

	client := pb.NewPonServiceClient(conn)

	onu := &pb.Onu{
		Portid: 22,
		Onuid:  33,
	}

	response, err := client.OnuActivate(context.Background(), onu)
	if err != nil {
		log.Fatalf("Failed to call OnuActivate: %v", err)
	}
	fmt.Println("Received message:", response.Message)

	_, err2 := client.OnuDeactivate(context.Background(), onu)
	if err2 != nil {
		log.Fatalf("Failed to call OnuActivate: %v", err2)
	}
	fmt.Println("Received message: ok")

	stream, err3 := client.OnuIndication(context.Background(), onu)
	if err3 != nil {
		log.Fatalf("Failed to call OnuIndication: %v", err3)
	}
	fmt.Println("Received message:", response.Message)

	for {
		onu, err := stream.Recv()
		if err == io.EOF {
			break
		}
		if err != nil {
			log.Fatalf("Failed to receive stream response: %v", err)
		}

		fmt.Printf("Received OnuIndication: Portid %d, Onuid %d\n",
			onu.Portid, onu.Onuid)
	}
}

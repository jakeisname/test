import sys
import time
import signal
import random
import grpc
import pon_pb2_grpc
from pon_pb2 import Onu, PonReply, Empty
from concurrent import futures

class PonServiceServicer(pon_pb2_grpc.PonServiceServicer):
    def OnuIndication(self, request, context):
        while True:
            portid = random.randint(0, 7)
            onuid = random.randint(0, 127)

            response = Onu(
                portid=portid+1,
                onuid=onuid+1
            )

            yield response
            time.sleep(1)

    def OnuActivate(self, request, context):
        message = f"ok. portid={request.portid}, onuid={request.onuid}"
        return PonReply(message=message)

    def OnuDeactivate(self, request, context):
        return Empty()

def main():
    arg_count = len(sys.argv)
    host = "172.18.0.6:6000"

    if arg_count < 2:
        print("usage) server <ip:port>")
    else:
        host = sys.argv[1]

    print("Server listening on ", host)

    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    pon_pb2_grpc.add_PonServiceServicer_to_server(PonServiceServicer(), server)
    server.add_insecure_port(host)
    server.start()

    print("gRPC server started.")

    try:
        signal.pause()
    except KeyboardInterrupt:
        print("Shutting down gRPC server...")
        server.stop(0)
        print("gRPC server stopped.")

if __name__ == '__main__':
    main()


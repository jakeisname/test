import sys
import grpc
from pon_pb2 import Onu
from pon_pb2 import Empty
from pon_pb2_grpc import PonServiceStub

def main():
    arg_count = len(sys.argv)
    host = "172.18.0.6:6000"

    if arg_count < 2:
        print("usage) client <ip:port>")
    else:
        host = sys.argv[1]

    print("try to connect ", host)

    channel = grpc.insecure_channel(host)
    stub = PonServiceStub(channel)

    onu = Onu(portid=22, onuid=33)

    try:
        response = stub.OnuActivate(onu)
        print("Received message:", response.message)

        stub.OnuDeactivate(onu)
        print("Received message: ok")

        stream = stub.OnuIndication(onu)
        for onu in stream:
            print(f"Received OnuIndication: Portid {onu.portid}, Onuid {onu.onuid}")
    except grpc.RpcError as e:
        print(f"RPC Error: {e}")

if __name__ == '__main__':
    main()


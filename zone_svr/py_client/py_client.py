import sys
import socket
import struct
import ctypes
import zonesvr_pb2

class MsgHead :
    def __init__(self, len = 0, type = 0):
        self.len = len
        self.type = type

    def ParseFromString(self, data):
        self.len, self.type = self.__format__.unpack(data)

    def SerializeToString(self):
        return self.__format__.pack(self.len, self.type)

    def Size(self):
        return self.__format__.size

    __format__ = struct.Struct("!II")
    len = 0
    type = 0

class Cmd:
    def ParseFromString(self, data):
        head = MsgHead()

        if len(data) < head.Size():
            return -1

        head.ParseFromString(data[:head.Size()])

        if len(data) < head.len:
            return -1

        self.type = head.type
        self.data = data[head.Size():head.len]

        return head.len

    type = 0
    data = ''

if len(sys.argv) < 3:
    print('exe ip port')
    sys.exit(0)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

HOST = sys.argv[1]
PORT = int(sys.argv[2])
client.connect((HOST, PORT))

login_req = zonesvr_pb2.LoginReq()
login_req.name = "czl"

data = login_req.SerializeToString()

head = MsgHead()
head.len = head.Size() + len(data)
head.type = zonesvr_pb2.MsgCmd.LOGIN_REQ

headstr = head.SerializeToString()

client.sendall(headstr + data)

rvstr = client.recv(1024)

print len(rvstr)

cmd = Cmd()

cmd.ParseFromString(rvstr)

print cmd.type

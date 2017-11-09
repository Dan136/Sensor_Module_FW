import socket
import time
HOST = '192.168.1.100'    # The remote host
PORT = 80              # The same port as used by the server
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while (True):
        #s.sendall(b'Hello, dan is cool')
        data = s.recv(1024)
        print('Received', repr(data))
        time.sleep(2)

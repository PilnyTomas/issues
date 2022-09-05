import socket
import os

AUDIO_FILE = "./sound.wav"

def loadFile():
    with open(AUDIO_FILE, 'r+b') as file:
        file.seek(0, os.SEEK_END)
        size = file.tell()
        file.seek(0, os.SEEK_SET)
        print("Filesize is", size)
        fileData = file.read()
    return fileData, size
    pass

if __name__ == '__main__':
    # Set up a TCP/IP server
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    tcp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
    tcp_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPIDLE, 1)
    tcp_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPINTVL, 3)
    tcp_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPCNT, 2)
    
    server_address = ('0.0.0.0', 8115)
    tcp_socket.bind(server_address)
    tcp_socket.listen(1)
    
    fileData, sizeOfFile = loadFile()
    
    while True:
        print("Waiting for connection")
        connection, client = tcp_socket.accept()
     
        try:
            print("Connected to client IP: {}".format(client))
             
            # Receive and print data 2048 bytes at a time, as long as the client is sending something
            while True:
                buffer = connection.recv(2048)
                binary_format = bytearray(buffer)
                print("Received data with input:", binary_format[0])
                if 0 == binary_format[0]:
                    packet = str(sizeOfFile).encode()
                    print(packet)
                    connection.sendall(bytes(packet))
                else:
                    writeRet = connection.sendall(bytearray(fileData))
        except Exception as e:
            print("Exception thrown:", type(e).__name__)
            raise e
        finally:
            connection.close()
    
    pass

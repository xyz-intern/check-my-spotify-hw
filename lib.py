import requests
import itertools
import threading
import serial
import socket

def send_to_nest(command):
    if command == None:
        pass
    url_items = f'http://192.168.0.133:3000/apis/command/{command}'
    print('req command')
    response = requests.get(url_items)
    return response.text

def set_socket(
        IP='192.168.0.105',
        PORT=8765,
        SIZE=1024,
    ):
    ADDR = (IP, PORT)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind(ADDR)

def listening_socket():
    server_socket.listen(5)
    client_socket, client_addr = server_socket.accept()  # 수신대기, 접속한 클라이언트 정보 (소켓, 주소) 반환
    msg = client_socket.recv(SIZE)  # 클라이언트가 보낸 메시지 반환
    print("[{}] message : {}".format(client_addr,msg))  # 클라이언트가 보낸 메시지 출력

    client_socket.sendall("welcome!".encode())  # 클라이언트에게 응답

    client_socket.close()
    serverSocket.close()
    print('socket is close')
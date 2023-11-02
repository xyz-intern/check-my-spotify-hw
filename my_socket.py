import socket
class My_socket:

    def __init__(self,IP='192.168.0.105',PORT=8765,SIZE=1024):
        ADDR = (IP,PORT)
        self.msg_size = SIZE
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind(ADDR)
        self.server_socket.listen(5)
        self.client_socket, self.client_addr = self.server_socket.accept()  # 수신대기, 접속한 클라이언트 정보 (소켓, 주소) 반환
    
    def listening_socket(self):
        msg = self.client_socket.recv(self.msg_size)  # 클라이언트가 보낸 메시지 반환
        
        print("[{}] message : {}".format(self.client_addr,msg))  # 클라이언트가 보낸 메시지 출력

        self.client_socket.sendall("welcome!".encode())  # 클라이언트에게 응답

    def close_socket(self):
        self.client_socket.close()
        self.serverSocket.close()
        print('socket is close')
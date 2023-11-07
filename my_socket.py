import socket
import lib
from timer_manager import Timer_Manager

timer_manager = Timer_Manager()

class My_socket:

    def __init__(self,IP='192.168.0.105',PORT=8765,SIZE=1024):
        ADDR = (IP,PORT)
        self.msg_size = SIZE
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind(ADDR)
        self.server_socket.listen(5)
    
    def listening_socket(self):
        while True:
            # 수신대기, 접속한 클라이언트 정보 (소켓, 주소) 반환
            self.client_socket, self.client_addr = self.server_socket.accept()
            
            # 클라이언트가 보낸 메시지 반환
            msg = self.client_socket.recv(self.msg_size)

            # 클라이언트가 보낸 메시지 출력
            print("[{}] socket message : {}".format(self.client_addr,msg))

            msg = msg.decode('utf-8')
            
            if msg.isdigit(): # is number ?
                write_type = 'duration'
                lib.write_data_in_file(txt=msg,write_type=write_type)
                duration_time = lib.read_file_data(write_type=write_type)
                timer_manager.set_interval(duration_time)
            else :
                write_type = 'user'
                lib.write_data_in_file(txt=msg,write_type=write_type)
                timer_manager.cancle_timer()

            self.client_socket.sendall("good socket communication!".encode())
            self.client_socket.close()
            print("==============socket============")

    def close_socket(self):
        self.server_socket.close()
        print('socket is close')
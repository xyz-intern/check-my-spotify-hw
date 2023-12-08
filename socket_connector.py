import socket
from lcd import write_data, read_data

class Socket:
    def __init__(self,IP,PORT,SIZE,timer_,serial_):
        ADDR = (IP,PORT)
        self.msg_size = SIZE
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind(ADDR)
        self.server_socket.listen(5)

        self.timer_manager = timer_
        self.my_serial = serial_
    
    def listening_socket(self):
        while True:
            # 수신대기, 접속한 클라이언트 정보 (소켓, 주소) 반환
            self.client_socket, self.client_addr = self.server_socket.accept()
            
            # 클라이언트가 보낸 메시지 반환
            msg = self.client_socket.recv(self.msg_size)

            # 클라이언트가 보낸 메시지 출력
            print("[{}] socket message : {}".format(self.client_addr,msg))

            msg = msg.decode('utf-8')
            msgs = msg.split('|')

            if len(msgs) > 1:
                write_type = 'duration'
                write_data(txt=msg,write_type=write_type)
                duration_time = read_data(write_type=write_type)
                times = duration_time.split('|')
                interval_time = (int(times[0]))
                self.timer_manager.set_interval(interval_time)
                self.my_serial.send_to_stm(f'duration|{int(times[1])-int(times[0])}|{times[1]}')
            else :
                write_type = 'user'
                write_data(txt=msg,write_type=write_type)

            self.client_socket.sendall("good socket communication!".encode())
            self.client_socket.close()

    def close_socket(self):
        self.server_socket.close()
        print('socket is close')
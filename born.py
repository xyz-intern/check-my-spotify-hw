import requests
import json
import threading
import serial
import socket
import urllib.request
import time



def write_data_in_file(txt,write_type):

    if write_type == 'user':
        file_url = './txts/user.txt'

    elif write_type == 'listening':
        file_url = './txts/listening_info.txt'
    
    elif write_type == 'duration' :
        file_url = './txts/duration.txt'
    
    data = txt
    f = open(file_url, 'w')
    f.write(data)
    f.close()

def read_file_data(write_type):

    if write_type == 'user':
        file_url = './txts/user.txt'

    elif write_type == 'listening':
        file_url = './txts/listening_info.txt'
    
    else :
        file_url = './txts/duration.txt'
    
    f = open(file_url,'r')
    file_data = f.read()
    f.close()
    return file_data

class Serial_controller:

    def __init__(self, baudrate=115200, port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00"):
        # Uart communication settings
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port

        # Open
        self.ser.open()

    def send_to_stm(self,data):
        send_data = str(data)
        send_data_bytes = send_data.encode('utf-8')
        self.ser.write(send_data_bytes)
        print(f'send to stm32 : {send_data_bytes}')

class Timer_Manager:
    def __init__(self):
        self.t = None
    
    def timer_get_track_info(self,user_id):

        url_items = f'http://192.168.0.133:3000/apis/getTrack/{user_id}'
        headers = {
            "User-Agent" : "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0"
        }
        response = requests.get(url_items,headers=headers,timeout=5)

        # timer_serial = Serial_controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D71326E5652-if00")
        # timer_serial.send_to_stm(response.text)

        write_data_in_file(txt=response.text,write_type='listening')
        return response.text
        # stm32 로 보내야함

    def set_interval(self,how_long):
        
        redirection_time = int(how_long) / 1000.0
        print(f'redirection time : {redirection_time}')
        user_id = read_file_data('user')
        
        self.t = threading.Timer(redirection_time,self.timer_get_track_info,args=(user_id,))
        self.t.start()
    
    def cancle_timer(self):
        if self.t is not None: # timer is alive ?
            self.t.cancel() # timer cancle

my_serial = Serial_controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D71326E5652-if00")
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
                write_data_in_file(txt=msg,write_type=write_type)
                duration_time = read_file_data(write_type=write_type)
                timer_manager.set_interval(duration_time)
            else :
                write_type = 'user'
                write_data_in_file(txt=msg,write_type=write_type)

            self.client_socket.sendall("good socket communication!".encode())
            self.client_socket.close()

    def close_socket(self):
        self.server_socket.close()
        print('socket is close')



def english_to_korean(papago_quote):
    encText = urllib.parse.quote(papago_quote)
    data = "source=ko&target=en&text=" + encText
    url = "https://openapi.naver.com/v1/papago/n2mt"
    request = urllib.request.Request(url)
    request.add_header("X-Naver-Client-Id",client_id)
    request.add_header("X-Naver-Client-Secret",client_secret)
    response = urllib.request.urlopen(request, data=data.encode("utf-8"))
    rescode = response.getcode()
    if(rescode==200):
        response_body = response.read()
        print(response_body.decode('utf-8'))
        data = json.loads(response_body)
        translated_text = data['message']['result']['translatedText']

    else:
        print("Error Code:" + rescode)

    return str(translated_text)

def settup_lcd_data(data):
    
    # vairable
    response_list_zero = False
    response_list_one  = False
    result_str = ''

    # data split
    response_list = data.split('|')

    for ch in response_list[0]:
        if '가' <= ch <= '힣':  # 한글의 유니코드 범위
            response_list_zero = True

    if response_list_zero:
        result_str += english_to_korean(response_list[0]) +'|'
    
    else:
        result_str += response_list[0]+'|'

    if response_list[1] is None:
        result_str += 'test'
    else :

        for ch in response_list[1]:
            if '가' <= ch <= '힣':  # 한글의 유니코드 범위
                response_list_one = True
            
        if response_list_one:
            result_str += english_to_korean(response_list[1])
        else:
            result_str += response_list[1]

    return result_str

def send_to_nest(command,user_id):

    if command == None:
        pass

    # volume command
    if command == '0' or command == '1':

        url_items = f'http://192.168.0.133:3000/apis/volume/'
        
        if command == '0':
            data = {'volume': False, 'userId': user_id}
        
        else :
            data = {'volume': True , 'userId': user_id}

        response = requests.post(url_items, json=data)
        return response.text
    
    else:

        # requests variables
        # url
        url_items = f'http://192.168.0.133:3000/apis/command/'
        
        # data
        data = {'command': command, 'userId': user_id}
        
        # send
        response = requests.post(url_items, json=data,timeout=20)

        if command == 'play':
            return settup_lcd_data(response.text)

        elif command == 'stop':
            timer_manager.cancle_timer()
            return 'Music is|Stop!!!'
        
        elif command == 'prev' or command == 'next':
            timer_manager.cancle_timer()
            timer_manager.set_interval(int(read_file_data('duration')))
            time.sleep(2.5)
            data2 = timer_manager.timer_get_track_info(user_id=user_id)
            stm_data= settup_lcd_data(data2)
            return stm_data

def socket_process():
    my_socket = My_socket(IP='192.168.0.105',PORT=8765,SIZE=1024)
    my_socket.listening_socket()

def serial_to_stm32():

    command_sent = False

    while True:

        # read serial data
        rcv_serial_data = my_serial.ser.read(1)
        rcv_serial_data = int.from_bytes(rcv_serial_data, 'big')
        
        # get user_id
        user_id = read_file_data(write_type='user')

        # play command
        if rcv_serial_data == 0 and not command_sent:
            command = 'play'
            command_sent = True  # 요청을 보냄
        
        # stop command
        elif rcv_serial_data == 1 and not command_sent:
            command = 'stop'
            timer_manager.cancle_timer()
            command_sent = True  # 요청을 보냄
        
        # next command
        elif rcv_serial_data == 2 and not command_sent:
            command = 'next'
            command_sent = True  # 요청을 보냄
            pass
        
        # prev command
        elif rcv_serial_data == 3 and not command_sent:
            command = 'prev'
            command_sent = True  # 요청을 보냄
        
        # volume down command
        elif rcv_serial_data == 4 and not command_sent:
            command = '0'
            command_sent = True  # 요청을 보냄
        
        # volume up command
        elif rcv_serial_data == 5 and not command_sent:
            command = '1'
            command_sent = True  # 요청을 보냄

        else:
            command = None

        if command is not None:

            response_data = send_to_nest(command, user_id)
            my_serial.send_to_stm(response_data)
            print(f'serial btn response data : {response_data}')
            
            # init
            command = None
            command_sent = False

thread_socket = threading.Thread(target=socket_process,name="socket thread")
thread_socket.daemon = True
thread_socket.start()

thread_serial = threading.Thread(target=serial_to_stm32,name="serial thread")
thread_serial.daemon = True
thread_serial.start()

while True:
    exit_signal = input('Type "e" anytime to stop server\n')
    if exit_signal == 'e':
        break
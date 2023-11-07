from serial_controller import Serial_controller
from my_socket import My_socket
import lib
import threading
import json
import time

my_serial = Serial_controller(baudrate=115200,
                              port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00"
                              )
my_socket = None

def socket_process():
    global my_socket
    my_socket = My_socket(IP='192.168.0.105',PORT=8765,SIZE=1024)
    my_socket.listening_socket()
    # my_socket.close_socket()

def serial_to_stm32():
    while True:
        rcv_serial_data = my_serial.ser.read(1)
        rcv_serial_data = int.from_bytes(rcv_serial_data,'big')
        user_id = lib.read_file_data(write_type='user')

        if rcv_serial_data == 0:
            command = 'play'

        elif rcv_serial_data == 1:
            command = 'stop'
        else :
            command = None

        response_data = lib.send_to_nest(command,user_id)
        
        print(f'2. serial btn response data : {response_data} | command : {command}')
        
        # if command is None:
        #     response_data = lib.send_to_nest(command, user_id)
        #     print(f'2. serial btn response data : {response_data}')

        #     if response_data == '음악이 정지되었습니다':
        #         print(f'3. response data = 음악이 정지되었습니다')
        #     else:
        #         if command == 'play':
        #             print("NPOOOOOONPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPppppp")
        #             my_serial.send_to_stm(response_data)
        #             command = None
        if response_data == '음악이 정지되었습니다':
            print(f'3. response data = 음악이 정지되었습니다')
        else:
            if command == 'play':
                my_serial.send_to_stm(response_data)
                command = None


thread_socket = threading.Thread(target=socket_process)
thread_socket.daemon = True
thread_socket.start()

thread_serial = threading.Thread(target=serial_to_stm32)
thread_serial.daemon = True
thread_serial.start()

while True:
    exit_signal = input('Type "e" or "EXIT" or "ㄷ턋" anytime to stop server\n')
    if exit_signal == 'e' or exit_signal == 'EXIT' or exit_signal == 'ㄷ턋':
        my_socket.close_socket()
        break
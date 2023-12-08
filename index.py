import requests
import threading
import time
from serial_connector import Serial
from timer_manager import TimerManager
from socket_connector import Socket
from lcd import read_data, send_to_nest

my_serial = Serial(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D71326E5652-if00")
timer_manager = TimerManager(my_serial)

def socket_process():
    my_socket = Socket(IP='192.168.0.105',PORT=8765,SIZE=1024,timer_=timer_manager,serial_=my_serial)
    my_socket.listening_socket()

def serial_to_stm32():

    command_sent = False

    while True:

        # read serial data
        rcv_serial_data = my_serial.ser.read(1)
        rcv_serial_data = int.from_bytes(rcv_serial_data, 'big')
        
        # get user_id
        user_id = read_data(write_type='user')

        if rcv_serial_data is not None and not command_sent:
            
            if rcv_serial_data >= 0 and rcv_serial_data <= 5:

                if rcv_serial_data == 0:    # play command
                    command = 'play'
                
                elif rcv_serial_data == 1:  # stop command
                    command = 'stop'
                    timer_manager.cancle_timer()

                elif rcv_serial_data == 2:  # next command
                    command = 'next'

                elif rcv_serial_data == 3:  # prev command
                    command = 'prev'

                elif rcv_serial_data == 4:  # volume down command
                    command = '0'

                elif rcv_serial_data == 5:  # volume up command
                    command = '1'
                
                command_sent = True

            else:
                command = None
            
            rcv_serial_data = None

            if command is not None and command_sent:
                
                print(f'command : {command}, user_id : {user_id}')
                
                response_data = send_to_nest(command, user_id, timer_manager)

                if response_data == 'error':
                    print(f'error response_data structure : {response_data}')

                else :
                    time.sleep(1.5)
                    if command == 'play' or command == 'next' or command == 'prev':
                        my_serial.send_to_stm(f'song|{response_data}')
                    elif command == 'stop':
                        my_serial.send_to_stm(f'stop|{response_data}')
                    else:
                        my_serial.send_to_stm(f'volume|{response_data}')
                # init
                command = None
                command_sent = False
            
try:
    thread_socket = threading.Thread(target=socket_process,name="socket thread")
    thread_socket.daemon = True
    thread_socket.start()
except:
    print('socket is not connection')

try:
    thread_serial = threading.Thread(target=serial_to_stm32,name="serial thread")
    thread_serial.daemon = True
    thread_serial.start()
except:
    print('serial is not connection')

while True:
    exit_signal = input('Type "e" anytime to stop server\n')
    if exit_signal == 'e' or 'exit' or 'EXIT':
        break
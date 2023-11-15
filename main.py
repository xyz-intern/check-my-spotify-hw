from serial_controller import Serial_controller
from my_socket import My_socket
import lib
import threading

my_serial = Serial_controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D71326E5652-if00")

def socket_process():
    my_socket = My_socket(IP='192.168.0.105',PORT=8765,SIZE=1024)
    my_socket.listening_socket()
    # my_socket.close_socket()

def send_stm(data):
    my_serial.send_to_stm(data)

def serial_to_stm32():
    command_sent = False
    while True:

        rcv_serial_data = my_serial.ser.read(1)
        rcv_serial_data = int.from_bytes(rcv_serial_data, 'big')
        
        user_id = lib.read_file_data(write_type='user')

        if rcv_serial_data == 0 and not command_sent:
            command = 'play'
            command_sent = True  # 요청을 보냄
        
        elif rcv_serial_data == 1 and not command_sent:
            command = 'stop'
            command_sent = True  # 요청을 보냄
        
        else:
            command = None

        if command is not None:
            response_data = lib.send_to_nest(command, user_id)
            print(f'serial btn response data : {response_data}')

            if response_data == '음악이 정지되었습니다':
                print(f'stop')

            else:
                if command == 'play':
                      my_serial.send_to_stm(response_data)
                      command = None
            
            command_sent = False  # 요청이 처리되었으므로 변수 초기화

thread_socket = threading.Thread(target=socket_process,name="socket thread")
thread_socket.daemon = True
thread_socket.start()

thread_serial = threading.Thread(target=serial_to_stm32,name="serial thread")
thread_serial.daemon = True
thread_serial.start()

while True:
    exit_signal = input('Type "exit" anytime to stop server\n')
    if exit_signal == 'exit':
        break
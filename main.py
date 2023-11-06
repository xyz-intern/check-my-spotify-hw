from serial_controller import Serial_controller
from my_socket import My_socket
import lib
import threading

my_serial = Serial_controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00")

def socket_process():
    my_socket = My_socket(IP='192.168.0.105',PORT=8765,SIZE=1024)
    my_socket.listening_socket()
    # my_socket.close_socket()

def serial_to_stm32():
    while True:
        rcv_serial_data = my_serial.ser.read(1)
        rcv_serial_data = int.from_bytes(rcv_serial_data,'big')
        
        print(rcv_serial_data)
        if rcv_serial_data == 0:
            command = 'play'
        elif rcv_serial_data == 1:
            command = 'stop'
        else :
            command = None
        
        user_id = lib.read_file_data(write_type='user')

        response_data = lib.send_to_nest(command,user_id)
        lib.write_data_in_file(txt=response_data,write_type='listening')
        print(f'serial btn response data : {response_data}')

thread_socket = threading.Thread(target=socket_process)
thread_socket.daemon = True
thread_socket.start()

thread_serial = threading.Thread(target=serial_to_stm32)
thread_serial.daemon = True
thread_serial.start()

while True:
    exit_signal = input('Type "exit" anytime to stop server\n')
    if exit_signal == 'exit':
        break
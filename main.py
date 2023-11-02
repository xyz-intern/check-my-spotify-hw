from serial_controller import Serial_controller
from my_socket import My_socket
from lib import *
my_serial = Serial_controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00")
my_socket = My_socket(IP='192.168.0.105',PORT=8765,SIZE=1024)
# my_socket.close_socket()

while True:
    try:
        rcv_serial_data = my_serial.ser.read(1)
        rcv_serial_data = int.from_bytes(rcv_serial_data,'big')
        print(rcv_serial_data)

        if rcv_serial_data == 0:
            command = 'play'
        elif rcv_serial_data == 1:
            command = 'stop'
        else :
            command = None

        response_data = send_to_nest(command)
        my_socket.listening_socket()
        
    except KeyboardInterrupt:
        my_socket.close_socket()
        break
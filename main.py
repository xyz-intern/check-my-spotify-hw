from serial_controller import Serial_controller
from my_socket import My_socket
import lib
import threading
import json

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
        print("============command==============")
        print(f'1. user id : {user_id}')

        if rcv_serial_data == 0:
            command = 'play'

        elif rcv_serial_data == 1:
            command = 'stop'
        else :
            command = None

        response_data = lib.send_to_nest(command,user_id)
        
        print(f'2. serial btn response data : {response_data}')
        
        # if response_data == '음악이 정지되었습니다':
        #     print(f'3. response data = 음악이 정지되었습니다')
        #     pass
        # else:
        #     # get track 쪽으로 내가 요청해서 데이터 받고
        #     lib.track_info(user_id=user_id)
        #     get_track_data = lib.read_file_data('listening')
        #     if get_track_data == None:
        #         print('get track data error')
        #     else :
        #         print(f'3. serial get track data : {get_track_data}')
        # print("=============command===========")
        
thread_socket = threading.Thread(target=socket_process)
thread_socket.daemon = True
thread_socket.start()

thread_serial = threading.Thread(target=serial_to_stm32)
thread_serial.daemon = True
thread_serial.start()

while True:
    exit_signal = input('Type "exit" or "EXIT" or "ㄷ턋" anytime to stop server\n')
    if exit_signal == 'exit' or exit_signal == 'EXIT' or exit_signal == 'ㄷ턋':
        my_socket.close_socket()
        break

'''
{
    "userId":"dlatldhs",
    "albumName":"Boys Like Girls",
    "artistName":"BOYS LIKE GIRLS",
    "songName":"The Great Escape",
    "imageUri":"https://i.scdn.co/image/ab67616d0000b273317ebb91078852577248ed98",
    "deviceId":"47ef08f937e0d53323e7d0ff9735ea8df181043e",
    "count":1
}
'''
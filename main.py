from my_spotify import Spotify_Controller
import requests

my_controller = Spotify_Controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00")

while True:
    try:
        rcv_data = my_controller.ser.read(1)
        rcv_data = int.from_bytes(rcv_data,'big')

        print(rcv_data)
        if rcv_data == 0:
            command='play'
        elif rcv_data == 1:
            command='stop'

        url_items = f'http://192.168.0.133:3000/apis/command/{command}'
        response = requests.get(url_items)
        print(response.text)
    except KeyboardInterrupt:
        break
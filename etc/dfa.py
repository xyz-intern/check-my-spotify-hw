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


    # import asyncio
# from websockets import ConnectionClosedError
# from websockets.server import serve
# import serial
# import requests

# class Spotify_Controller():
#     def __init__(self, baudrate, port):
#         self.ser = serial.Serial(port, baudrate)

# my_controller = Spotify_Controller(baudrate=115200, port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00")
    
# async def get_socket_data(websocket):
#     try:
#         async for message in websocket:
#             await websocket.send("Good Connection!!!")
#             my_controller.ser.write()
#             print(message)
#     except ConnectionClosedError:
#         print("Connection closed unexpectedly")

# async def serial_communication(controller):
#     while True:
#         try:
#             rcv_data = controller.ser.read(1)
#             rcv_data = int.from_bytes(rcv_data,'big')

#             print(rcv_data)
#             if rcv_data == 0:
#                 command='play'
#             elif rcv_data == 1:
#                 command='stop'

#             url_items = f'http://192.168.0.133:3000/apis/command/{command}'
#             response = requests.get(url_items)
#             print(response.text)
#         except KeyboardInterrupt:
#             break

# async def main():
#     my_controller = Spotify_Controller(baudrate=115200, port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00")
#     asyncio.create_task(serial_communication(my_controller))
#     async with serve(get_socket_data, "192.168.0.105", 8765):
#         await asyncio.Future()  # run forever

# asyncio.run(main())


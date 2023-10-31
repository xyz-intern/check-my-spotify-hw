import itertools
import threading

import serial

class Spotify_Controller:

    def __init__(self, baudrate=115200, port="/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A9EIFOED-if00-port0"):


        # Uart communication settings
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port

        # Open
        self.ser.open()
        
        # serial recive thread run !
        self.run_receive_thread()

    def run_receive_thread(self):
        task_receive = threading.Thread(target=self.__recv_data, name="serial_recv_task")
        task_receive.setDaemon(True)
        task_receive.start()

    def __recv_data(self):
        while True:
            data = bytearray(self.ser.read())
            print(data)
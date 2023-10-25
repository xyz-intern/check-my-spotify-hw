import itertools
import threading

import serial

class Spotify_Controller:
    def __init__(self, baudrate=115200, port="/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A9EIFOED-if00-port0"):
        self.seq = itertools.count()

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
    
    '''
    functions
    1. stop & start
    2. Volum Up & Down
    3. Next Music & Last Music
    '''

    def __send_data(self, command, data):
        seq = next(self.seq) % 0xFE + 1
        cmd = [0xFF, 0, seq, command, REQUEST]
        cmd.extend(data)
        cmd[1] = len(cmd) - 2
        checksum = sum(cmd) & 0xFF
        cmd.append(checksum)
        print(cmd)
        self.ser.write(cmd)
import serial
class Serial_controller:

    def __init__(self, baudrate=115200, port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00"):
        # Uart communication settings
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port

        self.start_byte = b'\x02'
        self.end_byte = b'\x03'

        # Open
        self.ser.open()
    
    def controll_disconnect(self):
        self.ser.close()
        print('serial is close')
    
    def __recv_data(self):
        pass

    def send_to_stm(self,data):
        send_data = str(data)
        send_data_bytes = self.start_byte + send_data.encode('utf-8') + self.end_byte
        self.ser.write(send_data_bytes)
        print(f'send to stm32 : {send_data_bytes}')
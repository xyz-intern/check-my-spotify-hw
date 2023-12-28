import serial

prefix = chr(6)  
suffix = chr(3)

class Serial:

    def __init__(self, baudrate=115200, port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D71326E5652-if00"):
        # Uart communication settings
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port

        # Open
        self.ser.open()

    def send_to_stm(self,data):
        send_data = prefix + str(data) + suffix
        send_data_bytes = send_data.encode('utf-8')

        self.ser.write(send_data_bytes)
        print(f'send to stm32 : {send_data_bytes}')
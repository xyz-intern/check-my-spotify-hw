import serial
class Serial_controller:

    def __init__(self, baudrate=115200, port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D7F439F5554-if00"):
        # Uart communication settings
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port

        # Open
        self.ser.open()
    
    def controll_disconnect(self):
        self.ser.close()
        print('serial is close')
    
    def __recv_data(self):
        pass    
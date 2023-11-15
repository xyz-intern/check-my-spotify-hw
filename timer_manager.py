import threading
import requests
import lib
from serial_controller import Serial_controller

class Timer_Manager:
    def __init__(self):
        self.t = None
    
    def get_track_info(self,user_id):
        url_items = f'http://192.168.0.133:3000/apis/getTrack/{user_id}'
        headers = {
            "User-Agent" : "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0"
        }
        response = requests.get(url_items,timeout=5,headers=headers)
        timer_serial = Serial_controller(baudrate=115200,port="/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_8D71326E5652-if00")
        timer_serial.send_to_stm(response.text)

        lib.write_data_in_file(txt=response.text,write_type='listening')

        # stm32 로 보내야함

    def set_interval(self,how_long):
        
        redirection_time = int(how_long) / 1000.0
        print(f'redirection time : {redirection_time}')
        user_id = lib.read_file_data('user')
        
        self.t = threading.Timer(redirection_time,self.get_track_info,args=(user_id,))
        self.t.start()
    
    def cancle_timer(self):
        if self.t is not None: # timer is alive ?
            self.t.cancel() # timer cancle
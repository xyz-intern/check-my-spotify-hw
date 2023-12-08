import time
import requests
import threading
from lcd import write_data, read_data, settup_lcd_data

class TimerManager:
    def __init__(self,serial_):
        self.t = None
        self.my_serial = serial_
    
    def timer_get_track_info(self,user_id):
        time.sleep(1.5)
        url_items = f'http://192.168.0.133:3000/apis/getTrack/{user_id}'
        headers = {
            "User-Agent" : "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0"
        }
        try:
            response = requests.get(url_items,headers=headers,timeout=30,verify=False)
        except:
            print('85 line: response error')

        write_data(txt=response.text,write_type='listening')
        
        return response.text
        # stm32 로 보내야함

    def set_interval_get_info(self,user_id):
        print('set interval get info excute!!!')
        data = self.timer_get_track_info(user_id=user_id)
        stm_data = settup_lcd_data(data)
        time.sleep(1.5)
        self.my_serial.send_to_stm(f'song|{stm_data}')

    def set_interval(self,how_long):
        if self.t is not None:
            self.t.cancel()

        redirection_time = int(how_long) / 1000.0
        print(f'set_interval function redirection time : {redirection_time}')
        user_id = read_data('user')
        
        self.t = threading.Timer(redirection_time,self.set_interval_get_info,args=(user_id,))
        self.t.start()
    
    def cancle_timer(self):
        if self.t is not None: # timer is alive ?
            self.t.cancel() # timer cancle
        # self.t.cancel()
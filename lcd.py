import urllib.request
import json
import os
import requests
import time
from dotenv import load_dotenv
# load .env
load_dotenv()

client_id = os.environ.get('papago_client_id')
client_secret = os.environ.get('papago_client_secret')

print(f'env file cli_id :{client_id} , cli_scre :{client_secret}')

def write_data(txt,write_type):

    if write_type == 'user':
        file_url = './txts/user.txt'

    elif write_type == 'listening':
        file_url = './txts/listening_info.txt'
    
    elif write_type == 'duration' :
        file_url = './txts/duration.txt'
    
    data = txt
    f = open(file_url, 'w')
    f.write(data)
    f.close()

def read_data(write_type):

    if write_type == 'user':
        file_url = './txts/user.txt'

    elif write_type == 'listening':
        file_url = './txts/listening_info.txt'
    
    else :
        file_url = './txts/duration.txt'
    
    f = open(file_url,'r')
    file_data = f.read()
    f.close()
    return file_data

def english_to_korean(papago_quote):
    encText = urllib.parse.quote(papago_quote)
    data = "source=ko&target=en&text=" + encText
    url = "https://openapi.naver.com/v1/papago/n2mt"
    try:

        request = urllib.request.Request(url)
        request.add_header("X-Naver-Client-Id",client_id)
        request.add_header("X-Naver-Client-Secret",client_secret)
        response = urllib.request.urlopen(request, data=data.encode("utf-8"))
        rescode = response.getcode()
        if(rescode==200):
            response_body = response.read()
            print(response_body.decode('utf-8'))
            data = json.loads(response_body)
            translated_text = data['message']['result']['translatedText']
            
        else:
            print("Error Code:" + rescode)
    except:
        print('enlish_to_korean() : papago error')

    return str(translated_text)

def settup_lcd_data(data):

    if (len(data) == 0 ):
        return 'no data|please replay'
    
    do_trans  = False
    tmp_list = []
    result_str = ''

    # data split
    response_list = data.split('|')

    for rl in response_list:
        do_trans = False
        for ch in rl:
            if '가' <= ch <= '힣':
                tmp_list.append(english_to_korean(rl))
                do_trans = True
                break
        
        if do_trans == False:
            tmp_list.append(rl)
    
    for i in tmp_list:
        print(f"tmp list : {i}")

    if len(tmp_list) == 2:
        result_str = f'{tmp_list[0]}|{tmp_list[1]}'
    
    elif len(tmp_list) == 3:
        result_str = f'{tmp_list[0]}|{tmp_list[1]}|{tmp_list[2]}'
        
    return result_str

def send_to_nest(command,user_id, tim):
    tm = tim

    if command == None:
        pass

    # volume command
    if command == '0' or command == '1':

        url_items = f'http://192.168.0.133:3000/apis/volume/'
        
        if command == '0':
            data = {'volume': False, 'userId': user_id}
        
        else :
            data = {'volume': True , 'userId': user_id}
        
        try :
            response = requests.post(url_items, json=data,verify=False)
        
            if ( len(response.text) == 0 or response is None ) :
                results_str = 'response.text|error'
                return results_str
            else:
                return response.text
        except:
            print('send_to_nest, response error')

    # play stop prev next command   
    else:
        # url
        url_items = f'http://192.168.0.133:3000/apis/command/'
        
        # data
        data = {'command': command, 'userId': user_id}
        
        try:
            # send
            response = requests.post(url_items, json=data,timeout=30,verify=False)
            
            print(f'response done : {response.text}')
            result = response.text.split('|')

            if ( len(result) <= 1 ) :
                return 'error'
            
            else:
                if command == 'play':
                    return settup_lcd_data(response.text)

                elif command == 'stop':
                    tm.cancle_timer()
                    return f'{response.text}'
                
                elif command == 'prev' or command == 'next':
                    tm.cancle_timer()
                    rfd_list = read_data('duration').split('|')
                    tm.set_interval(int(rfd_list[0]))
                    time.sleep(2.5)
                    data2 = tm.timer_get_track_info(user_id=user_id)
                    stm_data= settup_lcd_data(data2)
                    return stm_data
        except:
            print('send_to_nest, response error')
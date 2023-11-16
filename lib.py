import requests
import time
import json
import papago

def send_to_nest(command,user_id):

    if command == None:
        pass

    ''' get
    url_items = f'http://192.168.0.133:3000/apis/command/{command}'
    response = requests.get(url_items)
    '''

    url_items = f'http://192.168.0.133:3000/apis/command/'
    result_str = ''
    data = {'command': command, 'userId': user_id}
    response = requests.post(url_items, json=data,timeout=5)
    response_list_zero = False
    response_list_one  = False

    if command == 'play':
        response_list = response.text.split('|')

        for ch in response_list[0]:
            if '가' <= ch <= '힣':  # 한글의 유니코드 범위
                response_list_zero = True

        if response_list_zero:
            result_str += papago.english_to_korean(response_list[0]) +'|'
        else:
            result_str += response_list[0]+'|'

        for ch in response_list[1]:
            if '가' <= ch <= '힣':  # 한글의 유니코드 범위
                response_list_one = True
        
        if response_list_one:
            result_str += papago.english_to_korean(response_list[1])
        else:
            result_str += response_list[1]

        return result_str

    elif command == 'stop':
        return response.text

def write_data_in_file(txt,write_type):

    if write_type == 'user':
        file_url = './txts/user.txt'

    elif write_type == 'listening':
        file_url = './txts/listening_info.txt'
    
    else :
        file_url = './txts/duration.txt'
    
    data = txt
    f = open(file_url, 'w')
    f.write(data)
    f.close()

def read_file_data(write_type):

    if write_type == 'user':
        file_url = './txts/user.txt'

    elif write_type == 'listening':
        file_url = './txts/listening_info.txt'
        f = open(file_url,'r')
        file_data = f.read()
        f.close()
        json_object = json.loads(file_data)
        time.sleep(3)
        d1 = json_object['artistName']
        d2 = json_object['songName']
        serial_data= f"{d1}|{d2}"
        return serial_data
    
    else :
        file_url = './txts/duration.txt'
    
    f = open(file_url,'r')
    file_data = f.read()
    f.close()
    return file_data

def track_info(user_id):
    try:
        headers = {
            "User-Agent" : "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0"
        }
        url_items = f'http://192.168.0.133:3000/apis/getTrack/{user_id}'
        response = requests.get(url_items,timeout=5,headers=headers)
        print(response.text)
        write_data_in_file(txt=response.text,write_type='listening')
    except requests.exceptions.RequestException as e:
        print(f'error track_info : {e}')
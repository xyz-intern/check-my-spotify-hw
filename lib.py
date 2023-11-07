import requests
import itertools
import threading
import serial
import socket

def send_to_nest(command,user_id):

    if command == None:
        pass

    ''' get
    url_items = f'http://192.168.0.133:3000/apis/command/{command}'
    response = requests.get(url_items)
    '''

    url_items = f'http://192.168.0.133:3000/apis/command/'

    data = {'command': command, 'user_id': user_id}
    response = requests.post(url_items, json=data,timeout=5)

    return response.text

def write_data_in_file(txt,write_type):

    if write_type == 'user':
        file_url = './user.txt'
        # origin_data = read_file_data(write_type)
        # data = origin_data+f'{txt},'

    elif write_type == 'listening':
        file_url = './listening_info.json'
    
    else :
        file_url = './duration.txt'
    
    data = txt
    f = open(file_url, 'w')
    f.write(data)
    f.close()

def read_file_data(write_type):

    if write_type == 'user':
        file_url = './user.txt'

    elif write_type == 'listening':
        file_url = './listening_info.json'
    
    else :
        file_url = './duration.txt'
    
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
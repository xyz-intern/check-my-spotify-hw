import requests

command= 'stop'
url_items = f'http://192.168.0.133:3000/apis/{command}'
response = requests.get(url_items)

print(response.text)
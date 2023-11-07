import requests
import json

# URL
# 127.0.0.1은 localhost로 대체 가능 
url = "http://127.0.0.1:8080/test"

# headers
headers = {
    "Content-Type": "application/json"
}

# data
temp = {
    "color": "black",
    "size": 200
}
# 딕셔너리를 JSON으로 변환 
data = json.dumps(temp)


response = requests.post(url, headers=headers, data=data)

print("response: ", response)
print("response.text: ", response.text)
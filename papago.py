import urllib.request
import json

def english_to_korean(papago_quote):
    encText = urllib.parse.quote(papago_quote)
    data = "source=ko&target=en&text=" + encText
    url = "https://openapi.naver.com/v1/papago/n2mt"
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

    return str(translated_text)
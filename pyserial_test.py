
import serial # pyserial 라이브러리 가져오기 

def sendData(x,y):
    # 1 Byte 보내기 16진수 0xC0 보내기
    ser.write(bytes(bytearray([0xC0])))
    # 1 Byte 보내기 매개변수로 받은 x 값 보내기
    ser.write(bytes(bytearray([x]))) 
    # 1 Byte 보내기 매개변수로 받은 y 값 보내기
    ser.write(bytes(bytearray([y])))


#시리얼 열기 보드레이트 9600
ser = serial.Serial('/dev/ttyUSB0', 9600)

#sendData함수를 호출하여 시리얼 데이터 보내기
sendData(0x32, 0x5A) #10진수로 넣어도 동일한 결과가 나타난다.

#시리얼 포트 닫기
ser.close()
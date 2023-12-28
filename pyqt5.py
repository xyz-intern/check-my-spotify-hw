from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton
import sys
import requests

def send_data(command,user_id):
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
                    return response.text

                elif command == 'stop':
                    return f'{response.text}'
        except:
            print('send_to_nest, response error')

class Example(QMainWindow):
    def __init__(self):
        super().__init__()

        self.initUI()


    def initUI(self):
        btn1 = QPushButton('play', self)
        btn1.move(30, 50)
        btn1.clicked.connect(self.buttonClicked)  

        btn2 = QPushButton('stop', self)
        btn2.move(150, 50)
        btn2.clicked.connect(self.buttonClicked)
        
        self.statusBar()

        self.setGeometry(300, 300, 290, 150)
        self.setWindowTitle('Event sender')
        self.show()

    def buttonClicked(self):
        sender = self.sender()
        self.statusBar().showMessage(sender.text() + ' 눌림')
        if sender.text() == 'play':
            send_data('play','dlatldhs')
        else:
            send_data('stop','dlatldhs')

def main():
    app = QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

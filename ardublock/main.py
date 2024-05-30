import os
import pyduinocli
from tkinter import *

os.chdir(os.path.dirname(__file__))

arduino = pyduinocli.Arduino("./arduino-cli")
print(arduino.version())
brds = arduino.board.list()

id = -1 # -1은 아무 보드도 선택하지 않았다는 뜻

def find_board():
    global brds
    brds = arduino.board.list()
    print(brds['result'])

def select_board():
    global id
    try:
        id = int(input("Enter Port ID you want : "))
    except ValueError:
        print("Id must be number")

def upload():
    if id < 0 or len(brds['result']) <= id:
        print("There is no board id:", id)
        return
    port = brds['result'][id]['port']['address']
    fqbn = brds['result'][id]['matching_boards'][0]['fqbn']
    print(fqbn)

    arduino.compile(fqbn=fqbn, sketch="sketch")
    arduino.upload(fqbn=fqbn, sketch="sketch", port=port)
    print("Upload succeed")

tk = Tk()
label = Label(tk,text='Hello World!')
label.pack()
Button(tk, text="Find Board", command=find_board).pack()
Button(tk, text="Select Board", command=select_board).pack()
Button(tk, text="Upload", command=upload).pack()
Button(tk, text="Quit", command=tk.destroy).pack()
tk.mainloop()
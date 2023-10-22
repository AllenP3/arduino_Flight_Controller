from tkinter import *
import serial

ser = serial.Serial('COM3', 9600)

def show_values():
    print (w1.get(), w2.get())
master = Tk()
master.title("ESC")
master.geometry('600x400+300+300')
master.resizable(False, False)
w1 = Scale(master, from_=2000, to=1000, tickinterval=10)
w1.set(1000)
w1.pack()
w2 = Scale(master, from_=0, to=200,tickinterval=10, orient=HORIZONTAL)
w2.set(23)
w2.pack()
Button(master, text='Show', command=show_values).pack()

mainloop()
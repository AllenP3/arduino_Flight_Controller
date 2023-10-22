import serial
import time

arduino_linux=serial.Serial("COM4",9600)
while True:
    print('write:',str.encode('90')) #print the sent value through the serial to check it with the readed value
    arduino_linux.write(str.encode('90'))#send the following value
    print('read:',arduino_linux.read())#r
    time.sleep(1)
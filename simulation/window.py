from tkinter import *
import serial

# Initialize serial communication
try:
    ser = serial.Serial('COM3', 9600, timeout=1)  # Adjust COM port as needed
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    ser = None  # Avoid crashing if serial fails

def send_values():
    """Send values from sliders to ESC via serial"""
    if ser:
        throttle = w1.get()
        angle = w2.get()
        data = f"{throttle},{angle}\n"  # Format: "throttle,angle"
        ser.write(data.encode())  # Send to ESC
        print(f"Sent: {data.strip()}")

# Setup main window
master = Tk()
master.title("ESC Controller")
master.geometry('400x300')
master.resizable(False, False)

# Create labels and sliders
Label(master, text="Throttle (1000-2000)").pack()
w1 = Scale(master, from_=2000, to=1000, tickinterval=100)
w1.set(1000)
w1.pack()

Label(master, text="Steering (0-200)").pack()
w2 = Scale(master, from_=0, to=200, tickinterval=10, orient=HORIZONTAL)
w2.set(23)
w2.pack()

# Button to send values
Button(master, text='Send', command=send_values).pack()

# Run the GUI
mainloop()

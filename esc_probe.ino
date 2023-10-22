#include <Servo.h>

byte servoPin = 9; 
byte potentiometerPin = A0; 
Servo servo;

void setup() {
servo.attach(servoPin);
servo.writeMicroseconds(1500); 

delay(7000); 
}

void loop() {

int potVal = analogRead(potentiometerPin); 

int pwmVal = map(potVal,0, 1023, 1100, 1900); 

servo.writeMicroseconds(pwmVal);
}
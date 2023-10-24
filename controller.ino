#include <Wire.h>
#include <EEPROM.h>

float r_p_gain = 0;
float r_i_gain = 0;
float r_d_gain = 0;
float r_max = 0;

float p_p_gain = 0;
float p_i_gain = 0;
float p_d_gain = 0;
float p_max = 0;

float y_p_gain = 0;
float y_i_gain = 0;
float y_d_gain = 0;
float y_max = 0;

volatile int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
int receiver_input[5];
byte eeprom_data[36];
int cal_int, start, gyro_address;
double gyro_axis_cal[4];
int acc_axis[4], gyro_axis[4];

void setup(){

    for(start = 0; start <= 35; start++)eeprom_data[start] = EEPROM.read(start);
        start = 0;                                                                
        gyro_address = eeprom_data[32];

        Wire.begin();
        //Wire.setClock(400000);
        TWBR = 12;  
        DDRD |= B11110000;  //4, 5, 6, 7
        DDRB |= B00110000;  //12, 13
        digitalWrite(12,HIGH);   
        
        while(eeprom_data[33] != 'A' || eeprom_data[34] != 'B' || eeprom_data[35] != 'D')
            delay(10);
        if(eeprom_data[31] == 2 || eeprom_data[31] == 3)
            delay(10);
        set_gyro_registers();
        for (cal_int = 0; cal_int < 1250 ; cal_int ++){                           
            PORTD |= B11110000;                                                     
            delayMicroseconds(1000);                                                
            PORTD &= B00001111;                                                    
            delayMicroseconds(3000);                                                
        }

        for(cal_int = 0; cal_int < 2000 ; cal_int++){
            if(cal_int % 15 == 0)digitalWrite(12, !digitalRead(12));
            gyro_signalen();
            gyro_axis[1] += gyro_axis[1];
            gyro_axis[2] += gyro_axis[2];
            gyro_axis[3] += gyro_axis[3];

            PORTD |= B11110000;                                                     
            delayMicroseconds(1000);                                                
            PORTD &= B00001111;                                                     
            delay(3); 
        }

        gyro_axis_cal[1] /= 2000;                                                
        gyro_axis_cal[2] /= 2000;                                                 
        gyro_axis_cal[3] /= 2000;

        PCICR |= (1 << PCIE0);                                                    // scan enable
        PCMSK0 |= (1 << PCINT0);                                                  // state change 8
        PCMSK0 |= (1 << PCINT1);                                                  // state change 9
        PCMSK0 |= (1 << PCINT2);                                                  // state change 10
        PCMSK0 |= (1 << PCINT3);                                                  // state change 11




        while(receiver_input_channel_3 < 990 || receiver_input_channel_3 > 1020 || receiver_input_channel_4 < 1400){
            receiver_input_channel_3 = convert_receiver_channel(3);                 
            receiver_input_channel_4 = convert_receiver_channel(4);                 
            start ++;                                                              
    
            PORTD |= B11110000;                                                     //Set digital poort 4, 5, 6 and 7 high.
            delayMicroseconds(1000);                                                //Wait 1000us.
            PORTD &= B00001111;                                                     //Set digital poort 4, 5, 6 and 7 low.
            delay(3);                                                               //Wait 3 milliseconds before the next loop.
            if(start == 125){                                                       //Every 125 loops (500ms).
            digitalWrite(12, !digitalRead(12));                                   //Change the led status.
            start = 0;                                                            //Start again at 0.
            }
        }
        start = 0; 
        battery_voltage = (analogRead(0) + 65) * 1.2317;
        loop_timer = micros();
        digitalWrite(12,LOW);

}

void loop(){

}

/*void eschl(string state){
    if(state == "HIGH"){
        digitalWrite(4, HIGH);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(7, HIGH);
    }
    else if(state == "LOW"){
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
    }
}*/

int convert_receiver_channel(byte function){
  byte channel, reverse;                                                      
  int low, center, high, actual;
  int difference;

  //channel = eeprom_data[function + 23] & 0b00000111;                           
  //if(eeprom_data[function + 23] & 0b10000000)reverse = 1;                     
  //else reverse = 0;                                                            

  actual = receiver_input[channel];                                            
  low = (eeprom_data[channel * 2 + 15] << 8) | eeprom_data[channel * 2 + 14]; 
  center = (eeprom_data[channel * 2 - 1] << 8) | eeprom_data[channel * 2 - 2];
  high = (eeprom_data[channel * 2 + 7] << 8) | eeprom_data[channel * 2 + 6];   

  if(actual < center){                                                         
    if(actual < low)actual = low;                                              
    difference = ((long)(center - actual) * (long)500) / (center - low);       
    if(reverse == 1)return 1500 + difference;                                  
    else return 1500 - difference;                                             
  }
  else if(actual > center){                                                                        
    if(actual > high)actual = high;                                            
    difference = ((long)(actual - center) * (long)500) / (high - center);      
    if(reverse == 1)return 1500 - difference;                                  
    else return 1500 + difference;                                             
  }
  else return 1500;
}

void set_gyro_registers(){
  //Setup the MPU-6050
  if(eeprom_data[31] == 1){
    Wire.beginTransmission(gyro_address);                                      //Start communication with the address found during search.
    Wire.write(0x6B);                                                          //We want to write to the PWR_MGMT_1 register (6B hex)
    Wire.write(0x00);                                                          //Set the register bits as 00000000 to activate the gyro
    Wire.endTransmission();                                                    //End the transmission with the gyro.

    Wire.beginTransmission(gyro_address);                                      //Start communication with the address found during search.
    Wire.write(0x1B);                                                          //We want to write to the GYRO_CONFIG register (1B hex)
    Wire.write(0x08);                                                          //Set the register bits as 00001000 (500dps full scale)
    Wire.endTransmission();                                                    //End the transmission with the gyro

    Wire.beginTransmission(gyro_address);                                      //Start communication with the address found during search.
    Wire.write(0x1C);                                                          //We want to write to the ACCEL_CONFIG register (1A hex)
    Wire.write(0x10);                                                          //Set the register bits as 00010000 (+/- 8g full scale range)
    Wire.endTransmission();                                                    //End the transmission with the gyro

    //Let's perform a random register check to see if the values are written correct
    Wire.beginTransmission(gyro_address);                                      //Start communication with the address found during search
    Wire.write(0x1B);                                                          //Start reading @ register 0x1B
    Wire.endTransmission();                                                    //End the transmission
    Wire.requestFrom(gyro_address, 1);                                         //Request 1 bytes from the gyro
    while(Wire.available() < 1);                                               //Wait until the 6 bytes are received
    if(Wire.read() != 0x08){                                                   //Check if the value is 0x08
      digitalWrite(12,HIGH);                                                   //Turn on the warning led
      while(1)delay(10);                                                       //Stay in this loop for ever
    }

    Wire.beginTransmission(gyro_address);                                      //Start communication with the address found during search
    Wire.write(0x1A);                                                          //We want to write to the CONFIG register (1A hex)
    Wire.write(0x03);                                                          //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
    Wire.endTransmission();                                                    //End the transmission with the gyro    

  }  
}
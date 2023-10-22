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

volatile int rec_ch1_in, rec_ch2_in, rec_ch3_in, rec_ch4_in;
int receiver_input[5];
byte eeprom_data[36];

void setup(){

    for(start = 0; start <= 35; start++)eeprom_data[start] = EEPROM.read(start);
        start = 0;                                                                
        gyro_address = eeprom_data[32];

        Wire.begin();
        Wire.setClock(400000);
        DDRD |= B11110000;  //4, 5, 6, 7
        DDRB |= B00110000;  //12, 13

        for (cal_int = 0; cal_int < 1250 ; cal_int ++){
            if(cal_int % 15 == 0) digitalWrite(12, !digitalRead(12));
            eschl("HIGH");
            delayMicroseconds(1000);
            eschl("LOW");
            delay(2); 
        }
        while(eeprom_data[33] != 'A' || eeprom_data[34] != 'B' || eeprom_data[35] != 'D')
            delay(10);

        PCICR |= (1 << PCIE0);                                                    // scan enable
        PCMSK0 |= (1 << PCINT0);                                                  // state change 8
        PCMSK0 |= (1 << PCINT1);                                                  // state change 9
        PCMSK0 |= (1 << PCINT2);                                                  // state change 10
        PCMSK0 |= (1 << PCINT3);                                                  // state change 11




        while(rec_ch3_in < 980 || rec_ch3_in > 1020 || rec_ch4_in < 1400){
            rec_ch3_in = convert_receiver_channel(3);                 
            rec_ch4_in = convert_receiver_channel(4);                
            start ++;

            // disarm beeping
            // eschl("HIGH");
            PORTD |= B11110000;                                                   // 4, 5, 6, 7 HIGH                                                     
            delayMicroseconds(1000);                                                
            //eschl("LOW");
            PORTD &= B00001111;                                                    
            delay(3);                                                               
            if(start == 125){                                                       
                digitalWrite(12, !digitalRead(12));                                   
                start = 0;                                                           
            }
        }
        start = 0; 

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
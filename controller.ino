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

void setup(){

    for(start = 0; start <= 35; start++)eeprom_data[start] = EEPROM.read(start);
        start = 0;                                                                
        gyro_address = eeprom_data[32];

        Wire.begin();
        Wire.setClock(400000);
        pinmode(4, OUTPUT);
        pinmode(5, OUTPUT);
        pinmode(6, OUTPUT);
        pinmode(7, OUTPUT);
        pinmode(12, OUTPUT);
        pinmode(13, OUTPUT);

        for (cal_int = 0; cal_int < 1250 ; cal_int ++){
            if(cal_int % 15 == 0) digitalWrite(12, !digitalRead(12));
            eschl("HIGH");
            delayMicroseconds(1000);
            eschl("LOW");
            delay(2); 
        }

        while(rec_ch3_in < 990 || rec_ch3_in > 1020 || rec_ch4_in < 1400){
            rec_ch3_in = convert_receiver_channel(3);                 
            rec_ch4_in = convert_receiver_channel(4);                
            start ++;                                                               
            eschl("HIGH");                                                     
            delayMicroseconds(1000);                                                
            eschl("LOW");                                                    
            delay(3);                                                               
            if(start == 125){                                                       
                digitalWrite(12, !digitalRead(12));                                   
                start = 0;                                                           
            }
        }

}

void eschl(string state){
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
}
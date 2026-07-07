#include <Arduino.h>

const byte previous_page_button_pin = 18;
const byte next_page_button_pin = 19;
const byte encoder_CLK_A_pin = 32; 
const byte encoder_DT_B_pin = 33;
const byte encoder_switch_pin = 25;

void setup(){
    pinMode(previous_page_button_pin, INPUT);
    pinMode(next_page_button_pin, INPUT);
}

void loop(){
    
}
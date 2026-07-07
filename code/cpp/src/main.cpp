#include <Arduino.h>

const byte previous_page_button_pin = 18;
const byte next_page_button_pin = 19;

void setup(){
    pinMode(previous_page_button_pin, INPUT);
    pinMode(next_page_button_pin, INPUT);
}

void loop(){
    
}
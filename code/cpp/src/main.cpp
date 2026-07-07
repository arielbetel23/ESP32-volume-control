#include <Arduino.h>

const byte previous_page_button_pin = 18;
const byte next_page_button_pin = 19;
const byte encoder_CLK_A_pin = 32; 
const byte encoder_DT_B_pin = 33;
const byte encoder_switch_pin = 25;
const byte LED_pin = 14;
const byte buzzer_pin = 26;

void setup(){
    pinMode(previous_page_button_pin, INPUT);
    pinMode(next_page_button_pin, INPUT);
    pinMode(encoder_CLK_A_pin, INPUT);
    pinMode(encoder_DT_B_pin, INPUT);
    pinMode(encoder_switch_pin, INPUT);
    pinMode(LED_pin, OUTPUT);
    pinMode(buzzer_pin, OUTPUT);

    Serial.begin(115200);
}

bool last_state_next_button = HIGH;
bool last_state_previous_button = HIGH;

void loop(){
    bool current_state_next_button = digitalRead(next_page_button_pin);
    bool current_state_previous_button = digitalRead(previous_page_button_pin);

    if(last_state_previous_button == HIGH && current_state_previous_button == LOW){
        // the action
    }
    last_state_previous_button = current_state_previous_button;
    if (last_state_next_button == HIGH && current_state_next_button == LOW) {
        // the action
    }

    last_state_next_button = current_state_next_button;
}
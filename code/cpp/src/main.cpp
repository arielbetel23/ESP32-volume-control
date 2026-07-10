#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const byte previous_page_button_pin = 18;
const byte next_page_button_pin = 19;
const byte encoder_CLK_A_pin = 32;
const byte encoder_DT_B_pin = 33;
const byte encoder_switch_pin = 25;
const byte LED_pin = 14;
const byte buzzer_pin = 26;

const int LED_FEEDBACK_DURATION = 40;
const int BEEP_FREQUENCY = 2000;
const int BEEP_DURATION = 30;

const byte SCREEN_WIDTH = 128;
const byte SCREEN_HEIGHT = 64;
const byte OLED_ADDRESS = 0x3C;
const byte OLED_SDA_pin = 21;
const byte OLED_SCL_pin = 22;
const int DISPLAY_REFRESH_INTERVAL = 200;

String serial_buffer = "";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String current_date = "Sun, 27th Jul";
String current_time = "14:57";
String current_temp = "27";
String current_desc = "sunny day";

unsigned long led_start_time = 0;
bool led_is_on = false;

unsigned long last_draw_time = 0;

void handle_line(String line){
    int separator_index = line.indexOf(':');
    if(separator_index == -1){
        return;
    }
    String prefix = line.substring(0, separator_index);
    String value = line.substring(separator_index + 1);
}

void read_serial(){
    while(Serial.available()){
        char incoming_char = Serial.read();
        if(incoming_char == '\n'){
            handle_line(serial_buffer);
            serial_buffer = "";
        }
        else if(incoming_char != '\r'){
            serial_buffer += incoming_char;
        }
    }
}

void start_feedback(){
    digitalWrite(LED_pin, HIGH);
    led_start_time = millis();
    led_is_on = true;
    tone(buzzer_pin, BEEP_FREQUENCY, BEEP_DURATION);
}

void update_feedback(){
    if (led_is_on && millis() - led_start_time >= LED_FEEDBACK_DURATION) {
        digitalWrite(LED_pin, LOW);
        led_is_on = false;
    }
}

void init_display(){
    Wire.begin(OLED_SDA_pin, OLED_SCL_pin);
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
    display.cp437(true);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    display.display();
}

void draw_home_page(){
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(current_date);

    display.setTextSize(3);
    int16_t bounds_x, bounds_y;
    uint16_t bounds_w, bounds_h;
    display.getTextBounds(current_time, 0, 0, &bounds_x, &bounds_y, &bounds_w, &bounds_h);
    display.setCursor((SCREEN_WIDTH - bounds_w) / 2, 22);
    display.print(current_time);

    display.setTextSize(1);
    String weather_line = current_temp + (char)247 + "C " + current_desc;
    display.setCursor(0, SCREEN_HEIGHT - 8);
    display.print(weather_line);

    display.display();
}

void setup(){
    pinMode(previous_page_button_pin, INPUT);
    pinMode(next_page_button_pin, INPUT);
    pinMode(encoder_CLK_A_pin, INPUT);
    pinMode(encoder_DT_B_pin, INPUT);
    pinMode(encoder_switch_pin, INPUT);
    pinMode(LED_pin, OUTPUT);
    pinMode(buzzer_pin, OUTPUT);

    Serial.begin(115200);

    init_display();
}

bool last_state_next_button = HIGH;
bool last_state_previous_button = HIGH;
bool last_state_encoder_switch = HIGH;
bool last_state_encoder_CLK = HIGH;

void loop(){
    update_feedback();

    bool current_state_encoder_CLK = digitalRead(encoder_CLK_A_pin);
    bool current_state_next_button = digitalRead(next_page_button_pin);
    bool current_state_previous_button = digitalRead(previous_page_button_pin);
    bool current_state_encoder_switch = digitalRead(encoder_switch_pin);

    if(last_state_encoder_CLK == HIGH && current_state_encoder_CLK == LOW){
        if(digitalRead(encoder_DT_B_pin) == HIGH){
            Serial.println("VOL:+");
        }
        else{
            Serial.println("VOL:-");
        }
    }
    last_state_encoder_CLK = current_state_encoder_CLK;

    if(last_state_encoder_switch == HIGH && current_state_encoder_switch == LOW){
        Serial.println("VOL:MUTE");
        start_feedback();
    }
    last_state_encoder_switch = current_state_encoder_switch;

    if(last_state_previous_button == HIGH && current_state_previous_button == LOW){
        start_feedback();
        // the action - moving previous page
    }
    last_state_previous_button = current_state_previous_button;

    if (last_state_next_button == HIGH && current_state_next_button == LOW) {
        start_feedback();
        // the action - moving next page
    }

    last_state_next_button = current_state_next_button;

    if(millis() - last_draw_time >= DISPLAY_REFRESH_INTERVAL){
        draw_home_page();
        last_draw_time = millis();
    }
}

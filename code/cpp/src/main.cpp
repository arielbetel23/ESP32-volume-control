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

enum Page { PAGE_HOME, PAGE_TIMER, PAGE_STOPWATCH, PAGE_COUNT };
enum TimerState { TIMER_IDLE, TIMER_RUNNING, TIMER_PAUSED };

const int MAX_TIMER_SECONDS = 5999;
const unsigned long LONG_PRESS_DURATION = 800;
const unsigned long FINISH_ALERT_DURATION = 1200;
const int FINISH_BEEP_FREQUENCY = 2500;

String serial_buffer = "";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String current_date = "----/--/--";
String current_time = "--:--";
String current_temp = "--";
String current_desc = "waiting...";

unsigned long led_start_time = 0;
bool led_is_on = false;

unsigned long last_draw_time = 0;

int current_page = PAGE_HOME;
TimerState timer_state = TIMER_IDLE;
int remaining_seconds = 0;
unsigned long last_countdown_tick = 0;

bool switch_was_down = false;
bool long_fired = false;
unsigned long switch_press_start = 0;

bool finish_alert_active = false;
unsigned long finish_alert_start = 0;

void handle_line(String line){
    int separator_index = line.indexOf(':');
    if(separator_index == -1){
        return;
    }
    String prefix = line.substring(0, separator_index);
    String value = line.substring(separator_index + 1);

    if(prefix == "DATE"){
        current_date = value;
    }
    else if(prefix == "TIME"){
        current_time = value;
    }
    else if(prefix == "TEMP"){
        current_temp = value;
    }
    else if(prefix == "DESC"){
        current_desc = value;
    }
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

void go_to_next_page(){
    current_page = (current_page + 1) % PAGE_COUNT;
}

void go_to_previous_page(){
    current_page = (current_page + PAGE_COUNT - 1) % PAGE_COUNT;
}

void handle_encoder_rotation(int direction){
    if(current_page == PAGE_HOME){
        Serial.println(direction > 0 ? "VOL:+" : "VOL:-");
    }
    else if(current_page == PAGE_TIMER && timer_state == TIMER_IDLE){
        remaining_seconds += direction;
        if(remaining_seconds < 0){
            remaining_seconds = 0;
        }
        if(remaining_seconds > MAX_TIMER_SECONDS){
            remaining_seconds = MAX_TIMER_SECONDS;
        }
    }
}

void toggle_timer(){
    if(timer_state == TIMER_IDLE){
        if(remaining_seconds > 0){
            timer_state = TIMER_RUNNING;
            last_countdown_tick = millis();
        }
    }
    else if(timer_state == TIMER_RUNNING){
        timer_state = TIMER_PAUSED;
    }
    else{
        timer_state = TIMER_RUNNING;
        last_countdown_tick = millis();
    }
}

void cancel_timer(){
    timer_state = TIMER_IDLE;
    remaining_seconds = 0;
}

void start_finish_alert(){
    finish_alert_active = true;
    finish_alert_start = millis();
    digitalWrite(LED_pin, HIGH);
    tone(buzzer_pin, FINISH_BEEP_FREQUENCY);
}

void update_finish_alert(){
    if(!finish_alert_active){
        return;
    }
    if(millis() - finish_alert_start >= FINISH_ALERT_DURATION){
        finish_alert_active = false;
        digitalWrite(LED_pin, LOW);
        noTone(buzzer_pin);
    }
}

void handle_short_press(){
    if(current_page == PAGE_HOME){
        Serial.println("VOL:MUTE");
    }
    else if(current_page == PAGE_TIMER){
        toggle_timer();
    }
    start_feedback();
}

void handle_long_press(){
    if(current_page == PAGE_TIMER){
        cancel_timer();
        start_feedback();
    }
}

void read_encoder_switch(){
    bool pressed = digitalRead(encoder_switch_pin) == LOW;
    if(pressed && !switch_was_down){
        switch_was_down = true;
        switch_press_start = millis();
        long_fired = false;
    }
    else if(pressed && switch_was_down){
        if(!long_fired && millis() - switch_press_start >= LONG_PRESS_DURATION){
            long_fired = true;
            handle_long_press();
        }
    }
    else if(!pressed && switch_was_down){
        switch_was_down = false;
        if(!long_fired){
            handle_short_press();
        }
    }
}

void update_timer(){
    if(timer_state != TIMER_RUNNING){
        return;
    }
    if(millis() - last_countdown_tick >= 1000){
        last_countdown_tick += 1000;
        remaining_seconds--;
        if(remaining_seconds <= 0){
            remaining_seconds = 0;
            timer_state = TIMER_IDLE;
            start_finish_alert();
        }
    }
}

void draw_timer_page(){
    int minutes = remaining_seconds / 60;
    int seconds = remaining_seconds % 60;
    char buffer[6];
    sprintf(buffer, "%02d:%02d", minutes, seconds);

    display.clearDisplay();
    display.setTextSize(3);

    int16_t bounds_x, bounds_y;
    uint16_t bounds_w, bounds_h;
    display.getTextBounds(buffer, 0, 0, &bounds_x, &bounds_y, &bounds_w, &bounds_h);
    display.setCursor((SCREEN_WIDTH - bounds_w) / 2, (SCREEN_HEIGHT - bounds_h) / 2);
    display.print(buffer);

    display.display();
}

void draw_stopwatch_page(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Stopwatch");
    display.display();
}

void draw_current_page(){
    if(current_page == PAGE_HOME){
        draw_home_page();
    }
    else if(current_page == PAGE_TIMER){
        draw_timer_page();
    }
    else{
        draw_stopwatch_page();
    }
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
bool last_state_encoder_CLK = HIGH;

void loop(){
    update_feedback();
    update_finish_alert();
    read_serial();
    read_encoder_switch();
    update_timer();

    bool current_state_encoder_CLK = digitalRead(encoder_CLK_A_pin);
    bool current_state_next_button = digitalRead(next_page_button_pin);
    bool current_state_previous_button = digitalRead(previous_page_button_pin);

    if(last_state_encoder_CLK == HIGH && current_state_encoder_CLK == LOW){
        handle_encoder_rotation(digitalRead(encoder_DT_B_pin) == HIGH ? 1 : -1);
    }
    last_state_encoder_CLK = current_state_encoder_CLK;

    if(last_state_previous_button == HIGH && current_state_previous_button == LOW){
        go_to_previous_page();
        start_feedback();
    }
    last_state_previous_button = current_state_previous_button;

    if (last_state_next_button == HIGH && current_state_next_button == LOW) {
        go_to_next_page();
        start_feedback();
    }
    last_state_next_button = current_state_next_button;

    if(millis() - last_draw_time >= DISPLAY_REFRESH_INTERVAL){
        draw_current_page();
        last_draw_time = millis();
    }
}

#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  air_setup();
  motor_setup();
  switch_setup();
}

void loop(){
  receive(&Serial1);
  swtich_input();
  debug_dataprint(&Serial);
  air_output();
  motor_output();
}
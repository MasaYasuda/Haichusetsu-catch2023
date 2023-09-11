#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop(){
  receive(&Serial1);
  debug_dataprint(&Serial);
  //air_output();
  //motor_output();
}
#include <Arduino.h>

void setup(){
  Serial.begin(115200);
  Serial1.begin(115200);
  dxl_setup();

}

void loop(){
  receive(&Serial1);
  debug_dataprint(&Serial);
  dxl_output();
  delay(20);
}
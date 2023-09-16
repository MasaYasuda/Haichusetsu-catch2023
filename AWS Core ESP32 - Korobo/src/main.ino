#include <Arduino.h>
#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("78:21:84:A7:B2:D6");
  dxl_setup();
}

void loop() {
  //PS4コンデータ受信 & データ格納
  dataget();
  dxl_alltrq_on();
  dxl_output();
  delay(10);
}

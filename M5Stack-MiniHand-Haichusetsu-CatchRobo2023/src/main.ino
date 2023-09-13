#include <Arduino.h>
#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("24:D7:EB:39:42:92");
  dxl_setup();
}

void loop() {
  //PS4コンデータ受信 & データ格納
  dataget();
  dxl_alltrq_on();
  dxl_output();
  //送信間隔
  delay(100);
}

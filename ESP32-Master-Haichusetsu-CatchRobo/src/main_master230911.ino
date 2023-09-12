#include <Arduino.h>
#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("B0:A7:32:13:C1:F6");
}

void loop() {
  //PS4コンデータ受信 & Arduinoへ送信
  transmit();
  //送信間隔
  delay(100);
}


//送信用データ
byte data[5]={0x00};

//送信用関数
void transmit(){
  if (PS4.isConnected()) {
    //データ初期化
    for(byte i=0;i<5;i++){
      data[i]={0x00};
    }

    //送信用データ作成
    data[0]=0xFF;
    if (PS4.Circle())   data[1]|=0b00000001;
    if (PS4.Cross())    data[1]|=0b00000010;
    if (PS4.Square())   data[1]|=0b00000100;
    if (PS4.Triangle()) data[1]|=0b00001000;
    if (PS4.Right())    data[1]|=0b00010000;
    if (PS4.Down())     data[1]|=0b00100000;
    if (PS4.Left())     data[1]|=0b01000000;
    if (PS4.Up())       data[1]|=0b10000000;

    if (PS4.R1())       data[2]|=0b00000001;
    if (PS4.L1())       data[2]|=0b00000010;
    if (PS4.R2Value()>200) data[2]|=0b00000100;
    if (PS4.L2Value()>200) data[2]|=0b00001000;
    if (PS4.Options())  data[2]|=0b00010000;
    if (PS4.Share())    data[2]|=0b00100000;
    if (PS4.R3())       data[2]|=0b01000000;
    if (PS4.L3())       data[2]|=0b10000000;

    if (PS4.LStickX()>100)   data[3]|=0b00000001;
    if (PS4.LStickX()<-100)  data[3]|=0b00000010;
    if (PS4.LStickY()>100)   data[3]|=0b00000100;
    if (PS4.LStickY()<-100)  data[3]|=0b00001000;
    if (PS4.RStickX()>100)   data[3]|=0b00010000;
    if (PS4.RStickX()<-100)  data[3]|=0b00100000;
    if (PS4.RStickY()>100)   data[3]|=0b01000000;
    if (PS4.RStickY()<-100)  data[3]|=0b10000000;

    //calc checksum
    byte checksum=0x00;
    for(byte i=0;i<4;i++){ checksum += data[i]; }
    data[4] = checksum & 0xFF;

    //送信
    Serial.write(data,sizeof(data));
  }
}
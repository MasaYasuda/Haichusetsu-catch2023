#include <Arduino.h>
#include <HardwareSerial.h>
#include<Dxl.h>

// communication (receive) -----------------------

byte data[24]={0x00};

void receive(HardwareSerial *ser){
  if(ser->available() >=5){
    byte buf[5]={0x00};
    byte tmp_sum = 0;
    buf[0] = ser->read();
    tmp_sum += buf[0];

    if (buf[0] == 0xFF)
    { // 開始バイトの場合
      //ser.println("RECEIVED");
      for (byte i = 1; i < 4; i++)
      {
        buf[i] = ser->read();
        tmp_sum += buf[i];
      }
      buf[4] = ser->read(); // read checksum
      tmp_sum &= 0xFF;              // calc check sum

      if (buf[4]==tmp_sum)
        { // チェックサムが正しい場合
          for (int i = 0; i < 24; i++) {
            // 下位ビットから順にビットを抽出してdata配列に格納
            data[i] = (buf[i/8 +1] >> i%8) & 0x01;
          }
        }
    }
  }
}

void debug_dataprint(HardwareSerial *ser){
  for(int i=0;i<24;i++){
    ser->print(data[i]);
    ser->print(" ");
  }
  ser->println("");
}


// dxl -------------------------------------------


/**
 * Dynamixelの割り当て番号(id)
 * 昇降：1
 * 手首：2
*/

// IDは，モータ個別にDynamixel Wizard2で割り振ってください．
Dxl Dxl_1(1,&Serial2);
Dxl Dxl_2(2,&Serial2);
//駆動速度[昇降,手首]
const int MAX_SPEED[2]={100,100};
//id1(昇降)のボタン[上移動、下移動]
const int BUTTON_ID1[2]={7,5};
//id2(手首)のボタン[右移動、左移動]
const int BUTTON_ID2[2]={20,21};

void dxl_setup(){
  delay(200);
  // 制御設定
  //1   Velocity Control Mode
  //3   Position Control Mode
  //4   Extended Position Control Mode
  Dxl_1.servo_control(1); //角度制御設定
  Dxl_2.servo_control(1); //角度制御設定
  Dxl_1.servo_torque(1);
  Dxl_2.servo_torque(1);
  delay(400);
}

void dxl_output(){
  //昇降上入力
  if(data[BUTTON_ID1[0]]){
    Dxl_1.servo_speed(MAX_SPEED[0]);
    delay(50);
  }
  //昇降下入力
  if(data[BUTTON_ID1[1]]){
    Dxl_1.servo_speed(-MAX_SPEED[0]);
    delay(50);
  }
  if(data[BUTTON_ID1[0]]|data[BUTTON_ID1[1]]==0){
    Dxl_1.servo_speed(0);
    delay(50);
  }

  //手首右入力
  if(data[BUTTON_ID2[0]]){
    Dxl_2.servo_speed(MAX_SPEED[1]);
    delay(50);
  }
  //手首左入力
  if(data[BUTTON_ID2[1]]){
    Dxl_2.servo_speed(-MAX_SPEED[1]);
    delay(50);
  }
  if(data[BUTTON_ID2[0]]|data[BUTTON_ID2[1]]==0){
    Dxl_2.servo_speed(0);
    delay(50);
  }
}





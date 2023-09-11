#include <Arduino.h>
#include<Dxl.h>

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
  Serial2.begin(115200);
  delay(200);
  // 制御設定
  //1   Velocity Control Mode
  //3   Position Control Mode
  //4   Extended Position Control Mode
  Dxl_1.servo_control(1); //角度制御設定
  Dxl_2.servo_control(1); //角度制御設定
  Dxl_1.servo_torque(1);
  Dxl_2.servo_torque(1);
  delay(200);
}

void dxl_output(){
  //昇降上入力
  if(data[BUTTON_ID1[0]]){
    Dxl_1.servo_speed(MAX_SPEED[0]);
    delay(50);
  }
  //昇降下入力
  if(data[BUTTON_ID1[1]]){
    Dxl_1.servo_speed(MAX_SPEED[0]);
    delay(50);
  }
  //手首右入力
  if(data[BUTTON_ID2[0]]){
    Dxl_1.servo_speed(MAX_SPEED[1]);
    delay(50);
  }
  //手首左入力
  if(data[BUTTON_ID2[1]]){
    Dxl_1.servo_speed(MAX_SPEED[1]);
    delay(50);
  }
}





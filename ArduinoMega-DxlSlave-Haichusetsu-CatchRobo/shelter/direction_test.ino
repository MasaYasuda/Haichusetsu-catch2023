#include <Arduino.h>
#include <Dxl.h>

// IDは，モータ個別にDynamixel Wizard2で割り振ってください．
Dxl Dxl_1(1,&Serial2);
Dxl Dxl_2(2,&Serial2);

void setup(){
  Serial2.begin(115200);
  // 制御設定
  //1   Velocity Control Mode
  //3   Position Control Mode
  //4   Extended Position Control Mode
  Dxl_1.servo_control(1); //角度制御設定
  Dxl_2.servo_control(1); //角度制御設定
  Dxl_1.servo_torque(1);
  Dxl_2.servo_torque(1);
  delay(400);
  Dxl_1.servo_speed(200);
  Dxl_2.servo_speed(200);
  delay(200);
}

void loop(){
  delay(10);
}
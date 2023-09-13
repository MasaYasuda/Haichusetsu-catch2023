#include <Arduino.h>
#include <HardwareSerial.h>
#include <PS4Controller.h>
#include <Dxl.h>


// COMMUNICATION ----------------------------------------------

//データ格納用配列
byte data[24]={0x00};
//PS4からのデータ受信&データ格納

unsigned long previousMillis = 0;
const long interval = 500;

void dataget(){
  unsigned long currentMillis = millis(); // 現在の時間を取得

  if (PS4.isConnected()) {
    //データ初期化
    for(byte i=0;i<24;i++){
      data[i]={0x00};
    }
    if (PS4.Circle())   data[0]=0x01;
    if (PS4.Cross())    data[1]=0x01;
    if (PS4.Square())   data[2]=0x01;
    if (PS4.Triangle()) data[3]=0x01;
    if (PS4.Right())    data[4]=0x01;
    if (PS4.Down())     data[5]=0x01;
    if (PS4.Left())     data[6]=0x01;
    if (PS4.Up())       data[7]=0x01;

    if (PS4.R1())       data[8]=0x01;
    if (PS4.L1())       data[9]=0x01;
    if (PS4.R2Value()>200) data[10]=0x01;
    if (PS4.L2Value()>200) data[11]=0x01;
    if (PS4.Options())  data[12]=0x01;
    if (PS4.Share())    data[13]=0x01;
    if (PS4.R3())       data[14]=0x01;
    if (PS4.L3())       data[15]=0x01;

    if (PS4.LStickX()>100)   data[16]=0x01;
    if (PS4.LStickX()<-100)  data[17]=0x01;
    if (PS4.LStickY()>100)   data[18]=0x01;
    if (PS4.LStickY()<-100)  data[19]=0x01;
    if (PS4.RStickX()>100)   data[20]=0x01;
    if (PS4.RStickX()<-100)  data[21]=0x01;
    if (PS4.RStickY()>100)   data[22]=0x01;
    if (PS4.RStickY()<-100)  data[23]=0x01;

    previousMillis = currentMillis; // 前回の時間を更新
  }

  // 0.5秒経過したかどうかをチェック
  if (currentMillis - previousMillis >= interval) {
    // 0.5秒経過したら特定の配列をすべて0にする
    all_clear();
  }
}

void all_clear(){
  for(int i=0;i<24;i++){
    data[i]=0;
  }
}


// dxl -------------------------------------------


/**
 * Dynamixelの割り当て番号(id)
 * 手先：1 (位置制御)
 * 昇降：2
 * 関節：3
 * 台座回転：4
*/

// IDは，モータ個別にDynamixel Wizard2で割り振ってください．
Dxl Dxl_1(1,&Serial2);
Dxl Dxl_2(2,&Serial2);
Dxl Dxl_3(3,&Serial2);
Dxl Dxl_4(4,&Serial2);
Dxl Dxl_5(5,&Serial2);

//dxl1の状態格納変数
int dxl1_nowout_index=0;
int state_dxl1=0;
int last_state_dxl1=0;

//ID:1の位置[close,open]
const int DXL1_POSITION[2]={2053,1932};
//駆動速度[非割り当て、手首、昇降、関節、台座]
const int MAX_DXL_SPEED[5]={0,100,100,100,100};
//id1(手先)のボタン
const int BUTTON_ID1=0;
//id2(手首)のボタン[右移動、左移動]
const int BUTTON_ID2[2]={4,6};
//id3(昇降)のボタン[上移動、下移動]
const int BUTTON_ID3[2]={7,5};
//id4(関節)のボタン[右回転、左回転]
const int BUTTON_ID4[2]={16,17};
//id5(台座回転)のボタン[右回転、左回転]
const int BUTTON_ID5[2]={20,21};


void dxl_setup(){
  delay(200);
  // 制御設定
  //1   Velocity Control Mode
  //3   Position Control Mode
  //4   Extended Position Control Mode
  Dxl_1.servo_control(3); 
  Dxl_2.servo_control(1); 
  Dxl_3.servo_control(1); 
  Dxl_4.servo_control(1);
  Dxl_5.servo_control(1);
  Dxl_1.servo_torque(1);
  Dxl_2.servo_torque(1);
  Dxl_3.servo_torque(1);
  Dxl_4.servo_torque(1);
  Dxl_5.servo_torque(1);
}

void debug_dxl_input(){
  data[BUTTON_ID1]=1;
}

unsigned long last_trq_on = 0; // 前回の時間を格納する変数

void dxl_alltrq_on() {
  unsigned long nowtime = millis(); // 現在の時間を取得

  // 1秒経過したかどうかをチェック
  if (nowtime - last_trq_on >= 1000) {
    Dxl_1.servo_torque(1);
    Dxl_2.servo_torque(1);
    Dxl_3.servo_torque(1);
    Dxl_4.servo_torque(1);
    Dxl_5.servo_torque(1);
    last_trq_on = nowtime; // 前回の時間を更新
  }
}



void dxl_output(){

  state_dxl1= data[BUTTON_ID1];
  // ボタンが押されたかどうかをチェック
  if (state_dxl1 != last_state_dxl1) {
    // ボタンが押されたときの処理
    if (state_dxl1 == 1) {
      dxl1_nowout_index=1-dxl1_nowout_index;
      Dxl_1.servo_position(DXL1_POSITION[dxl1_nowout_index]);
    }
    // ボタンの状態を更新
    last_state_dxl1 = state_dxl1 ;
  }

  //手首右入力
  if(data[BUTTON_ID2[0]]){
    Dxl_2.servo_speed(MAX_DXL_SPEED[1]);
  }
  //手首左入力
  if(data[BUTTON_ID2[1]]){
    Dxl_2.servo_speed(-MAX_DXL_SPEED[1]);
  }
  if(data[BUTTON_ID2[0]]==0 && data[BUTTON_ID2[1]]==0){
    Dxl_2.servo_speed(0);
  }


  //昇降上入力
  if(data[BUTTON_ID3[0]]){
    Dxl_3.servo_speed(MAX_DXL_SPEED[2]);
  }
  //昇降下入力
  if(data[BUTTON_ID3[1]]){
    Dxl_3.servo_speed(-MAX_DXL_SPEED[2]);
  }
  if(data[BUTTON_ID3[0]]==0 && data[BUTTON_ID3[1]]==0){
    Dxl_3.servo_speed(0);
  }

  //関節右入力
  if(data[BUTTON_ID4[0]]){
    Dxl_4.servo_speed(MAX_DXL_SPEED[3]);
  }
  //関節左入力
  if(data[BUTTON_ID4[1]]){
    Dxl_4.servo_speed(-MAX_DXL_SPEED[3]);
  }
  if(data[BUTTON_ID4[0]]==0 && data[BUTTON_ID4[1]]==0){
    Dxl_4.servo_speed(0);
  }

  //台座回転右入力
  if(data[BUTTON_ID5[0]]){
    Dxl_5.servo_speed(-MAX_DXL_SPEED[4]);
  }
  //台座回転左入力
  if(data[BUTTON_ID5[1]]){
    Dxl_5.servo_speed(MAX_DXL_SPEED[4]);
  }
  if(data[BUTTON_ID5[0]]==0 && data[BUTTON_ID5[1]]==0){
    Dxl_5.servo_speed(0);
  }
}


#include <Arduino.h>
#include <HardwareSerial.h>
#include <Dxl.h>


// COMMUNICATION ----------------------------------------------


byte data[24]={0x00};
unsigned long previousMillis = 0;
const long interval = 500;

void receive(HardwareSerial *ser){
  unsigned long currentMillis = millis(); // 現在の時間を取得

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

          byte num=0x00;
          for(;ser->available()>0;){
            num=ser->read();
          }
          previousMillis = currentMillis; // 前回の時間を更新
        }
    }
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

void debug_dataprint(HardwareSerial *ser){
  for(int i=0;i<24;i++){
    ser->print(data[i]);
    ser->print(" ");
  }
  ser->println("");
}




// AIR --------------------------------------------------------
/**
 * エアシリ番号
 * 指3本…0番
 * 手首変形…1番
 * 
*/
//エアシリのボタン[指3本,手首変形]
const int BUTTON_AIR[2]={0,2};
//エアシリのdigital出力ピン番号
const int PIN_AIR[2]={13,24};
//エアシリの状態格納変数
int state_air[2]={0};
int last_state_air[2]={0};

void air_setup(){
  pinMode(PIN_AIR[0],OUTPUT);
  pinMode(PIN_AIR[1],OUTPUT);
}

void air_output(){
  // ボタンの状態を読み取り
  state_air[0]= data[BUTTON_AIR[0]];
  // ボタンが押されたかどうかをチェック
  if (state_air[0] != last_state_air[0]) {
    // ボタンが押されたときの処理
    if (state_air[0] == HIGH) {
      digitalWrite(PIN_AIR[0],!digitalRead(PIN_AIR[0]));
    }
    // ボタンの状態を更新
    last_state_air[0] = state_air[0] ;
  }
  state_air[1]= data[BUTTON_AIR[1]];
  if (state_air[1] != last_state_air[1]) {
    if (state_air[1] == HIGH) {
      digitalWrite(PIN_AIR[1],!digitalRead(PIN_AIR[1]));
    }
    last_state_air[1] = state_air[1] ;
  }

}

// DCMotor ----------------------------------------------------
/**
 * モーター番号
 * 横移動…0番
 * 前後移動…1番
 * 台座回転…2番
*/
//モーターのボタン[右移動,左移動,前移動,後移動,時計回転,反時計回転]
const int BUTTON_MOTOR[6]={16,17,18,19,20,21};
//モーターのDIR,PWM出力ピン番号
const int PIN_MDIR[3]={30,32,34};
const int PIN_MPWM[3]={2,3,5};
//モーターの回転速度
const int MAX_SPEED[3]={100,50,50};

void motor_setup(){
  pinMode(PIN_MDIR[0],OUTPUT);
  pinMode(PIN_MDIR[1],OUTPUT);
  pinMode(PIN_MDIR[2],OUTPUT);
  
  //PWM周波数変更(2,3,5が変更される)
  TCCR3B = (TCCR3B & 0b11111000) | 0b00000001;

}

void motor_output(){
  //左右移動
  int tmp_dir0 = data[BUTTON_MOTOR[0]];//この時MDはHIGHで正回転命令→右移動
  int tmp_pwm0 = MAX_SPEED[0]*(data[BUTTON_MOTOR[0]]|data[BUTTON_MOTOR[1]]);
  digitalWrite(PIN_MDIR[0],tmp_dir0);
  analogWrite(PIN_MPWM[0],tmp_pwm0);
  //前後移動
  int tmp_dir1 = data[BUTTON_MOTOR[2]];//この時MDはHIGHで正回転命令→前移動
  int tmp_pwm1 = MAX_SPEED[1]*(data[BUTTON_MOTOR[2]]|data[BUTTON_MOTOR[3]]);
  digitalWrite(PIN_MDIR[1],tmp_dir1);
  analogWrite(PIN_MPWM[1],tmp_pwm1);
  //回転
  int tmp_dir2 = data[BUTTON_MOTOR[4]];//この時MDはHIGHで正回転命令→時計回転
  int tmp_pwm2 = MAX_SPEED[2]*(data[BUTTON_MOTOR[4]]|data[BUTTON_MOTOR[5]]);
  digitalWrite(PIN_MDIR[2],tmp_dir2);
  analogWrite(PIN_MPWM[2],tmp_pwm2);
}

// PHYSICAL SWITCH --------------------------------------------

/**
 * 物理スイッチ
 * あるスイッチが押されたとき、指定のボタンの命令(data)を0にする
 * ※スイッチが押されたとき、入力ピンはHIGHとなるように配線する
*/

//スイッチの数
const int NUM_SWITCH=0;
//無効化したいボタンとその入力ピン番号（最後の要素として{0,0}を加えておくこと）
const int BUTTON_PIN_SWITCH[NUM_SWITCH+1][2]={{0,0}};


void switch_setup(){
  for(int i=0;i<NUM_SWITCH;i++){
    pinMode(BUTTON_PIN_SWITCH[i][1],INPUT);
  }
}

void switch_input(){
  for(int i=0;i<NUM_SWITCH;i++){
    if(digitalRead(BUTTON_PIN_SWITCH[i][1])==1){
      data[BUTTON_PIN_SWITCH[i][0]]=0;
    }
  }
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
const int MAX_DXL_SPEED[2]={100,100};
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

void debug_dxl_input(){
  data[BUTTON_ID1[0]]=1;
}

void dxl_output(){
  //昇降上入力
  if(data[BUTTON_ID1[0]]){
    Dxl_1.servo_speed(MAX_DXL_SPEED[0]);
    delay(50);
  }
  //昇降下入力
  if(data[BUTTON_ID1[1]]){
    Dxl_1.servo_speed(-MAX_DXL_SPEED[0]);
    delay(50);
  }
  if(data[BUTTON_ID1[0]]==0 && data[BUTTON_ID1[1]]==0){
    Dxl_1.servo_speed(0);
    delay(50);
  }

  //手首右入力
  if(data[BUTTON_ID2[0]]){
    Dxl_2.servo_speed(MAX_DXL_SPEED[1]);
    delay(50);
  }
  //手首左入力
  if(data[BUTTON_ID2[1]]){
    Dxl_2.servo_speed(-MAX_DXL_SPEED[1]);
    delay(50);
  }
  if(data[BUTTON_ID2[0]]==0 && data[BUTTON_ID2[1]]==0){
    Dxl_2.servo_speed(0);
    delay(50);
  }
}


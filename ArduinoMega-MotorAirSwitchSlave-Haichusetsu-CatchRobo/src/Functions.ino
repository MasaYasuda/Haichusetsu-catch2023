#include <Arduino.h>
#include <HardwareSerial.h>



// COMMUNICATION ----------------------------------------------


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
const int PIN_AIR[2]={22,24};
//エアシリの状態格納変数
int state_air[2]={0};

void air_setup(){
  pinMode(PIN_AIR[0],OUTPUT);
  pinMode(PIN_AIR[1],OUTPUT);
}

void air_output(){
  if(data[BUTTON_AIR[0]]==1){
    state_air[0]=1-state_air[0];
    digitalWrite(PIN_AIR[0],state_air[0]);
    delay(50);
  }
  if(data[BUTTON_AIR[1]]==1){
    state_air[1]=1-state_air[1];
    digitalWrite(PIN_AIR[1],state_air[1]);
    delay(50);
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
const int MAX_SPEED[3]={50,50,50}

void motor_setup(){
  pinMode(PIN_MDIR[0],OUTPUT);
  pinMode(PIN_MDIR[1],OUTPUT);
  pinMode(PIN_MDIR[2],OUTPUT);
  
  //PWM周波数変更(2,3,5が変更される)
  //TCCR3B = (TCCR3B & 0b11111000) | 0b00000001;

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
const int NUM_SWITCH=0
//無効化したいボタンとその入力ピン番号（最後の要素として{0,0}を加えておくこと）
const int BUTTON_PIN_SWITCH[NUM_SWITCH+1][2]={{0,0}};


void switch_setup(){
  for(int i=0;i<NUM_SWITCH;i++){
    pinMode(BUTTON_PIN_SWITCH[i,1],INPUT);
  }
}

void switch_input(){
  for(int i=0;i<NUM_SWITCH;i++){
    if(digitalRead(BUTTON_PIN_SWITCH[i,1])==1){
      data[BUTTON_PIN_SWITCH[i,0]]=0;
    }
  }
}






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

void air_setup(){
  pinMode(PIN_AIR[0],OUTPUT);
  pinMode(PIN_AIR[1],OUTPUT);
}

void air_output(){
  digitalWrite(PIN_AIR[0],data[BUTTON_AIR[0]])
  digitalWrite(PIN_AIR[1],data[BUTTON_AIR[1]])
}

// DCMotor ----------------------------------------------------
/**
 * モーター番号
 * 横移動…0番
 * 前後移動…1番
 * 台座回転…2番
*/
//モーターのボタン[右移動,左移動,前移動,後移動,時計回り回転,反時計回り回転]
const int BUTTON_MOTOR[6]={16,17,18,19,20,21};
//モーターのDIR出力ピン番号
const int PIN_MDIR[3]={30,32,34};
const int PIN_MDIR[3]={2,3,5};

void motor_setup(){
  pinMode(BUTTON_AIR[0],OUTPUT);
  pinMode(BUTTON_AIR[1],OUTPUT);
}

void motor_output(){
  digitalWrite(PIN_AIR[0],data[BUTTON_AIR[0]])
  digitalWrite(PIN_AIR[1],data[BUTTON_AIR[1]])
}

// PHYSICAL SWITCH --------------------------------------------
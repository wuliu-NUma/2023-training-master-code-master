/*
同步写例子,舵机出厂速度单位为0.0146rpm，舵机机运行速度V=2400
*/

#include <SCServo.h>
SMS_STS sms_sts;

byte ID[2];
s16 Position[2];
u16 Speed[2];
byte ACC[2];

void setup()
{
  Serial1.begin(115200);//sms_stss舵机波特率115200
  //Serial1.begin(1000000);//sts舵机波特率1000000
  sms_sts.pSerial = &Serial1;
  delay(1000);
  ID[0] = 1;
  ID[1] = 2;
  Speed[0] = 2400;
  Speed[1] = 2400;
  ACC[0] = 50;
  ACC[1] = 50;
}

void loop()
{
  Position[0] = 4095;
  Position[1] = 4095;
  sms_sts.SyncWritePosEx(ID, 2, Position, Speed, ACC);//舵机(ID1/ID2)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P1=4095位置
  delay(2240);//((P1-P0)/V)*1000+(V/(A*100))*1000

  Position[0] = 0;
  Position[1] = 0;
  sms_sts.SyncWritePosEx(ID, 2, Position, Speed, ACC);//舵机(ID1/ID2)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P0=0位置
  delay(2240);//((P1-P0)/V)*1000+(V/(A*100))*1000
}

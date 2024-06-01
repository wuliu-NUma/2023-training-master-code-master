/*
异步写例子，舵机出厂速度单位为0.0146rpm，舵机机运行速度V=2400
*/

#include <SCServo.h>

SMS_STS sms_sts;

void setup()
{
  Serial1.begin(115200);//sms舵机波特率115200
  //Serial1.begin(1000000);//sts舵机波特率1000000
  sms_sts.pSerial = &Serial1;
  delay(1000);
}

void loop()
{
  sms_sts.RegWritePosEx(1, 4095, 2400, 50);//舵机(ID1)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P1=4095位置
  sms_sts.RegWritePosEx(2, 4095, 2400, 50);//舵机(ID2)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P1=4095位
  sms_sts.RegWriteAction();
  delay(2240);//[(P1-P0)/V]*1000+[V/(A*100)]*1000

  sms_sts.RegWritePosEx(1, 0, 2400, 50);//舵机(ID1)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P0=0位置
  sms_sts.RegWritePosEx(2, 0, 2400, 50);//舵机(ID2)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P1=0位置
  sms_sts.RegWriteAction();
  delay(2240);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
}

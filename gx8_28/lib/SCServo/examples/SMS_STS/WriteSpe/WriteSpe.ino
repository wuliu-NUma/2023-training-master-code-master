/*
恒速模式例子,速度单位为0.0146rpm，舵机机运行速度V=2400
*/

#include <SCServo.h>

SMS_STS sms_sts;

void setup()
{
  Serial1.begin(115200);//sms舵机波特率115200
  //Serial1.begin(1000000);//sts舵机波特率1000000
  sms_sts.pSerial = &Serial1;
  delay(1000);
  sms_sts.WheelMode(1);//恒速模式
}

void loop()
{
  sms_sts.WriteSpe(1, 2400, 50);//舵机(ID1)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，旋转
  delay(4000);
  sms_sts.WriteSpe(1, 0, 50);//舵机(ID1)以加速度A=50(50*100步/秒^2)，停止旋转(V=0)
  delay(2000);
  sms_sts.WriteSpe(1, -2400, 50);//舵机(ID1)以最高速度V=-2400步/秒，加速度A=50(50*100步/秒^2)，反向旋转
  delay(4000);
  sms_sts.WriteSpe(1, 0, 50);//舵机(ID1)以加速度A=50(50*100步/秒^2)，停止旋转(V=0)
  delay(2000);
}

#include <Arduino.h>
#include <JY901.h>
#include "motor.h"
#include <Wire.h>

u8 cmd[5] = {0x5a, 0x05, 0x00, 0x01, 0x60};
int lidar_y;
int lidar_x;
int lidar_err_y;
int lidar_err_x;



float gyroscope;
float gyroscope_err1;


float Target_XA = 0.0;
float Target_XB = 0.0;
float Target_YA = 0.0;
float Target_YB = 0.0;

float Gray_WheelXA;
float Gray_WheelXB;
float Gray_WheelYA;
float Gray_WheelYB;

float Gray_WheelXA2;
float Gray_WheelXB2;
float Gray_WheelYA2;
float Gray_WheelYB2;

float Gray_WheelXA3;
float Gray_WheelXB3;
float Gray_WheelYA3;
float Gray_WheelYB3;

int Get_LidarDatafromIIC(byte address) {
  char i = 0;
  char j = 0;
  byte rx_buf[9] = {0};
  Wire.beginTransmission(address); /* Begin a transmission to the I2C Slave device with the given address. */
  for (j = 0; j < 5; j++)
  {
    Wire.write(cmd[j]);
  }
  Wire.endTransmission(0);  /* Send a START Sign */
  Wire.requestFrom(address, 9); /* request 7 bytes from slave device address */
  while ( Wire.available())
  {
    rx_buf[i] = Wire.read(); /* received one byte */
    i++;
    if (i >= 9)
    {
      i = 0;
    }
  }
  return rx_buf[3] * 256 + rx_buf[2];
}

void lidar_err1_Correct(int distance_x1){
  lidar_err_x = 0;
  lidar_x = Get_LidarDatafromIIC(0x10);
  lidar_err_x = lidar_x - distance_x1;
  lidar_err_x = lidar_err_x*1.5;
  
  if(lidar_err_x > 70){
  lidar_err_x = 70;
  }
  if(lidar_err_x < -70){
  lidar_err_x = -70;
  }
  // if(lidar_err_x < 5 && lidar_err_x > 0){
  // lidar_err_x = 5;
  // }
  //   if(lidar_err_x < 5 && lidar_err_x > 0){
  // lidar_err_x = 5;
  // }

 // Serial.println(lidar_err_y);
  Gray_WheelXA = lidar_err_x;
  Gray_WheelXB = lidar_err_x;
  Gray_WheelYA = lidar_err_x;
  Gray_WheelYB = lidar_err_x;
  
}

void lidar_err2_Correct(int distance_y1){
  lidar_err_y = 0;
  lidar_y = Get_LidarDatafromIIC(0x20);
  lidar_err_y = lidar_y - distance_y1;
  lidar_err_y = lidar_err_y*1.5;

  if(lidar_err_y > 70){
  lidar_err_y = 70;
  }
  if(lidar_err_y < -70){
  lidar_err_y = -70;
  }
  //Serial.println(lidar_err_y);
  Gray_WheelXA2 = lidar_err_y;
  Gray_WheelXB2 = -lidar_err_y;
  Gray_WheelYA2 = -lidar_err_y;
  Gray_WheelYB2 = lidar_err_y;
  //delay(30);
}

 void gyroscope_err1_Correct(float Angle_1)
{
  gyroscope_err1 = 0.0;
  gyroscope = Angle_Read();
  gyroscope_err1 = gyroscope - Angle_1;
  gyroscope_err1 = gyroscope_err1*1;

  if(gyroscope_err1 > 20){
  gyroscope_err1 = 20;
  }
  if(gyroscope_err1 < -20){
  gyroscope_err1 = -20;
  }
  if(gyroscope_err1 > 0.8 && gyroscope_err1 < 1){
  gyroscope_err1 = 1;
  }
  if(gyroscope_err1 < -0.8 && gyroscope_err1 > -1){
  gyroscope_err1 = -1;
  }
  //Serial.println(gyroscope_err1);
  Gray_WheelXA3 = gyroscope_err1;
  Gray_WheelXB3 = -gyroscope_err1;
  Gray_WheelYA3 = gyroscope_err1;
  Gray_WheelYB3 = -gyroscope_err1;
}





void control(int distance_x, int distance_y, float Angle)
{
  gyroscope_err1_Correct(Angle);
  lidar_err1_Correct(distance_x);
  lidar_err2_Correct(distance_y);

  Target_XA = (Gray_WheelXA + Gray_WheelXA2 + Gray_WheelXA3);
  Target_XB = (Gray_WheelXB + Gray_WheelXB2 + Gray_WheelXB3);
  Target_YA = (Gray_WheelYA + Gray_WheelYA2 + Gray_WheelYA3);
  Target_YB = (Gray_WheelYB + Gray_WheelYB2 + Gray_WheelYB3);
  
  //Serial.println(Target_XA);
  setPWM(Target_XA,Target_XB,Target_YA,Target_YB);
  delay(50);
}

void control_y(int distance_y,int Angle)
{
  gyroscope_err1_Correct(Angle);
  lidar_err2_Correct(distance_y);

  Target_XA = (Gray_WheelXA + Gray_WheelXA2 + Gray_WheelXA3);
  Target_XB = (Gray_WheelXB + Gray_WheelXB2 + Gray_WheelXB3);
  Target_YA = (Gray_WheelYA + Gray_WheelYA2 + Gray_WheelYA3);
  Target_YB = (Gray_WheelYB + Gray_WheelYB2 + Gray_WheelYB3);
  
  //Serial.println(Target_XA);
  setPWM(Target_XA,Target_XB,Target_YA,Target_YB);
  delay(50);
}

void control_angle(float Angle)
{
  gyroscope_err1_Correct(Angle);

  Target_XA = (Gray_WheelXA3);
  Target_XB = (Gray_WheelXB3);
  Target_YA = (Gray_WheelYA3);
  Target_YB = (Gray_WheelYB3);
  
  //Serial.println(Target_XA);
  setPWM(Target_XA,Target_XB,Target_YA,Target_YB);
  delay(50);
}
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>


SoftwareSerial softSerial(69,68); /* 定义软串口名称为Serial1，并把pin2定为RX，pin3定为TX */

#define SCREEN_WIDTH 128 // 设置OLED宽度,单位:像素
#define SCREEN_HEIGHT 64 // 设置OLED高度,单位:像素

#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned char k210_0[256]= {0xAA,0xAA,0x01,0x00,0x55};
unsigned char k210_1[256]= {0xAA,0xAA,0x01,0x01,0x56};
unsigned char k210_2[256]= {0xAA,0xAA,0x01,0x02,0x57};
unsigned char k210_3[256]= {0xAA,0xAA,0x01,0x03,0x58};
unsigned char k210_4[256]= {0xAA,0xAA,0x01,0x04,0x59};
unsigned char k210_5[256]= {0xAA,0xAA,0x01,0x05,0x5A};
unsigned char k210_6[256]= {0xAA,0xAA,0x01,0x06,0x5B};
unsigned char k210_7[256]= {0xAA,0xAA,0x01,0x07,0x5C};
unsigned char k210_8[256]= {0xAA,0xAA,0x01,0x08,0x5D};
unsigned char k210_9[256]= {0xAA,0xAA,0x01,0x09,0x5E};


int erweima1;/* 雷达实测距离值 */
int erweima2;/* 雷达信号强度 */
int erweima3;/* 雷达实测距离值 */
int erweima4;/* 雷达信号强度 */

int color_x = 0;
int color_read = 0;
int color_err = 0;
int color_state = 0;

int color_read2 = 0;
int color_err2 = 0;
int color_state2 = 0;

int color_1 = 0;
int color_2 = 0;
int color_3 = 0;
int color_4 = 0;
int color_5 = 0;
int color_6 = 0;
int xxx = 0;
int yyy = 0;

int ring_x1 = 0;
int ring_y1 = 0;
int lx_x = 0;

int check;/* 校验数值存放 */
int i;
int k210[9];/* 存放雷达测量的数据 */
const int HEADER=0xAA;/* 数据包帧头 */


void K210_fasong(int fasong1){
  if(fasong1 == 0){
    softSerial.write(k210_0,5);
  }
  if(fasong1 == 1){
    softSerial.write(k210_1,5);
  }
  if(fasong1 == 2){
    softSerial.write(k210_2,5);
  }
  if(fasong1 == 3){
    softSerial.write(k210_3,5);
  }
  if(fasong1 == 4){
    softSerial.write(k210_4,5);
  }
  if(fasong1 == 5){
    softSerial.write(k210_5,5);
  }
  if(fasong1 == 6){
    softSerial.write(k210_6,5);
  }
    if(fasong1 == 7){
    softSerial.write(k210_7,5);
  }
      if(fasong1 == 8){
    softSerial.write(k210_8,5);
  }
      if(fasong1 == 9){
    softSerial.write(k210_9,5);
  }
}

void words_display()
{
  // 清除屏幕
  display.clearDisplay();
 
  // 设置字体颜色,白色可见
  display.setTextColor(WHITE);
 
  //设置字体大小
  display.setTextSize(3.5);
 
  //设置光标位置
  display.setCursor(0, 20);
  display.print(erweima1);
  display.print("+");
  display.print(erweima2);
  //打印自开发板重置以来的秒数：
  display.display();
}

void read_k210_erweima2(){
  if (softSerial.available())/* 查看串口是否有数据输入 */
  {
    if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */ 
    { 
      k210[0]=HEADER;
      if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */
      {
        k210[1]=HEADER;
        for(i=2;i<9;i++)/* 存储数据到数组 */
        {
          k210[i]=softSerial.read();
        } 
        check=k210[0]+k210[1]+k210[2]+k210[3]+k210[4]+k210[5]+k210[6]+k210[7];
        if(k210[8]==(check&0xff))/* 按照协议对收到的数据进行校验 */
        {
          erweima3=k210[3]*256+k210[4];/* 计算距离值 */
          erweima4=k210[5]*256+k210[6];/* 计算信号强度值  */
          
          delay(100);
         }
      }
    }
  }

}

void read_k210_erweima(){
  if (softSerial.available())/* 查看串口是否有数据输入 */
  {
    if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */ 
    { 
      k210[0]=HEADER;
      if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */
      {
        k210[1]=HEADER;
        for(i=2;i<9;i++)/* 存储数据到数组 */
        {
          k210[i]=softSerial.read();
        } 
        check=k210[0]+k210[1]+k210[2]+k210[3]+k210[4]+k210[5]+k210[6]+k210[7];
        if(k210[8]==(check&0xff))/* 按照协议对收到的数据进行校验 */
        {
          erweima1=k210[3]*256+k210[4];/* 计算距离值 */
          erweima2=k210[5]*256+k210[6];/* 计算信号强度值  */
          Serial.print("erweima1 = ");
          Serial.print(erweima1);/* 输出雷达测试距离值 */
          Serial.println();
          Serial.print("erweima2 = ");
          Serial.print(erweima2);/* 输出雷达测试距离值 */
          Serial.println();
          
          delay(100);
         }
      }
    }
  }

}

void read_k210_color(){
  if (softSerial.available())/* 查看串口是否有数据输入 */
  {
    if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */ 
    { 
      k210[0]=HEADER;
      if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */
      {
        k210[1]=HEADER;
        for(i=2;i<9;i++)/* 存储数据到数组 */
        {
          k210[i]=softSerial.read();
        } 
        check=k210[0]+k210[1]+k210[2]+k210[3]+k210[4]+k210[5]+k210[6]+k210[7];
        if(k210[8]==(check&0xff))/* 按照协议对收到的数据进行校验 */
        {
          color_x=k210[7];
          // ring_x1=k210[3]*256+k210[4];/* 计算距离值 */
          // ring_y1=k210[5]*256+k210[6];/* 计算信号强度值  */

          delay(100);
         }
      }
    }
  }

}

void read_k210_ring(){
  if (softSerial.available())/* 查看串口是否有数据输入 */
  {
    if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */ 
    { 
      k210[0]=HEADER;
      if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */
      {
        k210[1]=HEADER;
        for(i=2;i<9;i++)/* 存储数据到数组 */
        {
          k210[i]=softSerial.read();
        } 
        check=k210[0]+k210[1]+k210[2]+k210[3]+k210[4]+k210[5]+k210[6]+k210[7];
        if(k210[8]==(check&0xff))/* 按照协议对收到的数据进行校验 */
        {
          ring_x1=k210[3]*256+k210[4];/* 计算距离值 */
          ring_y1=k210[5]*256+k210[6];/* 计算信号强度值  */
          // Serial.print("x = ");
          // Serial.print(ring_x1);/* 输出雷达测试距离值 */
          // Serial.println();
          // Serial.print("y = ");
          // Serial.print(ring_y1);/* 输出雷达测试距离值 */
          // Serial.println();
          delay(50);
         }
      }
    }
  }

}

void read_k210_l(){
  if (softSerial.available())/* 查看串口是否有数据输入 */
  {
    if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */ 
    { 
      k210[0]=HEADER;
      if(softSerial.read()==HEADER)/* 判断数据包帧头0x59 */
      {
        k210[1]=HEADER;
        for(i=2;i<9;i++)/* 存储数据到数组 */
        {
          k210[i]=softSerial.read();
        } 
        check=k210[0]+k210[1]+k210[2]+k210[3]+k210[4]+k210[5]+k210[6]+k210[7];
        if(k210[8]==(check&0xff))/* 按照协议对收到的数据进行校验 */
        {
          lx_x=k210[3]*256+k210[4];/* 计算距离值 */
          // if(lx_x>120){
          //   lx_x = 0;
          // }
          // Serial.print("x = ");
          // Serial.print(ring_x1);/* 输出雷达测试距离值 */
          // Serial.println();
          // Serial.print("y = ");
          // Serial.print(ring_y1);/* 输出雷达测试距离值 */
          // Serial.println();
          delay(50);
         }
      }
    }
  }

}



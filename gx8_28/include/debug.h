#include <Arduino.h>
#include <U8x8lib.h>
#include <SPI.h>
#include "lidar.h"
#include "pingmu.h"
#include "servo.h"
#ifdef U8X8_HAVE_HW_SPI
#endif
U8X8_SSD1309_128X64_NONAME0_4W_SW_SPI u8x8(52, 51, 53, 48 ,49);

int lidar_x_debug;
int lidar_y_debug;
int ring_errx = 0;
int ring_erry = 0;
int ring_speedx = 0;
int ring_speedy = 0;
int l_errx = 0;

int ring_eyes_dog1 = 0;
int ring_eyes_dog2 = 0;
int l_eyes_dog2 = 0;
int l_eyes_dog1 = 0;

void lidar_display()
{
  lidar_x_debug = Get_LidarDatafromIIC(0x10);
  lidar_y_debug = Get_LidarDatafromIIC(0x20);
  // 清除屏幕
  display.clearDisplay();
  // 设置字体颜色,白色可见
  display.setTextColor(WHITE);
  //设置字体大小
  display.setTextSize(2);
 
  //设置光标位置
  display.setCursor(0, 10);
  display.print("lidar_x:");
  display.print(lidar_x_debug);

  display.setCursor(0, 30);
  display.print("lidar_y:");
  //打印自开发板重置以来的秒数：
  display.print(lidar_y_debug);

  display.display();
  //delay(50);
}

void angle_display()
{
  
  // 清除屏幕
  display.clearDisplay();
  // 设置字体颜色,白色可见
  display.setTextColor(WHITE);
  //设置字体大小
  display.setTextSize(2);
 
  //设置光标位置
  display.setCursor(0, 10);
  display.print("angle:");
  display.print(Angle_Read());

  display.display();
  //delay(50);
}

void color_jud(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
    
if(color_x != 0){
    while (xxx == 0)
    {
      color_read = color_x;
      xxx = 1;
    }
    
    color_err = color_x - color_read;
    
    //Serial.println(color_err);
}
    
    if(color_x != 0 && color_err != 0 && color_state == 0){
        color_state = 1;
    }
    if(color_state == 1){
        Serial.println("进入颜色识别1");
        if(color_x == color_1){
          K210_fasong(0);
          zhuaqu_color1();
          color_state = 2;
        }
    }
    if(color_state == 2){
        Serial.println("进入颜色识别2");
        if(color_x == color_2){
          K210_fasong(0);
          zhuaqu_color2();
          color_state = 3;
        }
    }
    if(color_state == 3){
        Serial.println("进入颜色识别3");
        if(color_x == color_3){
          K210_fasong(0);
          zhuaqu_color3();
          color_state = 4;
          
        }
    }

}

void color_jud2(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
    
if(color_x != 0){
    while (yyy == 0)
    {
      color_read2 = color_x;
      yyy = 1;
    }
    
    color_err2 = color_x - color_read2;
    
    //Serial.println(color_err);
}
    
    if(color_x != 0 && color_err2 != 0 && color_state2 == 0){
        color_state2 = 1;
    }
    if(color_state2 == 1){
        Serial.println("进入颜色识别1");
        if(color_x == color_4){
          K210_fasong(0);
          zhuaqu_color1();
          color_state2 = 2;
        }
    }
    if(color_state2 == 2){
        Serial.println("进入颜色识别2");
        if(color_x == color_5){
          K210_fasong(0);
          zhuaqu_color2();
          color_state2 = 3;
        }
    }
    if(color_state2 == 3){
        Serial.println("进入颜色识别3");
        if(color_x == color_6){
          K210_fasong(0);
          zhuaqu_color3();
          color_state2 = 4;
          
        }
    }

}



void ring_err_xy(int ring_x1err,int ring_y1err){

   ring_errx = ring_x1err - ring_x1;
   ring_erry = ring_y1err - ring_y1;

  if((ring_errx > 20) || (ring_errx < -20)){
    ring_speedx = ring_errx/10;
  }
  if((ring_erry > 20) || (ring_erry < -20)){
    ring_speedy = ring_erry/10;
  }
  
  if((ring_errx <= 20) && (ring_errx >= 5)){
    ring_speedx = 1;  
  }
  if((ring_erry <= 20) && (ring_erry >= 5)){
    ring_speedy = 1;
  }

  if((ring_errx <= -5) && (ring_errx >= -20)){
    ring_speedx = -1;
  }

  if((ring_erry <= -5) && (ring_erry >= -20)){
    ring_speedy = -1;
  }
  
  if((ring_errx < 5) && (ring_errx > -5)){
    ring_speedx = 0;
  }

  if((ring_erry < 5) && (ring_erry > -5)){
   ring_speedy = 0;
  }

  //Serial.println(ring_errx);

  Gray_WheelXA = ring_speedx;
  Gray_WheelXB = -ring_speedx;
  Gray_WheelYA = -ring_speedx;
  Gray_WheelYB = ring_speedx;

  Gray_WheelXA2 = -ring_speedy;
  Gray_WheelXB2 = -ring_speedy;
  Gray_WheelYA2 = -ring_speedy;
  Gray_WheelYB2 = -ring_speedy;
}

void control_ring(int distance_x, int distance_y)
{
      //debug_ring();

  if(ring_x1 != 0 || ring_y1 != 0){
  ring_err_xy(distance_x,distance_y);
  if(ring_speedx == 0 && ring_speedy == 0){
    ring_eyes_dog1++;
  }
  Target_XA = (Gray_WheelXA + Gray_WheelXA2);
  Target_XB = (Gray_WheelXB + Gray_WheelXB2);
  Target_YA = (Gray_WheelYA + Gray_WheelYA2);
  Target_YB = (Gray_WheelYB + Gray_WheelYB2);
  
  //delay(10);
  //Serial.println(Target_XA);
  setPWM(Target_XA,Target_XB,Target_YA,Target_YB);
  ring_x1 = 0;
  ring_y1 = 0;
  
  }
  else{
  move(1,1,0);
  move(2,1,0);
  move(3,1,0);
  move(4,1,0);
  }
  
}

int err_lx = -10;
void l_err_xy(int lx_2){
 // lx_x = lx_x + err_lx;

 

   l_errx = (lx_2 - lx_x)/5;

  
  //Serial.println(ring_errx);

  Gray_WheelXA = l_errx;
  Gray_WheelXB = -l_errx;
  Gray_WheelYA = l_errx;
  Gray_WheelYB = -l_errx;

}
void control_l(int lx_1)
{

  if(lx_x != 0){
  l_err_xy(lx_1);
  if(l_errx == 0){
   l_eyes_dog1++;
  }
  Target_XA = Gray_WheelXA;
  Target_XB = Gray_WheelXB;
  Target_YA = Gray_WheelYA;
  Target_YB = Gray_WheelYB;
  //delay(10);
  //Serial.println(Target_XA);
  setPWM(Target_XA,Target_XB,Target_YA,Target_YB);
  l_errx = 0;
  lx_x = 0;
  l_eyes_dog2++;
  }

}


void fangzhi_luoji(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
  if(color_1 == 1){
    zhuaqu_car1();
    yuanhuan_fang_right();
  }
  if(color_1 == 2){
    zhuaqu_car1();
    yuanhuan_fang_zhongjian();
  }
  if(color_1 == 3){
    zhuaqu_car1();
    
    yuanhuan_fang_left();
  }

  if(color_2 == 1){
    zhuaqu_car2();
    
    yuanhuan_fang_right();
  }
  if(color_2 == 2){
    zhuaqu_car2();
    yuanhuan_fang_zhongjian();
  }
  if(color_2 == 3){
    zhuaqu_car2();
   
    yuanhuan_fang_left();
  }

  if(color_3 == 1){
    zhuaqu_car3();
    yuanhuan_fang_right();
  }
  if(color_3 == 2){
    zhuaqu_car3();
    yuanhuan_fang_zhongjian();
  }
  if(color_3 == 3){
    zhuaqu_car3();
    
    yuanhuan_fang_left();
  }
}

void fangzhi_luoji2(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
  if(color_4 == 1){
    zhuaqu_car1();
    yuanhuan_fang_right();
  }
  if(color_4 == 2){
    zhuaqu_car1();
    yuanhuan_fang_zhongjian();
  }
  if(color_4 == 3){
    zhuaqu_car1();
    
    yuanhuan_fang_left();
  }

  if(color_5 == 1){
    zhuaqu_car2();
    
    yuanhuan_fang_right();
  }
  if(color_5 == 2){
    zhuaqu_car2();
    yuanhuan_fang_zhongjian();
  }
  if(color_5 == 3){
    zhuaqu_car2();
   
    yuanhuan_fang_left();
  }

  if(color_6 == 1){
    zhuaqu_car3();
    yuanhuan_fang_right();
  }
  if(color_6 == 2){
    zhuaqu_car3();
    yuanhuan_fang_zhongjian();
  }
  if(color_6 == 3){
    zhuaqu_car3();
    
    yuanhuan_fang_left();
  }
}

void fangzhi_luoji3(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
  if(color_4 == 1){
    zhuaqu_car1();
    yuanhuan_fang_right2();
  }
  if(color_4 == 2){
    zhuaqu_car1();
    yuanhuan_fang_zhongjian2();
  }
  if(color_4 == 3){
    zhuaqu_car1();
    
    yuanhuan_fang_left2();
  }

  if(color_5 == 1){
    zhuaqu_car2();
    
    yuanhuan_fang_right2();
  }
  if(color_5 == 2){
    zhuaqu_car2();
    yuanhuan_fang_zhongjian2();
  }
  if(color_5 == 3){
    zhuaqu_car2();
   
    yuanhuan_fang_left2();
  }

  if(color_6 == 1){
    zhuaqu_car3();
    yuanhuan_fang_right2();
  }
  if(color_6 == 2){
    zhuaqu_car3();
    yuanhuan_fang_zhongjian2();
  }
  if(color_6 == 3){
    zhuaqu_car3();
    
    yuanhuan_fang_left2();
  }
}


void zhuaqu_ring_luoji(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
  if(color_1 == 1){
    yuanhuan_zhua_right();
    fangzhi_car1();
  }
  if(color_1 == 2){
    yuanhuan_zhua_zhongjian();
    fangzhi_car1();
  }
  if(color_1 == 3){
    yuanhuan_zhua_left();
    fangzhi_car1();
  }

  if(color_2 == 1){
    yuanhuan_zhua_right();
    fangzhi_car2();
  }
  if(color_2 == 2){
    yuanhuan_zhua_zhongjian();
    fangzhi_car2();
  }
  if(color_2 == 3){
    yuanhuan_zhua_left();
    fangzhi_car2();
  }

  if(color_3 == 1){
    yuanhuan_zhua_right();
    fangzhi_car3();
  }
  if(color_3 == 2){
    yuanhuan_zhua_zhongjian();
    fangzhi_car3();
  }
  if(color_3 == 3){
    yuanhuan_zhua_left();
    fangzhi_car3();
  }
}





void zhuaqu_luoji_guosai(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

     if(color_1 == color_4){
    yuanhuan_zhua_left();
    fangzhi_car1();
  }
  if(color_1 == color_5){
    yuanhuan_zhua_zhongjian();
    fangzhi_car1();
  }
  if(color_1 == color_6){
    yuanhuan_zhua_right();
    fangzhi_car1();
  }

  if(color_2 == color_4){
    yuanhuan_zhua_left();
    fangzhi_car2();
  }
  if(color_2 == color_5){
    yuanhuan_zhua_zhongjian();
    fangzhi_car2();
  }
  if(color_2 == color_6){
    yuanhuan_zhua_right();
    fangzhi_car2();
  }

  if(color_3 == color_4){
    yuanhuan_zhua_left();
    fangzhi_car3();
  }
  if(color_3 == color_5){
    yuanhuan_zhua_zhongjian();
    fangzhi_car3();
  }
  if(color_3 == color_6){
    yuanhuan_zhua_right();
    fangzhi_car3();
  }

}
void zhuaqu_ring_luoji2(){
    color_1 = erweima1/100;
    color_2 = erweima1/10%10;
    color_3 = erweima1%10;

    color_4 = erweima2/100;
    color_5 = erweima2/10%10;
    color_6 = erweima2%10;
  if(color_4 == 1){
    yuanhuan_zhua_right();
    fangzhi_car1();
  }
  if(color_4 == 2){
    yuanhuan_zhua_zhongjian();
    fangzhi_car1();
  }
  if(color_4 == 3){
    yuanhuan_zhua_left();
    fangzhi_car1();
  }

  if(color_5 == 1){
    yuanhuan_zhua_right();
    fangzhi_car2();
  }
  if(color_5 == 2){
    yuanhuan_zhua_zhongjian();
    fangzhi_car2();
  }
  if(color_5 == 3){
    yuanhuan_zhua_left();
    fangzhi_car2();
  }

  if(color_6 == 1){
    yuanhuan_zhua_right();
    fangzhi_car3();
  }
  if(color_6 == 2){
    yuanhuan_zhua_zhongjian();
    fangzhi_car3();
  }
  if(color_6 == 3){
    yuanhuan_zhua_left();
    fangzhi_car3();
  }
}







void Gray_init(){
  Gray_WheelXA = 0;
  Gray_WheelXB = 0;
  Gray_WheelYA = 0;
  Gray_WheelYB = 0;
  Target_XA = 0;
Target_XB = 0;
Target_YA = 0;
Target_YB = 0;
}



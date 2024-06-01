#include <SCServo.h> 

SMS_STS sms_sts;
int speed_servo = 4000;
int speed_jiasu = 200;

void servo_init(){
  sms_sts.WritePosEx(1, 4090, speed_servo, speed_jiasu);//舵机(ID1)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P1=2048位置
  //delay(50);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(2, 1900, speed_servo, speed_jiasu);
  //delay(250);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(3, 2048, speed_servo, speed_jiasu);
  //delay(50);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(4, 2800, speed_servo, speed_jiasu);
  //delay(50);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(5, 2700, speed_servo, speed_jiasu);
  //delay(750);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
}

void servo_control(int servo1, int servo2, int servo3, int servo4, int servo5){
  sms_sts.WritePosEx(1, servo1, speed_servo, speed_jiasu);//舵机(ID1)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至P1=2048位置
  //delay(50);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(2, servo2, speed_servo, speed_jiasu);
  //delay(250);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(3, servo3, speed_servo, speed_jiasu);
  //delay(50);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(4, servo4, speed_servo, speed_jiasu);
  //delay(50);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
  sms_sts.WritePosEx(5, servo5, speed_servo, speed_jiasu);
  //delay(750);//[(P1-P0)/V]*1000+[V/(A*100)]*1000
}

void erweima_control(){
  servo_control(3040,1519,1406,2245,2400);
}

void color_control(){
  servo_control(3315,2152,2347,3242,2048);//2790
}

void color_control_yu(){
  servo_control(3315,1700,2004,2790,2048);
}


void ring_control(){
  servo_control(1998,2191,2356,3245,2048);
}



void zhuaqu_color1(){
  delay(100);
  servo_control(3269,2206,1995,2795,2048); //zhuaqu
  delay(600);

  servo_control(3269,2206,1995,2795,2700);// 闭合夹具
  delay(400);

  servo_control(3269,1800,2330,2969,2700);// 抬起
  delay(200);

  servo_control(3565,1800,2330,2969,2700);// 旋转云台
  delay(300);

  servo_control(3565,1934,1900,3070,2700);// 准备放置
  delay(400);

  servo_control(3565,1928,1733,2866,2700);// 放置   放置关键点
  delay(200);

  servo_control(3565,1928,1733,2866,2048);//松开夹具
  delay(200);

  servo_control(3565,1700,2004,2790,2048);// 抬起
  delay(100);

  color_control_yu();// 旋转回去
  delay(250);

  color_control();// 识别颜色
  delay(1000);
  
}

void fangzhi_car1(){
  servo_control(3565,1790,2330,2969,2700);// 旋转云台
  delay(800);

  servo_control(3565,1934,1900,3070,2700);// 准备放置
  delay(400);

  servo_control(3565,1928,1733,2866,2700);// 放置   放置关键点
  delay(400);

  servo_control(3565,1928,1733,2866,2048);//松开夹具
  delay(200);

  servo_control(3565,1700,2004,2790,2048);// 抬起
  delay(100);

}

void zhuaqu_color2(){
  delay(100);
  servo_control(3269,2206,1995,2795,2048); //zhuaqu
  delay(600);

  servo_control(3269,2206,1995,2795,2700);// 闭合夹具
  delay(400);

  servo_control(3269,1800,2330,2969,2700);// 抬起
  delay(200);

  servo_control(4041,1800,2330,2969,2700);// 旋转云台
  delay(400);

  servo_control(4041,1934,1900,3070,2700);// 准备放置
  delay(400);

  servo_control(4041,1916,1713,2908,2700);// 放置
  delay(200);

  servo_control(4041,1916,1713,2908,2048);//松开夹具
  delay(200);

  servo_control(4041,1700,2004,2790,2048);// 抬起
  delay(100);

  color_control_yu();// 旋转回去
  delay(250);

  color_control();// 识别颜色
  delay(1000);
  
}

void fangzhi_car2(){
  servo_control(4041,1790,2330,2969,2700);// 旋转云台
  delay(800);

  servo_control(4041,1934,1900,3070,2700);// 准备放置
  delay(400);

  servo_control(4041,1916,1713,2908,2700);// 放置
  delay(400);

  servo_control(4041,1916,1713,2908,2048);//松开夹具
  delay(200);

  servo_control(4034,1700,2004,2790,2048);// 抬起
  delay(100);
}

void zhuaqu_color3(){
  delay(100);
  servo_control(3269,2206,1995,2795,2048); //zhuaqu
  delay(600);

  servo_control(3269,2206,1995,2795,2700);// 闭合夹具
  delay(400);

  servo_control(3269,1800,2330,2969,2700);// 抬起
  delay(200);

  servo_control(419,1800,2330,2969,2700);// 旋转云台
  delay(1000);

  servo_control(419,1934,1900,3070,2700);// 准备放置
  delay(400);

  servo_control(419,1923,1716,2908,2700);// 放置
  delay(200);

  servo_control(419,1923,1716,2908,2048);//松开夹具
  delay(200);

  servo_control(419,1700,2004,2790,2048);// 抬起
  delay(100);

}

void fangzhi_car3(){
  servo_control(419,1790,2330,2969,2700);// 旋转云台
  delay(800);

  servo_control(419,1934,1900,3070,2700);// 准备放置
  delay(400);

  servo_control(419,1923,1716,2908,2700);// 放置
  delay(400);

  servo_control(419,1923,1716,2908,2048);//松开夹具
  delay(200);

  servo_control(419,1700,2004,2790,2048);// 抬起
  delay(100);
}

void zhuaqu_car1(){
  servo_control(3565,1790,2330,2969,2048);// 抬起旋转
  delay(800);

  servo_control(3565,1934,1900,3070,2048);// 准备抓取小车上的1号物料
  delay(200);

  servo_control(3565,1928,1733,2866,2048);// 抓取1903，1798，3000
  delay(100);

  servo_control(3565,1928,1733,2866,2700);// 夹具合拢
  delay(400);

  servo_control(3565,1934,1900,3070,2700);
  delay(200);

  servo_control(3565,1790,2330,2969,2700);// 抬起
  delay(100);

}

void zhuaqu_car2(){
  servo_control(4041,1790,2330,2969,2048);// 抬起旋转
  delay(800);

  servo_control(4041,1934,1900,3070,2048);// 准备抓取小车上的1号物料
  delay(200);

  servo_control(4041,1916,1713,2908,2048);// 抓取
  delay(100);

  servo_control(4041,1916,1713,2908,2700);// 夹具合拢
  delay(400);

  servo_control(4041,1934,1900,3070,2700);
  delay(200);

  servo_control(4041,1790,2330,2969,2700);// 抬起
  delay(100);

}

void zhuaqu_car3(){
  servo_control(419,1790,2330,2969,2048);// 抬起旋转
  delay(800);

  servo_control(419,1934,1900,3070,2048);// 准备抓取小车上的1号物料
  delay(200);

  servo_control(419,1923,1716,2908,2048);// 抓取
  delay(100);

  servo_control(419,1923,1716,2908,2700);// 夹具合拢
  delay(400);

  servo_control(419,1934,1900,3070,2700);
  delay(200);

  servo_control(419,1790,2330,2969,2700);// 抬起
  delay(100);

}

void yuanhuan_fang_zhongjian(){
  servo_control(1960,1790,2330,2969,2700);// 高空准备放置
  delay(800);
  
  servo_control(1960,2229,1771,2639,2700);// 过渡帧
  delay(600);

  servo_control(1960,2367,1776,2500,2700);// 放置
  delay(400);

  servo_control(1960,2367,1776,2500,2048);// 放置
  delay(400);

  servo_control(1960,1790,2330,2969,2048);// 抬起旋转
  delay(300);

}

void yuanhuan_zhua_zhongjian(){
  servo_control(1960,1790,2330,2969,2048);// 高空准备放置
  delay(800);
  
  servo_control(1960,2229,1771,2639,2048);// 过渡帧
  delay(600);

  servo_control(1960,2367,1776,2500,2048);// 放置
  delay(400);

  servo_control(1960,2367,1776,2500,2700);// 放置
  delay(400);

  servo_control(1960,1790,2330,2969,2700);// 抬起旋转
  delay(300);

}

void yuanhuan_fang_left(){
  servo_control(1538,1790,2330,2969,2700);// 高空准备放置
  delay(800);
  
  servo_control(1538,2269,1918,2618,2700);// 过渡帧
  delay(600);

  servo_control(1538,2523,2117,2660,2700);// 放置
  delay(400);

  servo_control(1538,2523,2117,2660,2048);// 松开
  delay(400);

  servo_control(1538,1790,2330,2969,2048);// 抬起旋转
  delay(300);

}

void yuanhuan_zhua_left(){
  servo_control(1538,1790,2330,2969,2048);// 高空准备放置
  delay(800);
  
  servo_control(1538,2269,1918,2618,2048);// 过渡帧
  delay(600);

  servo_control(1538,2523,2117,2660,2048);// 放置
  delay(400);

  servo_control(1538,2523,2117,2660,2700);// 松开
  delay(400);

  servo_control(1538,1790,2330,2969,2700);// 抬起旋转
  delay(300);

}

void yuanhuan_fang_right(){
  servo_control(2406,1790,2330,2969,2700);// 高空准备放置
  delay(800);
  
  servo_control(2406,2358,2043,2758,2700);// 过渡帧
  delay(600);

  servo_control(2406,2497,2083,2691,2700);// 放置
  delay(400);

  servo_control(2406,2497,2083,2691,2048);// 松开
  delay(400);

  servo_control(2406,1790,2330,2969,2048);// 抬起旋转
  delay(300);

}

void yuanhuan_zhua_right(){
  servo_control(2406,1790,2330,2969,2048);// 高空准备放置
  delay(800);
  
  servo_control(2406,2358,2043,2758,2048);// 过渡帧
  delay(600);

  servo_control(2406,2497,2083,2691,2048);// 放置
  delay(400);

  servo_control(2406,2497,2083,2691,2700);// 松开
  delay(400);

  servo_control(2406,1790,2330,2969,2700);// 抬起旋转
  delay(300);

}





void yuanhuan_fang_zhongjian2(){
  servo_control(1984,1790,2330,2969,2750);// 高空准备放置
  delay(800);
  
  servo_control(1984,2149,2034,2949,2750);// 过渡帧
  delay(800);

  servo_control(1984,2155,1904,2798,2750);// 放置
  delay(600);

  servo_control(1984,2155,1904,2798,2048);// 松开
  delay(600);

  servo_control(1984,1790,2330,2969,2048);// 抬起旋转
  delay(500);

}

void yuanhuan_fang_left2(){
  servo_control(1550,1790,2330,2969,2750);// 高空准备放置
  delay(800);
  
  servo_control(1550,2324,2290,2937,2750);// 过渡帧
  delay(800);

  servo_control(1550,2360,2209,2883,2750);// 放置
  delay(600);

  servo_control(1550,2360,2209,2883,2048);// 松开
  delay(600);

  servo_control(1550,1790,2330,2969,2048);// 抬起旋转
  delay(500);

}

void yuanhuan_fang_right2(){
  servo_control(2397,1790,2330,2969,2750);// 高空准备放置
  delay(800);
  
  servo_control(2397,2389,2399,3044,2750);// 过渡帧
  delay(800);

  servo_control(2397,2305,2117,2783,2750);// 放置
  delay(600);

  servo_control(2397,2305,2117,2783,2048);// 松开
  delay(600);

  servo_control(2397,1790,2330,2969,2048);// 抬起旋转
  delay(500);

}


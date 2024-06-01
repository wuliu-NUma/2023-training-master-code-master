#include <Arduino.h>
#include <SoftwareSerial.h>

#include "debug.h"
// put function declarations here:
// U8X8_SSD1309_128X64_NONAME2_SW_I2C u8x8(SCL, SDA, U8X8_PIN_NONE); 
int state = 0;
int angle_err_buchang = 3;

//main location
int tiaoshi = 6;  //main主控 0是雷达系统 1是颜色识别雷达调试 2是二维码识别雷达调试 3是识别圆环雷达调试  4是雷达跑图测试
                 // 6是初赛开环跑 7是开环跑颜色 8是开环跑图  决赛：10是主程序识别颜色 11是识别颜色抓取测试 12是开环抓取测试 13是开环程序





int erweima_location[2] = {158,26};  //1
int color_location[2] = {70,13};    //2
int chu_ring_location[2] = {25,94};  //4
int jing_zheng[2] = {97,179};    //6
int jing_zheng2[2] = {112,179};  //8
int jing_ring_location_first[2] = {25,98};  //7
int jing_ring_location_second[2] = {21,114};  //


//过渡 location
int color_chu_location[2] = {29,18};  //3
int chu_jing_location[2] = {31,178};  //5
int back9[2] = {205,179};//9
int back10[2] = {207,14};//10
int back11[2] = {215,2};//11


int init_lidar[2] = {0,0};

int ring_r = 82;
int ring_g = 82;
int state2 = 0;

int color_pulse = 4;//前进距离
int color_pulse2 = 4;//后退距离
int color_l1 = 30;
int color_l2 = 19;//缩进距离
int color_l3 = 19;//缩进距离




void lidar_initinit(){
    init_lidar[0] = Get_LidarDatafromIIC(0x10);
    init_lidar[1] = Get_LidarDatafromIIC(0x20);
}

void judgment_move(int x_judgment, int y_judgment, float angle_judgment, int err_judgment){
    int x_jud = 0;
    int y_jud = 0;
    float a_jud = 0;
    int errx_jud = 0;
    int erry_jud = 0;
    float erra_jud = 0;
    x_jud = Get_LidarDatafromIIC(0x10);
    y_jud = Get_LidarDatafromIIC(0x20);
    a_jud = Angle_Read();
    errx_jud = x_judgment - x_jud;
    erry_jud = y_judgment - y_jud;
    erra_jud = angle_judgment - a_jud;
    if(errx_jud < 0){
    errx_jud = -errx_jud;
    }
    if(erry_jud < 0){
    erry_jud = -erry_jud;
    }
    if(erra_jud < 0){
    erra_jud = -erra_jud;
    }

    if(errx_jud < err_judgment && erry_jud < err_judgment && erra_jud < 1){
    state = state + 1;
    }
}

void judgment_move_y(int y_judgment, float angle_judgment, int err_judgment){
  
    int y_jud = 0;
    float a_jud = 0;
    int erry_jud = 0;
    float erra_jud = 0;
    y_jud = Get_LidarDatafromIIC(0x20);
    a_jud = Angle_Read();
    erry_jud = y_judgment - y_jud;
    erra_jud = angle_judgment - a_jud;

    if(erry_jud < 0){
    erry_jud = -erry_jud;
    }
    if(erra_jud < 0){
    erra_jud = -erra_jud;
    }

    if( erry_jud < err_judgment && erra_jud < 1){
    state = state + 1;
    }
}

void judgment_move_erweima(int x_judgment, int y_judgment, float angle_judgment, int err_judgment){
    int x_jud = 0;
    int y_jud = 0;
    float a_jud = 0;
    int errx_jud = 0;
    int erry_jud = 0;
    float erra_jud = 0;
    x_jud = Get_LidarDatafromIIC(0x10);
    y_jud = Get_LidarDatafromIIC(0x20);
    a_jud = Angle_Read();
    errx_jud = x_judgment - x_jud;
    erry_jud = y_judgment - y_jud;
    erra_jud = angle_judgment - a_jud;
    if(errx_jud < 0){
    errx_jud = -errx_jud;
    }
    if(erry_jud < 0){
    erry_jud = -erry_jud;
    }
    if(erra_jud < 0){
    erra_jud = -erra_jud;
    }
    if(errx_jud < err_judgment && erry_jud < err_judgment && erra_jud < 1 && erweima1 != 0){
    state = state + 1;
    }

}

int color_dogdog = 0;
void color_judgment(){
  color_dogdog++;
 // Serial.println(color_dogdog);
  
  if(color_state == 4 || color_dogdog > 500){
    state = state + 1;
    color_state = 0;
    color_read = 0;
    color_x = 0;
    color_err = 0;
    xxx = 0;
    color_dogdog = 0;
        Gray_init();
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}

void color_judgment2(){
  color_dogdog++;
  if(color_state2 == 4 || color_dogdog > 450){
    state = state + 1;
    color_state2 = 0;
    color_read2 = 0;
    color_x = 0;
    color_err2 = 0;
    yyy = 0;
    color_dogdog = 0;
        Gray_init();
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}

void ring_judgment(){
  ring_eyes_dog2++;
  if(ring_eyes_dog1 > 5 || ring_eyes_dog2 > 70){
    stop();
    state = state + 1;
    ring_x1 = 0;
    ring_y1 = 0;
    ring_eyes_dog1 = 0;
    ring_eyes_dog2 = 0;
    Gray_init();
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}
void ring_judgment2(){
  ring_eyes_dog2++;
  if(ring_eyes_dog1 > 10 || ring_eyes_dog2 > 100){
    stop();
    state2 = state2 + 1;
    ring_x1 = 0;
    ring_y1 = 0;
    ring_eyes_dog1 = 0;
    ring_eyes_dog2 = 0;
    Gray_init();
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}
void l_judgment(){

  if(l_eyes_dog1 > 10 || l_eyes_dog2 > 50){
    stop();
    state = state + 1;
    lx_x = 0;
    l_eyes_dog1 = 0;
    l_eyes_dog2 = 0;
    Gray_init();
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}
int err_angle_dog = 0;
void angle_judgment(float angle_judgment){
  float a_jud = 0;
  float erra_jud = 0;
  err_angle_dog++;
  a_jud = Angle_Read();
  erra_jud = angle_judgment - a_jud;
      if(erra_jud < 0){
    erra_jud = -erra_jud;
    }
    if(erra_jud < 0.8 || err_angle_dog > 50){
      stop();
    state = state + 1;
    err_angle_dog = 0;
    while(softSerial.read() >= 0){}
    }
}


void move_with_jud(int x_movewithjud, int y_movewithjud, float a_movewithjud, int err_movewithjud){
    control(x_movewithjud,y_movewithjud,a_movewithjud);
    judgment_move(x_movewithjud,y_movewithjud,a_movewithjud,err_movewithjud);
}

long time_dangqian = 0;
void move_f(long move_f_distance){
 
  long move_f_con = 0;
  move_f_con = move_f_distance/0.00048438;

  moveto_pulse(1,1,70,move_f_con);
  moveto_pulse(2,0,70,move_f_con);
  moveto_pulse(3,1,70,move_f_con);
  moveto_pulse(4,0,70,move_f_con);

  state = state + 1;
  time_dangqian = millis();
}

void move_b(long move_f_distance){
time_dangqian = millis();
  long move_f_con = 0;
  move_f_con = move_f_distance/0.00048438;

  moveto_pulse(1,0,70,move_f_con);
  moveto_pulse(2,1,70,move_f_con);
  moveto_pulse(3,0,70,move_f_con);
  moveto_pulse(4,1,70,move_f_con);

  state = state + 1;
 time_dangqian = millis();
}

void move_le(long move_f_distance){

  long move_f_con = 0;
  move_f_con = move_f_distance/0.00048438;

  moveto_pulse(1,0,60,move_f_con);
  moveto_pulse(2,0,60,move_f_con);
  moveto_pulse(3,1,60,move_f_con);
  moveto_pulse(4,1,60,move_f_con);

  state = state + 1;
time_dangqian = millis();
}

void move_ri(long move_f_distance){

  long move_f_con = 0;
  move_f_con = move_f_distance/0.00048438;

  moveto_pulse(1,1,60,move_f_con);
  moveto_pulse(2,1,60,move_f_con);
  moveto_pulse(3,0,60,move_f_con);
  moveto_pulse(4,0,60,move_f_con);

  state = state + 1;
 time_dangqian = millis();
}

void move_xy(long move_f_distance){

  long move_f_con = 0;
  move_f_con = move_f_distance/0.00048438;

  moveto_pulse(1,0,50,move_f_con);
  moveto_pulse(4,1,50,move_f_con);

  state = state + 1;
time_dangqian = millis();
}

void erweima_judddd(){
  if(erweima1 != 0 && erweima2 != 0){
    state = state + 1;
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}
void erweima_judddd2(){
  if(erweima3 != 0 && erweima4 != 0){
    state = state + 1;
    K210_fasong(0);
    while(softSerial.read() >= 0){}
  }
}

int motor4_read[3];

void read_motor4(long move_f_distance){
  long time_forward = 0;
  long time_err = 0;
  time_err = millis() - time_dangqian;
  time_forward = move_f_distance/0.02 + 6000;
  if(time_err > time_forward){
    state++;
  }
  if (Serial1.available())/* 查看串口是否有数据输入 */
  {
    if(Serial1.read()==0x04)/* 判断数据包帧头0x59 */ 
    { 
      motor4_read[0]=0x01;
      if(Serial1.read()==0x9F)/* 判断数据包帧头0x59 */
      {
        motor4_read[1]=0x9F;
       if(Serial1.read()==0x6B)/* 判断数据包帧头0x59 */
       {
        motor4_read[1]=0x6B;
        state++;
       }
      }
    }
  }
}
void zhuanpan_wuliao(int pwm1){
  setPWM(pwm1,pwm1,pwm1,pwm1);
  lidar_y_debug = Get_LidarDatafromIIC(0x20);
  if(lidar_y_debug < color_l1){
    stop();
    state++;
  }
}

void SPI_pingmu_all(){
  lidar_x_debug = Get_LidarDatafromIIC(0x10);
  lidar_y_debug = Get_LidarDatafromIIC(0x20);
  // u8x8.clearDisplay();

 u8x8.setFont(u8x8_font_inr21_2x4_r);
 u8x8.setCursor(0,0);//这里设置显示的坐标
 u8x8.print(erweima1);//输出显示内容
 u8x8.print("+");//输出显示内容
 u8x8.print(erweima2);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,4);//这里设置显示的坐标
 u8x8.print("lx1:");//输出显示内容
 u8x8.print(lx_x);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,4);//这里设置显示的坐标
 u8x8.print("rd2:");//输出显示内容
 u8x8.print(ring_eyes_dog2);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,5);//这里设置显示的坐标
 u8x8.print("a:");//输出显示内容
 u8x8.print(Angle_Read());//输出显示内容Angle_origal
u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,5);//这里设置显示的坐标
 u8x8.print("o:");//输出显示内容
 u8x8.print(Angle_origal);//输出显示内容err_angle_dog Angle_orign


 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,6);//这里设置显示的坐标
 u8x8.print("x:");//输出显示内容
 u8x8.print(lidar_x_debug);//输出显示内容
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,6);//这里设置显示的坐标
 u8x8.print("y:");//输出显示内容
 u8x8.print(lidar_y_debug);//输出显示内容


 u8x8.refreshDisplay();

}
void SPI_pingmu_all2(){
  lidar_x_debug = Get_LidarDatafromIIC(0x10);
  lidar_y_debug = Get_LidarDatafromIIC(0x20);
  // u8x8.clearDisplay();

 u8x8.setFont(u8x8_font_inr21_2x4_r);
 u8x8.setCursor(0,0);//这里设置显示的坐标
 u8x8.print(erweima3);//输出显示内容
 u8x8.print("+");//输出显示内容
 u8x8.print(erweima4);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,4);//这里设置显示的坐标
 u8x8.print("lx1:");//输出显示内容
 u8x8.print(lx_x);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,4);//这里设置显示的坐标
 u8x8.print("rd2:");//输出显示内容
 u8x8.print(ring_eyes_dog2);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,5);//这里设置显示的坐标
 u8x8.print("a:");//输出显示内容
 u8x8.print(Angle_Read());//输出显示内容Angle_origal
u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,5);//这里设置显示的坐标
 u8x8.print("o:");//输出显示内容
 u8x8.print(Angle_origal);//输出显示内容err_angle_dog Angle_orign


 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,6);//这里设置显示的坐标
 u8x8.print("x:");//输出显示内容
 u8x8.print(lidar_x_debug);//输出显示内容
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,6);//这里设置显示的坐标
 u8x8.print("y:");//输出显示内容
 u8x8.print(lidar_y_debug);//输出显示内容


 u8x8.refreshDisplay();

}

void Angle_chushihua(int chushihuazhi){
  while(Angle_origal < chushihuazhi){
    Angle_init();
    if(Angle_origal > chushihuazhi){
      state++;
    }
  }
}

void Angle_chushihua2(int chushihuazhi){
  while(Angle_origal > chushihuazhi){
    Angle_init();
    if(Angle_origal < chushihuazhi){
      state++;
    }
  }
}


void SPI_pingmu_shuaxin(){
  lidar_x_debug = Get_LidarDatafromIIC(0x10);
  lidar_y_debug = Get_LidarDatafromIIC(0x20);
  // u8x8.clearDisplay();

 u8x8.setFont(u8x8_font_inr21_2x4_r);
 u8x8.setCursor(0,0);//这里设置显示的坐标
 u8x8.print(erweima1);//输出显示内容
 u8x8.print("+");//输出显示内容
 u8x8.print(erweima2);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,4);//这里设置显示的坐标
 u8x8.print("x:");//输出显示内容
 u8x8.print(lidar_x_debug);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,4);//这里设置显示的坐标
 u8x8.print("y:");//输出显示内容
 u8x8.print(lidar_y_debug);//输出显示内容

 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,5);//这里设置显示的坐标
 u8x8.print("a:");//输出显示内容
 u8x8.print(Angle_Read());//输出显示内容Angle_origal
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,5);//这里设置显示的坐标
 u8x8.print("o:");//输出显示内容
 u8x8.print(Angle_origal);//输出显示内容


 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(0,6);//这里设置显示的坐标
 u8x8.print("r1:");//输出显示内容
 u8x8.print(lx_x);//输出显示内容
 u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
 u8x8.setCursor(8,6);//这里设置显示的坐标
 u8x8.print("r2:");//输出显示内容
 u8x8.print(ring_x1);//输出显示内容

 u8x8.clearDisplay();
 u8x8.refreshDisplay();

}

void zhuanpan_wuliao_ri(int pwm1){
  setPWM(pwm1,-pwm1,-pwm1,pwm1);
  lidar_x_debug = Get_LidarDatafromIIC(0x10);
  if(lidar_x_debug < 35){
    stop();
    delay(1000);
    state++;
  }
}

int color_flag = 0;
void shibie_color(){
if(color_flag == 0){
  color_x = 0;
  color_5 = 0;
  ring_control();
  K210_fasong(5);
  delay(100);
  read_k210_color();
  color_5 = color_x;
  if(color_5 =! 0){
  color_flag = 1;
  K210_fasong(0);
  delay(100);
  while(softSerial.read() >= 0){}
  //shibie_color_left();
  delay(5000);
  }
}
if(color_flag == 1){
  color_x = 0;
  color_4 = 0;
  //shibie_color_left();
  K210_fasong(5);
  delay(100);
  read_k210_color();
  color_4 = color_x;
  if(color_4 =! 0){
  color_flag = 2;
  K210_fasong(0);
  delay(100);
  while(softSerial.read() >= 0){}
  //shibie_color_right();
  delay(5000);
  }
}
if(color_flag == 2){
  color_x = 0;
  color_6 = 0;
  //shibie_color_right();
  K210_fasong(5);
  read_k210_color();
  color_6 = color_x;
  if(color_6 =! 0){
  color_flag = 3;
  K210_fasong(0);
  delay(100);
  while(softSerial.read() >= 0){}
  }
}
if(color_flag == 3){
  color_x = 0;
  K210_fasong(0);
  delay(100);
  while(softSerial.read() >= 0){}
  state++;
}


}

int lujing1 = 15;
int lujing2 = 15;
int lujing3 = 80;
int lujing4 = 81;
int lujing5 = 63;
int lujing6 = 28;
int lujing7 = 90;
int lujing8 = 52;
int lujing9 = 100;
int lujing10 = 66;
int lujing11 = 107;
int lujing12 = 15;



void setup() {
  // put your setup code here, to run once:
  Wire.begin();  //开启iic
  Serial.begin(9600);  //单片机串口开启
  Serial1.begin(38400);  //步进电机总线波特率38400
  Serial2.begin(1000000);  //舵机波特率1000000
  //Serial3.begin(9600);  //陀螺仪波特率9600
  softSerial.begin(9600);
  sms_sts.pSerial = &Serial2;  //
  servo_init();  //舵机初始化位置 0 2048 2048 2048 2048
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.clearDisplay();
  WitInit(WIT_PROTOCOL_NORMAL, 0x50);
	WitSerialWriteRegister(SensorUartSend);
	WitRegisterCallBack(SensorDataUpdata);
  WitDelayMsRegister(Delayms);
  // display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //开启屏幕，设置地址为0x3C
  // display.clearDisplay();
  AutoScanSensor();
  Serial3.write(cmd_gyro,5);
  Angle_init();
  delay(4000);
  Angle_init();  //陀螺仪初始化
  Angle_init();
 
}


void loop() {

if(tiaoshi == 6){
 SPI_pingmu_all();
switch(state){
  case 0: move_le(20);break;
  case 1: read_motor4(20);break;
  case 2: K210_fasong(1);read_k210_erweima();erweima_control();move_f(59);break;
  case 3: read_motor4(59);break;
  case 4: K210_fasong(1);read_k210_erweima();erweima_judddd();break;

  case 5: servo_init();move_ri(10);break;
  case 6: read_motor4(10);break;
  case 7: move_f(70);break;
  case 8: read_motor4(70);break;
  case 9: color_control();zhuanpan_wuliao(5);break;
  case 10: color_control();stop();delay(500);move_f(color_pulse);break;
  case 11: read_motor4(color_pulse);break;
  case 12: control_y(color_l2,Angle_origal);judgment_move_y(color_l2,Angle_origal,1);break;
  case 13: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 14: stop();delay(10);K210_fasong(2);read_k210_color();color_jud();color_judgment();break;//read_k210_color();color_jud();color_judgment();

  case 15:servo_init();state++;break;
  case 16:move_le(5);break;
  case 17:read_motor4(5);break;
  case 18:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 19:move_f(43);break;
  case 20:read_motor4(43);break;
  case 21:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 22:move_le(80);break;
  case 23:read_motor4(80);break;
  case 24:ring_control();control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 25:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 26:move_le(15);break;
  case 27:read_motor4(15);break;
  case 28:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 29:fangzhi_luoji();zhuaqu_ring_luoji();state++;break;

  case 30:servo_init();move_le(74);break;
  case 31:read_motor4(74);break;
  case 32:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 33:move_b(68);break;
  case 34:read_motor4(68);break;
  case 35:ring_control();control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 36:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 37:move_le(15);break;
  case 38:read_motor4(15);break;
  case 39:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 40:fangzhi_luoji();servo_init();delay(1000);servo_init();Angle_chushihua(80);break;




  case 41:control_angle(Angle_origal-90);angle_judgment(Angle_origal-90);break;
  case 42:delay(1000);Angle_chushihua2(10);break;
  case 43:move_f(82);break;
  case 44:read_motor4(82);break;
  case 45:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 46:move_ri(92);break;
  case 47:read_motor4(92);break;
  case 48:ring_control();control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 49:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 50:servo_init();move_ri(80);break;
  case 51:read_motor4(80);break;
  case 52:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 53:move_b(26);break;
  case 54:read_motor4(26);break;
  case 55:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 56: color_control();zhuanpan_wuliao(-5);break;
  case 57: color_control();stop();delay(500);move_b(color_pulse2);break;
  case 58: read_motor4(color_pulse2);break;
  case 59: control_y(color_l3,Angle_origal);judgment_move_y(color_l3,Angle_origal,1);break;
  case 60:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 61:stop();delay(10);K210_fasong(2);read_k210_color();color_jud2();color_judgment2();break;//read_k210_color();color_jud();color_judgment();

  case 62:servo_init();move_le(5);break;
  case 63:read_motor4(5);break;
  case 64:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 65:move_f(43);break;
  case 66:read_motor4(43);break;
  case 67:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 68:move_le(80);break;
  case 69:read_motor4(80);break;
  case 70:ring_control();control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 71:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 72:move_le(15);break;
  case 73:read_motor4(15);break;
  case 74:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 75:fangzhi_luoji2();zhuaqu_ring_luoji2();state++;break;

  case 76:servo_init();move_le(74);break;
  case 77:read_motor4(74);break;
  case 78:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 79:move_b(83);break;
  case 80:read_motor4(83);break;
  case 81:ring_control();control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 82:stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 83:state++;break;
  case 84:state++;break;
  case 85:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 86:fangzhi_luoji3();servo_init();state++;break;

  case 87:servo_init();move_le(110);break;
  case 88:read_motor4(110);break;
  case 89:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 90:move_b(88);break;
  case 91:read_motor4(88);break;
  case 92:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 93:move_b(80);break;
  case 94:read_motor4(80);break;
  case 95:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 96:move_xy(10);break;
  case 97:read_motor4(10);break;

  default : stop();break;//
}
}


if(tiaoshi == 7){  // 颜色
 SPI_pingmu_shuaxin();
switch(state){
  case 0: move_le(20);break;
  case 1: read_motor4(20);break;
  case 2: K210_fasong(1);read_k210_erweima();erweima_control();move_f(59);break;
  case 3: read_motor4(59);break;
  case 4: K210_fasong(1);read_k210_erweima();erweima_judddd();break;

  case 5: servo_init();move_ri(10);break;
  case 6: read_motor4(10);break;
  case 7: move_f(70);break;
  case 8: read_motor4(70);break;
  case 9: color_control();zhuanpan_wuliao(5);break;
  case 10: color_control();stop();delay(500);move_f(color_pulse);break;
  case 11: read_motor4(color_pulse);break;
  case 12: control_y(color_l2,Angle_origal);judgment_move_y(color_l2,Angle_origal,1);break;
  case 13: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 14: stop();delay(10);K210_fasong(2);read_k210_color();break;//read_k210_color();color_jud();color_judgment();

  default : stop();break;//
}
}


if(tiaoshi == 8){ //跑图
 SPI_pingmu_shuaxin();
switch(state){
  case 0: move_le(20);break;
  case 1: read_motor4(20);break;
  case 2: K210_fasong(1);read_k210_erweima();erweima_control();move_f(59);break;
  case 3: read_motor4(59);break;
  case 4: K210_fasong(1);read_k210_erweima();erweima_judddd();break;

  case 5: servo_init();move_ri(10);break;
  case 6: read_motor4(10);break;
  case 7: move_f(70);break;
  case 8: read_motor4(70);break;
  case 9: color_control();zhuanpan_wuliao(5);break;
  case 10: color_control();stop();delay(500);move_f(color_pulse);break;
  case 11: read_motor4(color_pulse);break;
  case 12: control_y(color_l2,Angle_origal);judgment_move_y(color_l2,Angle_origal,1);break;
  case 13: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 14: state++;break;//read_k210_color();color_jud();color_judgment();

  case 15:servo_init();state++;break;
  case 16:move_le(5);break;
  case 17:read_motor4(5);break;
  case 18:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 19:move_f(43);break;
  case 20:read_motor4(43);break;
  case 21:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 22:move_le(80);break;
  case 23:read_motor4(80);break;
  case 24:ring_control();control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 25:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 26:move_le(15);break;
  case 27:read_motor4(15);break;
  case 28:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 29:state++;break;

  case 30:servo_init();move_le(74);break;
  case 31:read_motor4(74);break;
  case 32:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 33:move_b(68);break;
  case 34:read_motor4(68);break;
  case 35:ring_control();control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 36:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 37:move_le(15);break;
  case 38:read_motor4(15);break;
  case 39:control_angle(Angle_origal+90.5);angle_judgment(Angle_origal+90.5);break;
  case 40:delay(1000);Angle_chushihua(80);servo_init();break;




  case 41:control_angle(Angle_origal-90.5);angle_judgment(Angle_origal-90.5);break;
  case 42:delay(1000);Angle_chushihua2(10);break;
  case 43:move_f(82);break;
  case 44:read_motor4(82);break;
  case 45:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 46:move_ri(92);break;
  case 47:read_motor4(92);break;
  case 48:ring_control();control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 49:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 50:servo_init();move_ri(80);break;
  case 51:read_motor4(80);break;
  case 52:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 53:move_b(26);break;
  case 54:read_motor4(26);break;
  case 55:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 56: color_control();zhuanpan_wuliao(-5);break;
  case 57: color_control();stop();delay(500);move_b(color_pulse2);break;
  case 58: read_motor4(color_pulse2);break;
  case 59: control_y(color_l3,Angle_origal);judgment_move_y(color_l3,Angle_origal,1);break;
  case 60: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 61: state++;break;//read_k210_color();color_jud();color_judgment();

  case 62:servo_init();move_le(5);break;
  case 63:read_motor4(5);break;
  case 64:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 65:move_f(43);break;
  case 66:read_motor4(43);break;
  case 67:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 68:move_le(80);break;
  case 69:read_motor4(80);break;
  case 70:ring_control();control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 71:stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 72:move_le(15);break;
  case 73:read_motor4(15);break;
  case 74:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 75:state++;break;

  case 76:servo_init();move_le(74);break;
  case 77:read_motor4(74);break;
  case 78:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 79:move_b(83);break;
  case 80:read_motor4(83);break;
  case 81:ring_control();control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 82:stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 83:state++;break;
  case 84:state++;break;
  case 85:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 86:servo_init();state++;break;

  case 87:servo_init();move_le(108);break;
  case 88:read_motor4(108);break;
  case 89:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 90:move_b(88);break;
  case 91:read_motor4(88);break;
  case 92:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 93:move_b(80);break;
  case 94:read_motor4(80);break;
  case 95:control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 96:move_xy(10);break;
  case 97:read_motor4(10);break;

  default : stop();break;//
}
}


if(tiaoshi == 1){
  color_control();
  K210_fasong(2);
  SPI_pingmu_shuaxin();
}
if(tiaoshi == 2){
  erweima_control();
  K210_fasong(1);
  read_k210_erweima();
  SPI_pingmu_shuaxin();
}
if(tiaoshi == 3){
  ring_control();
  K210_fasong(4);
  SPI_pingmu_shuaxin();
}

if(tiaoshi == 0){
//雷达程序
SPI_pingmu_all();
switch(state){
  case 0:  move_le(15);break;
  case 1:  read_motor4(15);break;
//扫描二维码并显示步骤
  case 2 : control(erweima_location[0],erweima_location[1],Angle_origal);erweima_control();
           K210_fasong(1);read_k210_erweima();judgment_move_erweima(erweima_location[0],erweima_location[1],Angle_origal,3);break;  //words_display(); 移动至二维码位置并扫码显示

//识别颜色抓取步骤
  case 3 : move_with_jud(color_location[0],color_location[1],Angle_origal,1);color_control();break;                                      //移动至抓取物料位置，机械臂摆动到识别颜色姿态
  case 4 : stop();delay(10);K210_fasong(2);read_k210_color();color_jud();color_judgment();break;//开始识别物料颜色并抓取

//移动至粗加工区步骤
  case 5 : servo_init();move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal,3);break;                          //移动到物料区于粗加工区的拐角过渡位置
  case 6 : ring_control();move_with_jud(chu_ring_location[0],chu_ring_location[1],Angle_origal,1);break;         //移动至粗加工区，机械臂摆出识别圆环姿态

//粗加工区过程
  case 7 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;    //开始识别圆环并调整车身姿态
  case 8:  move_le(15);break;
  case 9:  read_motor4(15);break;
  case 10:  control_angle(Angle_origal+1);angle_judgment(Angle_origal+1);break;
  case 11: K210_fasong(0);fangzhi_luoji();state = state + 1;break;                                           //从车上拿下物料，放置粗加工区
  case 12 : zhuaqu_ring_luoji();state = state + 1;servo_init();break;                                                      //将物料抓起放回车上

//移动至精加工区
  case 13 : servo_init();move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal+1,3);break;                   //移动至粗加工区与精加工区的拐角过渡位置
  case 14 : move_with_jud(jing_zheng[0],jing_zheng[1],Angle_origal+1,3);break;                  //移动至精加工区
  case 15 : control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;       //车身逆时针旋转90度    车身旋转时角度不够准确 考虑启动线段识别重置陀螺仪方法
  case 16 : move_with_jud(jing_ring_location_first[0],jing_ring_location_first[1],Angle_origal+91,1);ring_control();break;  //因车身旋转，重新定位精加工区，并机械臂摆出识别圆环姿态

//将物料放于精加工区
  case 17 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;       //识别圆环调整车身姿态
  case 18:  move_le(15);break;
  case 19:  read_motor4(15);break;
  case 20:  control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 21 : K210_fasong(0);fangzhi_luoji();state = state + 1;servo_init();break;                                              //将物料从车身上放置精加工区

//离开精加工区 准备抓取第二套物料
  case 22 : servo_init();state++;break;         //移动出精加工区外围move_with_jud(jing_ring_location_second[0],jing_ring_location_second[1],Angle_origal+91,1);
  case 23 : control_angle(Angle_origal+1.2);angle_judgment(Angle_origal+1.2);break;              //顺旋转车身姿态到0度
  case 24 : move_with_jud(jing_zheng2[0],jing_zheng2[1],Angle_origal+1.2,3);break;                      //重新定位自身位置（可省略？）
  case 25 : move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal+1.2,3);break;                       //移动至粗加工区与精加工区的拐角过渡位置
  case 26 : move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal+1.2,3);break;                        //移动到物料区于粗加工区的拐角过渡位置
                 

  //抓取第二套物料区域，更改函数有：color_jud2();识别颜色判断 更改为第二套物料 erweima2
  //                             fangzhi_luoji2();放置物料的判断逻辑
  //                             zhuaqu_ring_luoji2();抓取物料的判断逻辑
  //                             case 29和11 的不同之处，车身位置不同，因为有台阶存在，考虑更改case11 使用码垛方法 摄像头识别程序改为识别绿色调整车身姿态


  case 27 : move_with_jud(color_location[0]+1,color_location[1],Angle_origal+1.2,1);color_control();break;   //移动至抓取物料位置，机械臂摆动到识别颜色姿态
  case 28 : stop();delay(10);K210_fasong(2);read_k210_color();color_jud2();color_judgment2();break;     //开始识别物料颜色并抓取

  case 29 : servo_init();move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal+1.2,3);break;                        //移动到物料区于粗加工区的拐角过渡位置         
  case 30 : ring_control();move_with_jud(chu_ring_location[0],chu_ring_location[1],Angle_origal+1.2,3);break;        //移动至粗加工区，机械臂摆出识别圆环姿态

  case 31 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;     //开始识别圆环并调整车身姿态
  case 32:  move_le(15);break;
  case 33:  read_motor4(15);break;
  case 34:  control_angle(Angle_origal+1.2);angle_judgment(Angle_origal+1.2);break;
  case 35 : K210_fasong(0);fangzhi_luoji2();state = state + 1;break;                                           //从车上拿下物料，放置粗加工区
  case 36 : zhuaqu_ring_luoji2();state = state + 1;servo_init();break;                                                      //将物料抓起放回车上

  case 37 : servo_init();move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal+1.2,3);break;                                 //移动至粗加工区与精加工区的拐角过渡位置
  case 38 : move_with_jud(jing_zheng2[0],jing_zheng2[1],Angle_origal+1.2,3);break;                                //移动至精加工区
  case 39 : control_angle(Angle_origal+92);angle_judgment(Angle_origal+92);ring_control();break;                      //车身逆时针旋转90度    车身旋转时角度不够准确 考虑启动线段识别重置陀螺仪方法
  case 40 : state++;ring_control();break;                 //因车身旋转，重新定位精加工区，并机械臂摆出识别圆环姿态move_with_jud(jing_ring_location_second[0],jing_ring_location_second[1],Angle_origal+91,1);

  case 41 : ring_control();stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;    //识别圆环调整车身姿态
  case 42 : K210_fasong(0);fangzhi_luoji3();state = state + 1;servo_init();break;                                          //将物料从车身上放置精加工区

  case 43 : servo_init();control_angle(Angle_origal+1.3);angle_judgment(Angle_origal+1.3);break;         //返回程序需要更改，需要在这里旋转车身再返回
  case 44 : state++;break;//move_with_jud(107,179,Angle_origal,3);
  case 45 : move_with_jud(back9[0],back9[1],Angle_origal+1.3,3);break;//
  case 46 : move_with_jud(back10[0],back10[1],Angle_origal+1.3,3);break;//
  case 47 : move_with_jud(back11[0],back11[1],Angle_origal+1.3,3);break;//

  default : stop();break;//
}
}

if(tiaoshi == 4){
//雷达程序
SPI_pingmu_all();
switch(state){
  case 0:  move_le(15);break;
  case 1:  read_motor4(15);break;
//扫描二维码并显示步骤
  case 2 : control(erweima_location[0],erweima_location[1],Angle_origal);erweima_control();
           K210_fasong(1);read_k210_erweima();judgment_move_erweima(erweima_location[0],erweima_location[1],Angle_origal,3);break;  //words_display(); 移动至二维码位置并扫码显示

//识别颜色抓取步骤
  case 3 : move_with_jud(color_location[0],color_location[1],Angle_origal,1);color_control();break;                                      //移动至抓取物料位置，机械臂摆动到识别颜色姿态
  case 4 : stop();delay(10);state++;break;//开始识别物料颜色并抓取

//移动至粗加工区步骤
  case 5 : servo_init();move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal,3);break;                          //移动到物料区于粗加工区的拐角过渡位置
  case 6 : ring_control();move_with_jud(chu_ring_location[0],chu_ring_location[1],Angle_origal,1);break;         //移动至粗加工区，机械臂摆出识别圆环姿态

//粗加工区过程
  case 7 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;    //开始识别圆环并调整车身姿态
  case 8:  move_le(15);break;
  case 9:  read_motor4(15);break;
  case 10:  control_angle(Angle_origal+1);angle_judgment(Angle_origal+1);break;
  case 11: K210_fasong(0);state = state + 1;break;                                           //从车上拿下物料，放置粗加工区
  case 12 : state = state + 1;servo_init();break;                                                      //将物料抓起放回车上

//移动至精加工区
  case 13 : servo_init();move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal+1,3);break;                   //移动至粗加工区与精加工区的拐角过渡位置
  case 14 : move_with_jud(jing_zheng[0],jing_zheng[1],Angle_origal+1,3);break;                  //移动至精加工区
  case 15 : control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;       //车身逆时针旋转90度    车身旋转时角度不够准确 考虑启动线段识别重置陀螺仪方法
  case 16 : move_with_jud(jing_ring_location_first[0],jing_ring_location_first[1],Angle_origal+91,1);ring_control();break;  //因车身旋转，重新定位精加工区，并机械臂摆出识别圆环姿态

//将物料放于精加工区
  case 17 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;       //识别圆环调整车身姿态
  case 18:  move_le(15);break;
  case 19:  read_motor4(15);break;
  case 20:  control_angle(Angle_origal+91);angle_judgment(Angle_origal+91);break;
  case 21 : K210_fasong(0);state = state + 1;servo_init();break;                                              //将物料从车身上放置精加工区

//离开精加工区 准备抓取第二套物料
  case 22 : servo_init();state++;break;         //移动出精加工区外围move_with_jud(jing_ring_location_second[0],jing_ring_location_second[1],Angle_origal+91,1);
  case 23 : control_angle(Angle_origal+1.2);angle_judgment(Angle_origal+1.2);break;              //顺旋转车身姿态到0度
  case 24 : move_with_jud(jing_zheng2[0],jing_zheng2[1],Angle_origal+1.2,3);break;                      //重新定位自身位置（可省略？）
  case 25 : move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal+1.2,3);break;                       //移动至粗加工区与精加工区的拐角过渡位置
  case 26 : move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal+1.2,3);break;                        //移动到物料区于粗加工区的拐角过渡位置
                 

  //抓取第二套物料区域，更改函数有：color_jud2();识别颜色判断 更改为第二套物料 erweima2
  //                             fangzhi_luoji2();放置物料的判断逻辑
  //                             zhuaqu_ring_luoji2();抓取物料的判断逻辑
  //                             case 29和11 的不同之处，车身位置不同，因为有台阶存在，考虑更改case11 使用码垛方法 摄像头识别程序改为识别绿色调整车身姿态


  case 27 : move_with_jud(color_location[0]+1,color_location[1],Angle_origal+1.2,1);color_control();break;   //移动至抓取物料位置，机械臂摆动到识别颜色姿态
  case 28 : stop();delay(10);state++;break;     //开始识别物料颜色并抓取

  case 29 : servo_init();move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal+1.2,3);break;                        //移动到物料区于粗加工区的拐角过渡位置         
  case 30 : ring_control();move_with_jud(chu_ring_location[0],chu_ring_location[1],Angle_origal+1.2,3);break;        //移动至粗加工区，机械臂摆出识别圆环姿态

  case 31 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;     //开始识别圆环并调整车身姿态
  case 32:  move_le(15);break;
  case 33:  read_motor4(15);break;
  case 34:  control_angle(Angle_origal+1.2);angle_judgment(Angle_origal+1.2);break;
  case 35 : K210_fasong(0);state = state + 1;break;                                           //从车上拿下物料，放置粗加工区
  case 36 : state = state + 1;servo_init();break;                                                      //将物料抓起放回车上

  case 37 : servo_init();move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal+1.2,3);break;                                 //移动至粗加工区与精加工区的拐角过渡位置
  case 38 : move_with_jud(jing_zheng2[0],jing_zheng2[1],Angle_origal+1.2,3);break;                                //移动至精加工区
  case 39 : control_angle(Angle_origal+92);angle_judgment(Angle_origal+92);ring_control();break;                      //车身逆时针旋转90度    车身旋转时角度不够准确 考虑启动线段识别重置陀螺仪方法
  case 40 : state++;ring_control();break;                 //因车身旋转，重新定位精加工区，并机械臂摆出识别圆环姿态move_with_jud(jing_ring_location_second[0],jing_ring_location_second[1],Angle_origal+91,1);

  case 41 : ring_control();stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;    //识别圆环调整车身姿态
  case 42 : K210_fasong(0);state = state + 1;servo_init();break;                                          //将物料从车身上放置精加工区

  case 43 : servo_init();control_angle(Angle_origal+1.3);angle_judgment(Angle_origal+1.3);break;         //返回程序需要更改，需要在这里旋转车身再返回
  case 44 : state++;break;//move_with_jud(107,179,Angle_origal,3);
  case 45 : move_with_jud(back9[0],back9[1],Angle_origal+1.3,3);break;//
  case 46 : move_with_jud(back10[0],back10[1],Angle_origal+1.3,3);break;//
  case 47 : move_with_jud(back11[0],back11[1],Angle_origal+1.3,3);break;//

  default : stop();break;//
}
}








if(tiaoshi == 11){
  SPI_pingmu_all();
  erweima1 = 123;
  erweima2 = 321;
  switch(state){
  case 0: ring_control();stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 1:shibie_color();break;//state++;stop();delay(10);K210_fasong(6); read_k210_erweima2();erweima_juddd2();
  case 2:zhuaqu_luoji_guosai();state++;break;//zhuaqu_luoji_guosai();
  }
}

if(tiaoshi == 12){
  SPI_pingmu_all();
  erweima1 = 123;
  erweima2 = 321;
  switch(state){
  case 0: ring_control();stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 1:shibie_color();break;//state++;stop();delay(10);K210_fasong(6); read_k210_erweima2();erweima_juddd2();
  case 2:zhuaqu_ring_luoji();state++;break;//zhuaqu_luoji_guosai();
  }
}


if(tiaoshi == 10){
  SPI_pingmu_all();

  switch(state){
  case 0: move_f(lujing1);break;
  case 1: read_motor4(lujing1);break;
  case 2: ring_control();move_ri(lujing2);break;
  case 3: read_motor4(lujing2);break;
  case 4: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 5: move_f(lujing3);break;
  case 6: K210_fasong(1);erweima_control();read_motor4(lujing3);break;
  case 7: K210_fasong(1);read_k210_erweima();erweima_judddd();break;
  case 8: servo_init();delay(2000);move_le(10);break;
  case 9: read_motor4(10);break;
  case 10: control_angle(Angle_origal);angle_judgment(Angle_origal);break;

  case 11: servo_init();move_f(lujing4);break;
  case 12: read_motor4(lujing4);break;
  case 13: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 14: move_ri(lujing5);break;
  case 15: read_motor4(lujing5);break;
  case 16: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 17: ring_control();zhuanpan_wuliao_ri(5);break;
  case 18: ring_control();stop();delay(500);move_ri(lujing6);break;
  case 19: read_motor4(lujing6);break;
  case 20: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 21: stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;

  case 22:shibie_color();break;//state++;stop();delay(10);K210_fasong(6); read_k210_erweima2();erweima_juddd2();
  case 23:zhuaqu_luoji_guosai();state++;break;//zhuaqu_luoji_guosai();

  case 24:servo_init();move_ri(lujing7);break;
  case 25:read_motor4(lujing7);break;
  case 26:control_angle(Angle_origal-90.5);angle_judgment(Angle_origal-90.5);break;
  case 27:delay(1000);Angle_chushihua2(-80);break;
  case 28:move_ri(lujing8);break;
  case 29:read_motor4(lujing8);break;
  case 30:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 31:ring_control();stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 32:move_ri(15);break;
  case 33:read_motor4(15);break;
  case 34:fangzhi_luoji();zhuaqu_ring_luoji();state++;break;

  case 35:servo_init();move_ri(lujing9);break;
  case 36:read_motor4(lujing9);break;
  case 37:control_angle(Angle_origal);angle_judgment(Angle_origal);break;

  case 38:move_b(lujing10);break;
  case 39:read_motor4(lujing10);break;
  case 40:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 41:color_control();zhuanpan_wuliao(-5);break;
  case 42: color_control();stop();delay(500);move_b(color_pulse);break;
  case 43: read_motor4(color_pulse);break;
  case 44: control_y(color_l2,Angle_origal);judgment_move_y(color_l2,Angle_origal,1);break;
  case 45: stop();delay(10);K210_fasong(2);read_k210_color();color_jud();color_judgment();break;

  case 46:servo_init();state++;break;
  case 47:move_le(5);break;
  case 48:read_motor4(5);break;
  case 49:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 50:move_b(lujing11);break;
  case 51:read_motor4(lujing11);break;
  case 52:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 53:move_ri(lujing12);break;
  case 54:read_motor4(lujing12);break;
  

default : stop();break;//
 }

 }

 if(tiaoshi == 13){
  SPI_pingmu_all();

  switch(state){
  case 0: move_f(lujing1);break;
  case 1: read_motor4(lujing1);break;
  case 2: ring_control();move_ri(lujing2);break;
  case 3: read_motor4(lujing2);break;
  case 4: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 5: move_f(lujing3);break;
  case 6: K210_fasong(1);erweima_control();read_motor4(lujing3);break;
  case 7: K210_fasong(1);read_k210_erweima();erweima_judddd();break;
  case 8: servo_init();delay(2000);move_le(10);break;
  case 9: read_motor4(10);break;
  case 10: control_angle(Angle_origal);angle_judgment(Angle_origal);break;

  case 11: servo_init();move_f(lujing4);break;
  case 12: read_motor4(lujing4);break;
  case 13: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 14: move_ri(lujing5);break;
  case 15: read_motor4(lujing5);break;
  case 16: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 17: ring_control();zhuanpan_wuliao_ri(5);break;
  case 18: ring_control();stop();delay(500);move_ri(lujing6);break;
  case 19: read_motor4(lujing6);break;
  case 20: control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 21: stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;

  case 22:state++;break;//state++;stop();delay(10);K210_fasong(6); read_k210_erweima2();erweima_juddd2();
  case 23:zhuaqu_ring_luoji();state++;break;//zhuaqu_luoji_guosai();

  case 24:servo_init();move_ri(lujing7);break;
  case 25:read_motor4(lujing7);break;
  case 26:control_angle(Angle_origal-89);angle_judgment(Angle_origal-89);break;
  case 27:delay(1000);Angle_chushihua2(-80);break;
  case 28:move_ri(lujing8);break;
  case 29:read_motor4(lujing8);break;
  case 30:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 31:ring_control();stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,120);ring_judgment();break;
  case 32:move_ri(15);break;
  case 33:read_motor4(15);break;
  case 34:fangzhi_luoji();zhuaqu_ring_luoji();state++;break;

  case 35:servo_init();move_ri(lujing9);break;
  case 36:read_motor4(lujing9);break;
  case 37:control_angle(Angle_origal);angle_judgment(Angle_origal);break;

  case 38:move_b(lujing10);break;
  case 39:read_motor4(lujing10);break;
  case 40:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 41:color_control();zhuanpan_wuliao(-5);break;
  case 42: color_control();stop();delay(500);move_b(color_pulse);break;
  case 43: read_motor4(color_pulse);break;
  case 44: control_y(color_l2,Angle_origal);judgment_move_y(color_l2,Angle_origal,1);break;
  case 45: stop();delay(10);K210_fasong(2);read_k210_color();color_jud();color_judgment();break;

  case 46:servo_init();state++;break;
  case 47:move_le(5);break;
  case 48:read_motor4(5);break;
  case 49:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 50:move_b(lujing11);break;
  case 51:read_motor4(lujing11);break;
  case 52:control_angle(Angle_origal);angle_judgment(Angle_origal);break;
  case 53:move_ri(lujing12);break;
  case 54:read_motor4(lujing12);break;
  

default : stop();break;//
 }

 }

//雷达测试
// SPI_pingmu_all();
// switch(state){
// //扫描二维码并显示步骤
//   case 0 : control(erweima_location[0],erweima_location[1],Angle_origal);erweima_control();
//            K210_fasong(1);read_k210_erweima();judgment_move_erweima(erweima_location[0],erweima_location[1],Angle_origal,3);break;  //words_display(); 移动至二维码位置并扫码显示

// //识别颜色抓取步骤
//   case 1 : move_with_jud(color_location[0],color_location[1],Angle_origal,1);color_control();break;                                      //移动至抓取物料位置，机械臂摆动到识别颜色姿态
//   case 2 : stop();delay(10);K210_fasong(2);read_k210_color();color_jud();color_judgment();break;//开始识别物料颜色并抓取

// //移动至粗加工区步骤
//   case 3 : move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal,3);break;                          //移动到物料区于粗加工区的拐角过渡位置
//   case 4 : ring_control();move_with_jud(chu_ring_location[0],chu_ring_location[1],Angle_origal,1);break;         //移动至粗加工区，机械臂摆出识别圆环姿态

// //粗加工区过程
//   case 5 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,125);ring_judgment();break;    //开始识别圆环并调整车身姿态
//   case 6 : K210_fasong(0);state = state + 1;break;                                           //从车上拿下物料，放置粗加工区
//   case 7 : state = state + 1;break;                                                      //将物料抓起放回车上

// //移动至精加工区
//   case 8 : move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal,3);break;                   //移动至粗加工区与精加工区的拐角过渡位置
//   case 9 : move_with_jud(107,179,Angle_origal,3);break;                  //移动至精加工区
//   case 10 : control_angle(Angle_origal+89);angle_judgment(Angle_origal+89);break;       //车身逆时针旋转90度    车身旋转时角度不够准确 考虑启动线段识别重置陀螺仪方法
//   case 11 : move_with_jud(jing_ring_location_first[0],jing_ring_location_first[1],Angle_origal+89,1);ring_control();break;  //因车身旋转，重新定位精加工区，并机械臂摆出识别圆环姿态

// //将物料放于精加工区
//   case 12 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,125);ring_judgment();break;       //识别圆环调整车身姿态
//   case 13 : K210_fasong(0);state = state + 1;break;                                              //将物料从车身上放置精加工区

// //离开精加工区 准备抓取第二套物料
//   case 14 : servo_init();move_with_jud(26,109,Angle_origal+89,1);break;         //移动出精加工区外围
//   case 15 : control_angle(Angle_origal);angle_judgment(Angle_origal);break;              //顺旋转车身姿态到0度
//   case 16 : move_with_jud(107,179,Angle_origal,3);break;                      //重新定位自身位置（可省略？）
//   case 17 : move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal,3);break;                       //移动至粗加工区与精加工区的拐角过渡位置
//   case 18 : move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal,3);break;                        //移动到物料区于粗加工区的拐角过渡位置
                  

//   //抓取第二套物料区域，更改函数有：color_jud2();识别颜色判断 更改为第二套物料 erweima2
//   //                             fangzhi_luoji2();放置物料的判断逻辑
//   //                             zhuaqu_ring_luoji2();抓取物料的判断逻辑
//   //                             case 29和11 的不同之处，车身位置不同，因为有台阶存在，考虑更改case11 使用码垛方法 摄像头识别程序改为识别绿色调整车身姿态


//   case 19 : move_with_jud(color_location[0]+1,color_location[1],Angle_origal,1);color_control();break;                                            //移动至抓取物料位置，机械臂摆动到识别颜色姿态
//   case 20 : stop();delay(10);K210_fasong(2);read_k210_color();color_jud2();color_judgment2();break;     //开始识别物料颜色并抓取

//   case 21 : move_with_jud(color_chu_location[0],color_chu_location[1],Angle_origal,3);break;                        //移动到物料区于粗加工区的拐角过渡位置         
//   case 22 : ring_control();move_with_jud(chu_ring_location[0],chu_ring_location[1],Angle_origal,3);break;        //移动至粗加工区，机械臂摆出识别圆环姿态

//   case 23 : stop();delay(10);K210_fasong(4); read_k210_ring();control_ring(160,125);ring_judgment();break;     //开始识别圆环并调整车身姿态
//   case 24 : K210_fasong(0);state = state + 1;break;                                           //从车上拿下物料，放置粗加工区
//   case 25 : state = state + 1;break;                                                      //将物料抓起放回车上

//   case 26 : move_with_jud(chu_jing_location[0],chu_jing_location[1],Angle_origal,3);break;                                 //移动至粗加工区与精加工区的拐角过渡位置
//   case 27 : move_with_jud(107,179,Angle_origal,3);break;                                //移动至精加工区
//   case 28 : control_angle(Angle_origal+89);angle_judgment(Angle_origal+89);break;                      //车身逆时针旋转90度    车身旋转时角度不够准确 考虑启动线段识别重置陀螺仪方法
//   case 29 : move_with_jud(jing_ring_location_second[0],jing_ring_location_second[1],Angle_origal+89,1);ring_control();break;                 //因车身旋转，重新定位精加工区，并机械臂摆出识别圆环姿态

//   case 30 : stop();delay(10);K210_fasong(5); read_k210_ring();control_ring(160,120);ring_judgment();break;    //识别圆环调整车身姿态
//   case 31 : K210_fasong(0);state = state + 1;break;                                          //将物料从车身上放置精加工区

//   case 32 : servo_init();control_angle(Angle_origal);angle_judgment(Angle_origal);break;         //返回程序需要更改，需要在这里旋转车身再返回
//   case 33 : move_with_jud(107,179,Angle_origal,3);break;//
//   case 34 : move_with_jud(202,179,Angle_origal,3);break;//
//   case 35 : move_with_jud(207,14,Angle_origal,3);break;//
//   case 36 : move_with_jud(215,2,Angle_origal,3);break;//

//   default : stop();break;//
// }

}




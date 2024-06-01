#include <REG.h>
#include <wit_c_sdk.h>
#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80
static volatile char s_cDataUpdate = 0, s_cCmd = 0xff; 

static void CmdProcess(void);
static void AutoScanSensor(void);
static void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum);
static void Delayms(uint16_t ucMs);
const uint32_t c_uiBaud[8] = {0,4800, 9600, 19200, 38400, 57600, 115200, 230400};
unsigned char cmd_gyro[5] = {255, 170, 118, 0, 0};
int i2;
float fAcc[3], fGyro[3], fAngle[3];

double ABS = 1/7.5;
double wheelspeed[4] = {};
float chassis_xSpeed = 0;
float chassis_ySpeed = 0;
float chassis_wSpeed = 0;
float chassis_world_xspeed = 0;
float chassis_world_yspeed = 0;

float Angle_normal;
float Angle_origal;
  float angle_world = 0;

void Angle_init(){

      while (Serial3.available())
    {
      WitSerialDataIn(Serial3.read());
    }
		if(s_cDataUpdate)
		{
			for(i2 = 0; i2 < 3; i2++)
			{
				fAngle[i2] = sReg[Roll+i2] / 32768.0f * 180.0f;
			}
			if(s_cDataUpdate & ANGLE_UPDATE )
			{
        Angle_origal = fAngle[2];
      
				s_cDataUpdate &= ~ANGLE_UPDATE;
			}

      s_cDataUpdate = 0;
		}
  
}

float Angle_Read(){
      while (Serial3.available())
    {
      WitSerialDataIn(Serial3.read());
    }
		if(s_cDataUpdate)
		{
			for(i2 = 0; i2 < 3; i2++)
			{
				fAngle[i2] = sReg[Roll+i2] / 32768.0f * 180.0f;
			}
			if(s_cDataUpdate & ANGLE_UPDATE)
			{
        Angle_normal = fAngle[2];

				s_cDataUpdate &= ~ANGLE_UPDATE;
			}
      s_cDataUpdate = 0;
		}

  return Angle_normal;
}

void move(int motor, int dir, int speed1){
  unsigned char a[236]= {0,246,0,0,0,107};
  if(dir == 0){
    dir = 0;
  }
  if(dir == 1){
    dir = 16;
  }
  
  a[0] = motor ;
  a[2] = dir;
  a[3] = speed1;
  
  Serial1.write(a,6);
  delay(6);

}

void move_acceleration0(int motor, int dir, int speed1){
  unsigned char a[236]= {0,246,0,0,255,107};
  if(dir == 0){
    dir = 0;
  }
  if(dir == 1){
    dir = 16;
  }
  
  a[0] = motor ;
  a[2] = dir;
  a[3] = speed1;
  
  Serial1.write(a,6);
  delay(6);

}

void moveto_pulse(int motor, int dir, int speed1, long distance){ //motor（1-4） dir（1-2） speed（0-255） distance（0-16777215） 根据计算 forward:1/0.00048438cm      left:1/0.00046484cm
  unsigned char a[236]= {0,253,0,0,80,0,0,0,107};
  if(dir == 0){
    dir = 0;
  }
  if(dir == 1){
    dir = 16;
  }
  
  a[0] = motor;
  a[2] = dir;
  a[3] = speed1;

  int distance1 = 0;
  int distance2 = 0;
  int distance3 = 0;
//   distance = mil * 160;
  distance1 = distance / 65536;
  distance2 = (distance % 65536) / 256;
  distance3 = (distance % 65536) % 256;
  a[5] = distance1;
  a[6] = distance2;
  a[7] = distance3;
  
  Serial1.write(a,9);
  delay(7);

}




void stop(){
    move(1,0,0);
    move(2,0,0);
    move(3,1,0);
    move(4,1,0);
}


void setPWM(int sudu1, int sudu2, int sudu3, int sudu4){
   if (sudu1 > 0 || sudu1 == 0){
    move_acceleration0(1,1,sudu1);
   }
   if (sudu1 < 0){
    sudu1 = -sudu1;
    move_acceleration0(1,0,sudu1);
   }
   if (sudu2 > 0 || sudu2 == 0){
    move_acceleration0(2,0,sudu2);
   }
   if (sudu2 < 0){
    sudu2 = -sudu2;
    move_acceleration0(2,1,sudu2);
   }
   if (sudu3 > 0 || sudu3 == 0){
    move_acceleration0(3,1,sudu3);
   }
   if (sudu3 < 0){
    sudu3 = -sudu3;
    move_acceleration0(3,0,sudu3);
   }
   if (sudu4 > 0 || sudu4 == 0){
    move_acceleration0(4,0,sudu4);
   }
   if (sudu4 < 0){
    sudu4 = -sudu4;
    move_acceleration0(4,1,sudu4);
   }
}

void world_xyz(){
  angle_world = angle_world + 20;
  if(angle_world == 360){
    angle_world = 0;
  }
  float chassis_xSpeed_1 = 0;
  float chassis_xSpeed_2 = 0;
  float chassis_ySpeed_1 = 0;
  float chassis_ySpeed_2 = 0;

  chassis_xSpeed_1 = cos(angle_world)*chassis_world_xspeed;
  chassis_ySpeed_1 = sin(angle_world)*chassis_world_xspeed;
  // chassis_xSpeed_2 = cos(angle_world)*chassis_world_yspeed;
  // chassis_ySpeed_2 = sin(angle_world)*chassis_world_yspeed;

  chassis_xSpeed = chassis_xSpeed_1;
  chassis_ySpeed = -chassis_ySpeed_1;
}


void mecanum_calcSpeed(){
  wheelspeed[0] = ABS*(chassis_xSpeed - chassis_ySpeed - chassis_wSpeed);
  wheelspeed[1] = ABS*(chassis_xSpeed + chassis_ySpeed + chassis_wSpeed);
  wheelspeed[2] = ABS*(chassis_xSpeed + chassis_ySpeed - chassis_wSpeed);
  wheelspeed[3] = ABS*(chassis_xSpeed - chassis_ySpeed + chassis_wSpeed);
  setPWM(wheelspeed[0],wheelspeed[1],wheelspeed[2],wheelspeed[3]);
}


// 速度档位0xE8，加速度0，脉冲数0x0C80 = 3200个脉冲
//unsigned char a[236]={
//  0X02,0XFD,0X00,0X32,0X00,0X00,0X32,0X00,0X6B
//};
//unsigned char b[236]={
//  0X04,0XFD,0X00,0X32,0X00,0X00,0X32,0X00,0X6B
//};
//unsigned char c[236]={
//  0X01,0XFD,0X10,0X32,0X00,0X00,0X32,0X00,0X6B
//};
//unsigned char d[236]={
//  0X03,0XFD,0X10,0X32,0X00,0X00,0X32,0X00,0X6B
//};

void CopeCmdData(unsigned char ucData)
{
	static unsigned char s_ucData[50], s_ucRxCnt = 0;
	
	s_ucData[s_ucRxCnt++] = ucData;
	if(s_ucRxCnt<3)return;										//Less than three data returned
	if(s_ucRxCnt >= 50) s_ucRxCnt = 0;
	if(s_ucRxCnt >= 3)
	{
		if((s_ucData[1] == '\r') && (s_ucData[2] == '\n'))
		{
			s_cCmd = s_ucData[0];
			memset(s_ucData,0,50);
			s_ucRxCnt = 0;
		}
		else 
		{
			s_ucData[0] = s_ucData[1];
			s_ucData[1] = s_ucData[2];
			s_ucRxCnt = 2;
			
		}
	}
}
static void ShowHelp(void)
{
	Serial.print("\r\n************************	 WIT_SDK_DEMO	************************");
	Serial.print("\r\n************************          HELP           ************************\r\n");
	Serial.print("UART SEND:a\\r\\n   Acceleration calibration.\r\n");
	Serial.print("UART SEND:m\\r\\n   Magnetic field calibration,After calibration send:   e\\r\\n   to indicate the end\r\n");
	Serial.print("UART SEND:U\\r\\n   Bandwidth increase.\r\n");
	Serial.print("UART SEND:u\\r\\n   Bandwidth reduction.\r\n");
	Serial.print("UART SEND:B\\r\\n   Baud rate increased to 115200.\r\n");
	Serial.print("UART SEND:b\\r\\n   Baud rate reduction to 9600.\r\n");
	Serial.print("UART SEND:R\\r\\n   The return rate increases to 10Hz.\r\n");
  Serial.print("UART SEND:r\\r\\n   The return rate reduction to 1Hz.\r\n");
  Serial.print("UART SEND:C\\r\\n   Basic return content: acceleration, angular velocity, angle, magnetic field.\r\n");
  Serial.print("UART SEND:c\\r\\n   Return content: acceleration.\r\n");
  Serial.print("UART SEND:h\\r\\n   help.\r\n");
	Serial.print("******************************************************************************\r\n");
}

static void CmdProcess(void)
{
	switch(s_cCmd)
	{
		case 'a':	if(WitStartAccCali() != WIT_HAL_OK) Serial.print("\r\nSet AccCali Error\r\n");
			break;
		case 'm':	if(WitStartMagCali() != WIT_HAL_OK) Serial.print("\r\nSet MagCali Error\r\n");
			break;
		case 'e':	if(WitStopMagCali() != WIT_HAL_OK) Serial.print("\r\nSet MagCali Error\r\n");
			break;
		case 'u':	if(WitSetBandwidth(BANDWIDTH_5HZ) != WIT_HAL_OK) Serial.print("\r\nSet Bandwidth Error\r\n");
			break;
		case 'U':	if(WitSetBandwidth(BANDWIDTH_256HZ) != WIT_HAL_OK) Serial.print("\r\nSet Bandwidth Error\r\n");
			break;
		case 'B':	if(WitSetUartBaud(WIT_BAUD_115200) != WIT_HAL_OK) Serial.print("\r\nSet Baud Error\r\n");
              else 
              {
                Serial3.begin(c_uiBaud[WIT_BAUD_115200]);
                Serial.print(" 115200 Baud rate modified successfully\r\n");
              }
			break;
		case 'b':	if(WitSetUartBaud(WIT_BAUD_9600) != WIT_HAL_OK) Serial.print("\r\nSet Baud Error\r\n");
              else 
              {
                Serial3.begin(c_uiBaud[WIT_BAUD_9600]); 
                Serial.print(" 9600 Baud rate modified successfully\r\n");
              }
			break;
		case 'r': if(WitSetOutputRate(RRATE_1HZ) != WIT_HAL_OK)  Serial.print("\r\nSet Baud Error\r\n");
			        else Serial.print("\r\nSet Baud Success\r\n");
			break;
		case 'R':	if(WitSetOutputRate(RRATE_10HZ) != WIT_HAL_OK) Serial.print("\r\nSet Baud Error\r\n");
              else Serial.print("\r\nSet Baud Success\r\n");
			break;
    case 'C': if(WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG) != WIT_HAL_OK) Serial.print("\r\nSet RSW Error\r\n");
      break;
    case 'c': if(WitSetContent(RSW_ACC) != WIT_HAL_OK) Serial.print("\r\nSet RSW Error\r\n");
      break;
		case 'h':	ShowHelp();
			break;
		default :break;
	}
	s_cCmd = 0xff;
}
static void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
  Serial3.write(p_data, uiSize);
  Serial3.flush();
}
static void Delayms(uint16_t ucMs)
{
  delay(ucMs);
}
static void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)
{
	int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
            case AZ:
				s_cDataUpdate |= ACC_UPDATE;
            break;
            case GZ:
				s_cDataUpdate |= GYRO_UPDATE;
            break;
            case HZ:
				s_cDataUpdate |= MAG_UPDATE;
            break;
            case Yaw:
				s_cDataUpdate |= ANGLE_UPDATE;
            break;
            default:
				s_cDataUpdate |= READ_UPDATE;
			break;
        }
		uiReg++;
    }
}

static void AutoScanSensor(void)
{
	int i, iRetry;
	
	for(i = 0; i < sizeof(c_uiBaud)/sizeof(c_uiBaud[0]); i++)
	{
		Serial3.begin(c_uiBaud[i]);
    Serial3.flush();
		iRetry = 2;
		s_cDataUpdate = 0;
		do
		{
			WitReadReg(AX, 3);
			delay(200);
      while (Serial3.available())
      {
        WitSerialDataIn(Serial3.read());
      }
			if(s_cDataUpdate != 0)
			{
				Serial.print(c_uiBaud[i]);
				Serial.print(" baud find sensor\r\n\r\n");
				ShowHelp();
				return ;
			}
			iRetry--;
		}while(iRetry);		
	}
	Serial.print("can not find sensor\r\n");
	Serial.print("please check your connection\r\n");
}
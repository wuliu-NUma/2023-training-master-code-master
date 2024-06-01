# UART_V2.0 - By: FITQY - 周三 8 月 24 日 2022
#__________________________________________________________________
# 导入模块
import sensor, time, image                                  # 导入感光元件模块 sensor 跟踪运行时间模块 time 机器视觉模块 image
import utime                                                # 导入延时模块 utime
from fpioa_manager import fm                                # 从 fpioa_manager 模块中导入 引脚注册模块 fm
from Maix import GPIO                                       # 从 Maix 模块中导入 模块 GPIO
import lcd                                                  # 导入 LCD 模块
from machine import Timer, PWM, UART                        # 从 machine 模块中导入 定时器模块 Timer 脉宽调制模块 PWM 双向串行通信模块 UART
import math                                                 # 导入数学函数模块 math

#__________________________________________________________________
# 感光元件设置
sensor.reset()                                              # 重置并初始化感光元件 默认设置为 摄像头频率 24M 不开启双缓冲模式
#sensor.reset(freq=24000000, dual_buff=True)                # 设置摄像头频率 24M 开启双缓冲模式 会提高帧率 但内存占用增加

sensor.set_pixformat(sensor.RGB565)                         # 设置图像格式为 RGB565 (彩色) 除此之外 还可设置格式为 GRAYSCALE 或者 YUV422
sensor.set_framesize(sensor.QVGA)                           # 设置图像大小为 QVGA (320 x 240) 像素个数 76800 K210最大支持格式为 VGA

sensor.set_auto_exposure(0)                                 # 关闭自动曝光
#sensor.set_auto_exposure(0, exposure=120000)               # 设置手动曝光 曝光时间 120000 us

sensor.set_auto_gain(0)                                     # 关闭画面增益
sensor.set_auto_whitebal(0)                                 # 关闭RGB自动增益（白平衡）
#sensor.set_auto_gain(0, gain_db = 12)                      # 设置画面增益 17 dB 影响实时画面亮度
#sensor.set_auto_whitebal(0, rgb_gain_db = (0,0,0))         # 设置RGB增益 0 0 0 dB 影响画面色彩呈现效果 在 K210 上无法调节增益 初步判定是感光元件 ov2640 无法支持

#sensor.set_contrast(0)                                     # 设置对比度 0 这个参数无法读取 且调这个参数对画面似乎不会产生影响 暂时注释
#sensor.set_brightness(0)                                   # 设置亮度 0 这个参数无法读取 且调这个参数对画面似乎不会产生影响 暂时注释
#sensor.set_saturation(0)                                   # 设置饱和度 0 这个参数无法读取 且调这个参数对画面似乎不会产生影响 暂时注释

sensor.set_vflip(1)                                         # 打开垂直翻转 如果是 01Studio 的 K210 不开启会导致画面方向与运动方向相反
sensor.set_hmirror(1)                                       # 打开水平镜像 如果是 01Studio 的 K210 不开启会导致画面方向与运动方向相反

sensor.skip_frames(time = 2000)                             # 延时跳过2s 等待感光元件稳定

#__________________________________________________________________
# 创建时钟对象
clock = time.clock()                                        # 创建时钟对象 clock

#__________________________________________________________________
# 目标点输入类 举例 2022 年 TI 杯送货无人机 中的目标点输入部分
class Point_Input():
    point1  = 0                                             # 目标点 1
    point2  = 0                                             # 目标点 2
    cross   = 0                                             # 穿圈模式标志位
    send    = 0                                             # 目标点发送标志位

point = Point_Input()                                       # 实例化目标点输入类 Point_Input() 为 point

# 按键控制下的目标点获取函数
def Point_Control(ckey):
    if ckey.control == 1:                                   # 按键确认及发送控制标志位为1 即 按键3 按下
        ckey.control = 0                                    # 重置标志位
        if ckey.cs == 0:                                    # 如果当前为模式 0
            point.send = 1                                  # 目标点发送标志置为 1 串口开始发送

        elif ckey.cs == 1:                                  # 如果当前为模式 1
            point.point1 = ckey.cinput                      # 将按键输入值赋值给目标点 1

        elif ckey.cs == 2:                                  # 如果当前为模式 2
            point.point2 = ckey.cinput                      # 将按键输入值赋值给目标点 2

        elif ckey.cs == 3:                                  # 如果当前为模式 3
            point.cross = ckey.cinput                       # 将按键输入值赋值给 穿圈模式标志位

    if ckey.csflag == 1:                                    # 如果检测到按键模式切换
        ckey.csflag = 0                                     # 重置按键模式切换标志位
        ckey.cinput = 0                                     # 重置按键输入值

#__________________________________________________________________
# 按键的使用
# 定义按键控制类
class Key_Control():                                        # 定义按键控制类
    cnt     = 0                                             # 按键计数值
    cs      = 0                                             # 按键模式选择标志位
    csmax   = 0                                             # 按键模式上限
    csflag  = 0                                             # 按键模式切换标志位
    cinput  = 0                                             # 按键输入值保存位
    control = 0                                             # 按键确认及发送控制标志位

# 实例化按键类
key = Key_Control()                                         # 实例化按键控制类 Key_Control() 为 key
key.csmax = 3                                               # 按键模式上限为 3 即最多有 4 个模式 (0, 1, 2, 3)

# 注册按键引脚
fm.register(16, fm.fpioa.GPIOHS0, force = True)             # 配置 16 脚为 KEY0 使用高速 GPIO 口 强制注册
fm.register(18, fm.fpioa.GPIOHS1, force = True)             # 配置 18 脚为 KEY1 使用高速 GPIO 口 强制注册
fm.register(19, fm.fpioa.GPIOHS2, force = True)             # 配置 19 脚为 KEY2 使用高速 GPIO 口 强制注册
fm.register(20, fm.fpioa.GPIOHS3, force = True)             # 配置 20 脚为 KEY3 使用高速 GPIO 口 强制注册

# 创建按键对象
KEY0 = GPIO(GPIO.GPIOHS0, GPIO.IN, GPIO.PULL_UP)            # 创建按键对象 KEY0
KEY1 = GPIO(GPIO.GPIOHS1, GPIO.IN, GPIO.PULL_UP)            # 创建按键对象 KEY1
KEY2 = GPIO(GPIO.GPIOHS2, GPIO.IN, GPIO.PULL_UP)            # 创建按键对象 KEY2
KEY3 = GPIO(GPIO.GPIOHS3, GPIO.IN, GPIO.PULL_UP)            # 创建按键对象 KEY3

# 中断回调函数 KEY0 控制按键模式选择
def Key0_Switch(KEY0):
    utime.sleep_ms(10)                                      # 延时 10ms 消除按键抖动
    if KEY0.value() == 0:                                   # 确认 按键0 按下
        key.csflag = 1                                      # 标记按键模式切换
        if key.cs < key.csmax:                              # 若当前按键模式值小于按键模式选择上限值
            key.cs = key.cs + 1                             # 控制按键模式选择 自增
        else:                                               # 若达到上限 则重新从 0 开始
            key.cs = 0

# 中断回调函数 KEY1 按键输入值自增
def Key1_Switch(KEY1):
    utime.sleep_ms(10)                                      # 延时 10ms 消除按键抖动
    if KEY1.value() == 0:                                   # 确认 按键1 按下
        key.cinput = key.cinput + 1                         # 按键输入值自增

# 中断回调函数 KEY2 按键输入值自减
def Key2_Switch(KEY2):
    utime.sleep_ms(10)                                      # 延时 10ms 消除按键抖动
    if KEY2.value() == 0:                                   # 确认 按键2 按下
        key.cinput = key.cinput - 1                         # 按键输入值自减

# 中断回调函数 KEY3 按键确认及发送控制标志位
def Key3_Switch(KEY3):
    utime.sleep_ms(10)                                      # 延时 10ms 消除按键抖动
    if KEY3.value() == 0:                                   # 确认按键按下
        key.control = 1                                     # 按键确认及发送控制标志位

# 开启中断 下降沿触发
KEY0.irq(Key0_Switch, GPIO.IRQ_FALLING)                     # 开启 按键0 外部中断 下降沿触发
KEY1.irq(Key1_Switch, GPIO.IRQ_FALLING)                     # 开启 按键1 外部中断 下降沿触发
KEY2.irq(Key2_Switch, GPIO.IRQ_FALLING)                     # 开启 按键2 外部中断 下降沿触发
KEY3.irq(Key3_Switch, GPIO.IRQ_FALLING)                     # 开启 按键3 外部中断 下降沿触发

#__________________________________________________________________
# LED的使用
# 注册 LED 引脚
fm.register(14, fm.fpioa.GPIO2, force = True)               # 配置 14 脚为 LED_R 强制注册
fm.register(13, fm.fpioa.GPIO1, force = True)               # 配置 13 脚为 LED_G 强制注册
fm.register(12, fm.fpioa.GPIO0, force = True)               # 配置 12 脚为 LED_B 强制注册

# 创建 LED 对象
LED_R = GPIO(GPIO.GPIO2, GPIO.OUT)                          # 创建 LED_R 对象
LED_G = GPIO(GPIO.GPIO1, GPIO.OUT)                          # 创建 LED_G 对象
LED_B = GPIO(GPIO.GPIO0, GPIO.OUT)                          # 创建 LED_B 对象

# LED控制函数
def LED_Control(led_flag):                                  # LED控制函数 根据传入 led_flag 点亮对应的灯 需要注意的是 0为点亮 1为熄灭
    if led_flag == 0:                                       # 传入参数为 0 所有灯打开
        LED_R.value(0)
        LED_G.value(0)
        LED_B.value(0)

    elif led_flag == 1:                                     # 传入参数为 1 所有灯关闭
        LED_R.value(1)
        LED_G.value(1)
        LED_B.value(1)

    elif led_flag == 2:                                     # 传入参数为 2 红灯常亮
        LED_R.value(0)
        LED_G.value(1)
        LED_B.value(1)

    elif led_flag == 3:                                     # 传入参数为 3 绿灯常亮
        LED_R.value(1)
        LED_G.value(0)
        LED_B.value(1)

    elif led_flag == 4:                                     # 传入参数为 4 蓝灯常亮
        LED_R.value(1)
        LED_G.value(1)
        LED_B.value(0)

    else:                                                   # 其他情况 紫灯
        LED_R.value(0)
        LED_G.value(1)
        LED_B.value(0)

#__________________________________________________________________
# 定时器的使用
# 定义定时器属性类
class Timer_Property():
    cnt     = 0                                             # 定时器计数值
    cnt_max = 0                                             # 定时器计数值上限
    period  = 0                                             # 定时器周期
    freq    = 0                                             # 定时器频率

# 定时器0 配置_______________________________________________________
# 定时器0 实例化类
timer0 = Timer_Property()                                   # 实例化定时器属性类 Timer_Property() 为 timer0
timer0.cnt_max = 9                                          # 设定 定时器0 的计数值上限为 9
timer0.period = 100                                         # 设定 定时器0 的周期为 100

# 定时器0 定义回调函数
def Timer0_Back(tim0):
    if timer0.cnt < timer0.cnt_max:                         # 若 定时器0 的计数值小于 定时器0 的计数值上限
        timer0.cnt = timer0.cnt + 1                         # 计数值自增
    else:
        timer0.cnt = 0                                      # 超出计数值上限 则计数值重置为0

# 定时器0 初始化
tim0 = Timer(Timer.TIMER0,                                  # 定时器编号 定时器0
            Timer.CHANNEL0,                                 # 定时器通道 通道0
            mode = Timer.MODE_PERIODIC,                     # 定时器模式 周期性
            unit = Timer.UNIT_MS,                           # 定时器周期单位 ms
            period = timer0.period,                         # 定时器周期 timer0.period 若 unit 为 Timer.UNIT_MS 则周期为 timer0.period ms
            callback = Timer0_Back)                         # 定时器触发中断后执行的回调函数 Timer0_Back

# 定时器1 配置_______________________________________________________
# 电机类定义
class Motor_Property():
    motor1      = 0                                         # 电机1 占空比
    motor2      = 0                                         # 电机2 占空比
    motor3      = 0                                         # 电机3 占空比
    motor4      = 0                                         # 电机4 占空比

    motor1_pin  = 0                                         # 电机1 引脚
    motor2_pin  = 0                                         # 电机2 引脚
    motor3_pin  = 0                                         # 电机3 引脚
    motor4_pin  = 0                                         # 电机4 引脚

    control_x   = 0                                         # 被控坐标 x
    control_y   = 0                                         # 被控坐标 y

# 实例化电机类
motor = Motor_Property()                                    # 实例化电机类 Motor_Property() 为 motor
motor.motor1 = 50                                           # 电机1的占空比 初始设置为 50%
motor.motor2 = 50                                           # 电机2的占空比 初始设置为 50%
motor.motor1_pin  = 14                                      # 电机1的引脚 14为红灯引脚 这里先用灯的亮灭观察效果
motor.motor2_pin  = 13                                      # 电机2的引脚 13为绿灯引脚 这里先用灯的亮灭观察效果

# 定时器1 实例化类
timer1 = Timer_Property()                                   # 实例化定时器属性类 Timer_Property() 为 timer1
timer1.freq = 1000                                          # 设定 定时器1 的频率为 1000

# 定时器1 通道0 初始化
tim1_ch0 = Timer(Timer.TIMER1,                              # 定时器编号 定时器1
                 Timer.CHANNEL0,                            # 定时器通道 通道0
                 mode = Timer.MODE_PWM)                     # 定时器模式 PWM

# 定时器1 通道1 初始化
tim1_ch1 = Timer(Timer.TIMER1,                              # 定时器编号 定时器1
                 Timer.CHANNEL1,                            # 定时器通道 通道1
                 mode = Timer.MODE_PWM)                     # 定时器模式 PWM

# 创建对象 电机1 通道为 定时器1的通道0 频率为 定时器1的频率 占空比为 电机1的占空比 引脚为 电机1的引脚
motor1 = PWM(tim1_ch0, freq = timer1.freq, duty = motor.motor1, pin = motor.motor1_pin)

# 创建对象 电机2 通道为 定时器1的通道1 频率为 定时器1的频率 占空比为 电机2的占空比 引脚为 电机2的引脚
motor2 = PWM(tim1_ch1, freq = timer1.freq, duty = motor.motor2, pin = motor.motor2_pin)

# 定义电机占空比控制函数
def Motor_Control(motor, x):
    val = 0
    if x < motor.control_x:                                 # 若 当前坐标 小于 被控坐标x 即当前状态小车在目标的 左边
        val = (motor.control_x - x) * 0.3125                # 获取坐标差值 并转换为 0~50 之间的值
        motor.motor1 = 50 - val                             # 减小 电机1 占空比 电机1为左电机 使小车右转
        motor.motor2 = 50 + val                             # 增大 电机2 占空比 电机2为右电机 使小车右转

    elif x > motor.control_x:                               # 若 当前坐标 大于 被控坐标x 即当前状态小车在目标的 右边
        val = (x - motor.control_x) * 0.3125                # 获取坐标差值 并转换为 0~50 之间的值
        motor.motor1 = 50 + val                             # 增大 电机1 占空比 电机1为左电机 使小车左转
        motor.motor2 = 50 - val                             # 减小 电机2 占空比 电机2为右电机 使小车左转

    motor.motor1 = int(motor.motor1)                        # 将 电机1占空比 转换为 整数
    motor.motor2 = int(motor.motor2)                        # 将 电机1占空比 转换为 整数

#__________________________________________________________________
# 串口的使用
# 串口1 设置 P9 RX P6 TX
fm.register(9, fm.fpioa.UART1_RX, force = True)             # 配置 9 脚为 UART1_RX 强制注册
fm.register(6, fm.fpioa.UART1_TX, force = True)             # 配置 6 脚为 UART1_TX 强制注册

uart1 = UART(UART.UART1, 115200, 8, 0, 1)                   # 设置 uart1 为 串口1 波特率 921600 数据位 8位 校验位 0位 停止位 1位

# 串口2 设置 P7 RX P8 TX
fm.register(7, fm.fpioa.UART2_RX, force = True)             # 配置 7 脚为 UART2_RX 强制注册
fm.register(8, fm.fpioa.UART2_TX, force = True)             # 配置 8 脚为 UART2_TX 强制注册

uart2 = UART(UART.UART2, 921600, 8, 0, 1)                   # 设置 uart2 为 串口2 波特率 921600 数据位 8位 校验位 0位 停止位 1位

#__________________________________________________________________
# 串口发送
# 定义 UART 发送类
class UART_Transmit():                                      # 定义 UART 发送类
    pack_flag   = 0                                         # 打包方法标志位
    head1  = 0x00                                           # uint8_t   帧头1
    head2  = 0x00                                           # uint8_t   帧头2
    x      = 0                                              # uint16_t  目标x轴坐标
    y      = 0                                              # uint16_t  目标y轴坐标
    color  = 0                                              # uint8_t   目标颜色标志位
    shape  = 0                                              # uint8_t   目标形状标志位
    flag   = 0                                              # uint8_t   目标标志位
    mode   = 0                                              # uint8_t   模式标志位

# 实例化类
TSTM32  = UART_Transmit()                                   # 实例化 UART_Transmit() 为 TSTM32
TSTM32.pack_flag  = 1                                       # 打包方法为 方法1
TSTM32.head1 = 0xAA                                         # TSTM32 的帧头1为 0xAA
TSTM32.head2 = 0xAA                                         # TSTM32 的帧头2为 0xAA

TOpenMV = UART_Transmit()                                   # 实例化 UART_Transmit() 为 TOpenMV
TOpenMV.pack_flag  = 2                                      # 打包方法为 方法2
TOpenMV.head1 = 0xAA                                        # TOpenMV 的帧头1为 0xAA
TOpenMV.head2 = 0xAA                                        # TOpenMV 的帧头2为 0xAA

# 定义打包函数
def Pack_Data(TData):

    data = UART_Pack_Method(TData)                          # 根据不同的方法打包发送数据

    # 数据包的长度
    data_len = len(data)                                    # 获得数据包总长度
    data[2]  = data_len - 4                                 # 有效数据的长度 扣去 帧头1 帧头2 有效数据长度位 校验位

    # 校验和
    sum = 0                                                 # 和置零
    for i in range(0,data_len-1):
        sum = sum + data[i]                                 # 和累加
    data[data_len-1] = sum                                  # 和赋值 给数组最后一位发送 只保存低8位 溢出部分无效

    # 返回打包好的数据
    return data

#__________________________________________________________________
# 串口接收
# 定义 UART 接收类
class UART_Receive(object):                                 # 定义 UART 接收类
    uart_buf  = []                                          # 串口缓冲区数组
    data_len  = 0                                           # 有效数据长度
    data_cnt  = 0                                           # 总数据长度
    state     = 0                                           # 接收状态
    buf_len   = 0                                           # 保存串口等待字节的数量

    head1     = 0x00                                        # 接收帧头1
    head2     = 0x00                                        # 接收帧头2
    lenmax    = 0                                           # 有效数据最大长度
    save_flag = 0                                           # 保存方法标志位

# 实例化类
RSTM32  = UART_Receive()                                    # 实例化 UART_Receive() 为 RSTM32
RSTM32.head1        = 0xAA                                  # RSTM32 的帧头1为 0xAA
RSTM32.head2        = 0xAA                                  # RSTM32 的帧头2为 0xAA
RSTM32.lenmax       = 40                                    # RSTM32 的有效数据最大长度为 40
RSTM32.save_flag    = 1                                     # RSTM32 的保存方法标志位为 1

ROpenMV = UART_Receive()                                    # 实例化 UART_Receive() 为 ROpenMV
ROpenMV.head1       = 0xAA                                  # ROpenMV 的帧头1为 0xAA
ROpenMV.head2       = 0xAA                                  # ROpenMV 的帧头2为 0xAA
ROpenMV.lenmax      = 40                                    # ROpenMV 的有效数据最大长度为 40
ROpenMV.save_flag   = 2                                     # ROpenMV 的保存方法标志位为 2

# 定义串口数据读取函数
def UART_Read(RData, uart):
    RData.buf_len = uart.any()                              # 检查 串口 是否有内容需要读取 返回等待的字节数量（可能为0）
    for i in range(0, RData.buf_len):                       # 读取 RData.buf_len 个数据
        Receive_Data(RData, uart.readchar())                # 接收单个数据 uart.readchar() 然后将这个数据传递到函数 Receive_Data() 进行 数据接收

# 定义串口数据接收函数
def Receive_Data(RData, buf):
    if RData.state == 0 and buf == RData.head1:             # 判断帧头1是否符合要求 符合则进入下一个状态
        RData.state = 1                                     # 更改状态为 1
        RData.uart_buf.append(buf)                          # 将这个数据添加到数组末尾

    elif RData.state == 1 and buf == RData.head2:           # 判断帧头2是否符合要求 符合则进入下一个状态
        RData.state = 2                                     # 更改状态为 2
        RData.uart_buf.append(buf)                          # 将这个数据添加到数组末尾

    elif RData.state == 2 and buf < RData.lenmax:           # 有效数据长度位 规定有效数据长度小于40 符合则进入下一个状态
        RData.state = 3                                     # 更改状态为 3
        RData.data_len = buf                                # 获得有效数据长度
        RData.data_cnt = buf + 4                            # 获得总数据长度 总数据长度 = 帧头1 + 帧头2 + 有效数据长度位 + 有效数据 + 校验位
        RData.uart_buf.append(buf)                          # 将这个数据添加到数组末尾

    elif RData.state == 3 and RData.data_len > 0:           # 存储有效数据长度个数据
        RData.data_len = RData.data_len - 1                 # 每存储一次 还需要存储的数据个数减1
        RData.uart_buf.append(buf)                          # 将这个数据添加到数组末尾
        if RData.data_len == 0:                             # 直到存储完毕
            RData.state = 4                                 # 进入下一个状态

    elif RData.state == 4:                                  # 当接收到存储完毕的信息
        RData.uart_buf.append(buf)                          # 保存最后一位校验位 将这个数据添加到数组末尾
        RData.state = 0                                     # 状态重置为0 调用串口数据解析函数进行数据解析
        Parse_Data(RData)                                   # 解析数据
        #print(RData.uart_buf)                              # 打印接收数组 若接收结果不对 可取消注释查看数组
        RData.uart_buf = []                                 # 清空缓冲区 准备下次接收数据

    else:                                                   # 不满足以上条件 视为接收出错 重置状态为0 丢弃所有数据 准备下一次接收数据
        RData.state = 0                                     # 重置状态为0
        RData.uart_buf = []                                 # 清空缓冲区 准备下一次接收数据

# 定义串口数据解析函数
def Parse_Data(PData):
    # 和累加
    sum = 0                                                 # 和置0
    i = 0                                                   # 已循环次数置0
    while i < (PData.data_cnt - 1):                         # 循环累加
        sum = sum + PData.uart_buf[i]                       # 累加求数组和
        i = i + 1                                           # 已循环次数自增
    # 求余 因为 校验和 为 8 位 超出部分无效 因此只校验 低8位 即可
    sum = sum % 256                                         # 和对256取余 得低八位

    # 和校验失败则退出
    if sum != PData.uart_buf[PData.data_cnt - 1]:           # 和取余结果若不等于校验位的值
        return                                              # 退出

    # 和校验成功则根据保存方法的不同 接收数据
    UART_Save_Method(PData)

#__________________________________________________________________
# 寻找色块
# 定义类
class Color_Property():
    cx                      =  0                            # 色块 x轴 中心坐标
    cy                      =  0                            # 色块 y轴 中心坐标
    flag                    =  0                            # 色块标志位 1 找到 0 未找到
    color                   =  0                            # 色块颜色标志位 例如 你可以用 1 来表示 黑色
    density                 =  0                            # 色块密度比 反映色块锁定程度 值越大 锁定程度越好
    pixels_max              =  0                            # 色块像素最大值
    led_flag                =  0                            # LED标志位 方便调试用

    color_threshold         = (0, 0, 0, 0, 0, 0)            # 色块颜色阈值
    color_roi               = (0,0,320,240)                 # 色块寻找区域（感兴趣区域）
    color_x_stride          =  1                            # 色块 x轴 像素最小宽度 色块如果比较大可以调大此参数 提高寻找速度
    color_y_stride          =  1                            # 色块 y轴 像素最小宽度 色块如果比较大可以调大此参数 提高寻找速度
    color_pixels_threshold  =  100                          # 色块 像素个数阈值 例如调节此参数为100 则可以滤除色块像素小于100的色块
    color_area_threshold    =  100                          # 色块 被框面积阈值 例如调节此参数为100 则可以滤除色块被框面积小于100的色块
    color_merge             =  True                         # 是否合并寻找到的色块 True 则合并 False 则不合并
    color_margin            =  1                            # 色块合并间距 例如调节此参数为1 若上面选择True合并色块 且被找到的色块有多个 相距1像素 则会将这些色块合并

# 实例化类
# 黑色
black = Color_Property()
black.color_threshold         = (0, 50, -10, 10, -10, 10)
black.color_roi               = (0,0,320,240)
black.color_x_stride          =  1
black.color_y_stride          =  1
black.color_pixels_threshold  =  100
black.color_area_threshold    =  100
black.color_merge             =  True
black.color_margin            =  1

# 红色
red   = Color_Property()
red.color_threshold           = (0, 100, 20, 127, -10, 127)

#red.color_roi                = (0,0,320,240)
red.color_roi                 = (0,110,320,20)

red.color_x_stride            =  1
red.color_y_stride            =  1

#red.color_pixels_threshold   =  100
#red.color_area_threshold     =  100
red.color_pixels_threshold    =  10
red.color_area_threshold      =  10

red.color_merge               =  True
red.color_margin              =  1

# 绿色 预留
green = Color_Property()

# 蓝色 预留
blue  = Color_Property()

# 定义寻找色块函数
def K210_Find_Blobs(color,led_flag):
    color.pixels_max = 0                                    # 重置 色块 最大像素数量
    color.flag       = 0                                    # 重置 色块 标志位
    color.led_flag   = 0                                    # 重置 led 标志位

    for blobs in img.find_blobs([color.color_threshold],    # 色块颜色阈值
    roi = color.color_roi,                                  # 色块寻找区域（感兴趣区域）
    x_stride = color.color_x_stride,                        # 色块 x轴 像素最小宽度 色块如果比较大可以调大此参数 提高寻找速度
    y_stride = color.color_y_stride,                        # 色块 y轴 像素最小宽度 色块如果比较大可以调大此参数 提高寻找速度
    pixels_threshold = color.color_pixels_threshold,        # 色块 像素个数阈值 例如调节此参数为100 则可以滤除色块像素小于100的色块
    area_threshold = color.color_area_threshold,            # 色块 被框面积阈值 例如调节此参数为100 则可以滤除色块被框面积小于100的色块
    merge = color.color_merge,                              # 是否合并寻找到的色块 True 则合并 False 则不合并
    margin = color.color_margin):                           # 色块合并间距 例如调节此参数为1 若上面选择True合并色块 且被找到的色块有多个 相距1像素 则会将这些色块合并
        img.draw_rectangle(blobs[0:4])                      # 圈出找到的色块
        if color.pixels_max < blobs.pixels():               # 找到面积最大的色块
            color.pixels_max = blobs.pixels()
            color.cx = blobs.cx()                           # 将面积最大的色块的 x轴 中心坐标值 赋值给 color
            color.cy = blobs.cy()                           # 将面积最大的色块的 y轴 中心坐标值 赋值给 color
            color.flag = 1                                  # 标志画面中有找到色块
            color.density = blobs.density()                 # 将面积最大的色块的 色块密度比 赋值给 color
            color.led_flag = led_flag                       # 将控制led颜色的标志位的值 赋值给 color

    if color.flag == 1:                                     # 标记画面中被找到的最大色块的中心坐标
        img.draw_cross(color.cx,color.cy, color=127, size = 15)
        img.draw_circle(color.cx,color.cy, 15, color = 127)

#__________________________________________________________________
# 调试区
# 定义 K210 属性类
class K210_Property(object):                                # 定义 K210 接收类
    x      = 0                                              # uint16_t  目标x轴坐标
    y      = 0                                              # uint16_t  目标y轴坐标
    color  = 0                                              # uint8_t   目标颜色标志位
    shape  = 0                                              # uint8_t   目标形状标志位
    flag   = 0                                              # uint8_t   目标标志位
    mode   = 0                                              # uint8_t   工作模式位

# 实例化类
K210 = K210_Property()                                      # 实例化 K210_Property() 为 K210

LED_Control(1)                                              # 关闭一下所有灯 再进入 while 循环 使显示结果正确

# 串口发送测试信息赋值
TSTM32.mode   = 1

TOpenMV.x     = 65535
TOpenMV.y     = 65536
TOpenMV.color = 255
TOpenMV.shape = 256
TOpenMV.flag  = 3

# 打印信息函数
# 打印 sensor 各参数
def Print_sensor():
    print("Exposure :", sensor.get_exposure_us(), "Gain:", sensor.get_gain_db(), "RGB:", sensor.get_rgb_gain_db())

# 打印 K210 各参数
def Print_K210():
    print("Mode:", K210.mode, "x:", K210.x, "y:", K210.y, "color:", K210.color, "shape:", K210.shape, "flag:", K210.flag)

# 打印 色块 各参数
def Print_Blobs_Property(color,name):
    print(name,"cx:",color.cx,"cy:",color.cy,"flag:",color.flag,"color:",color.color,"density:",color.density,"led_flag:",color.led_flag)

# 打印总函数
def Print_All():
    print("______________________________________________________________________")
    Print_sensor()                                          # 打印 sensor 参数
    Print_Blobs_Property(black,"Black ")                    # 打印 黑色色块 参数
    Print_Blobs_Property(red,  "Red   ")                    # 打印 红色色块 参数
    Print_K210()                                            # 打印 K210 参数

# LCD
# LCD 初始化
lcd.init()                                                  # lcd初始化

# LCD 按键信息及目标点信息显示函数
def LCD_Show():
    lcd.draw_string(0, 0,  "key_cs: "+str(key.cs), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 15, "cinput: "+str(key.cinput), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 30, "point1: "+str(point.point1), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 45, "point2: "+str(point.point2), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 60, "cross : "+str(point.cross), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 75, "red_cx: "+str(red.cx), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 90, "motor1: "+str(motor.motor1), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 105,"motor2: "+str(motor.motor2), lcd.BLUE, lcd.WHITE)
    lcd.draw_string(0, 120,"FPS   : "+str(clock.fps()), lcd.BLUE, lcd.WHITE)

# 串口数据打包方法函数
def UART_Pack_Method(TData):
    if TData.pack_flag == 1:
        data = bytearray([TData.head1,                      # 帧头1
                          TData.head2,                      # 帧头2
                          0x00,                             # 有效数据长度 0x00 + data_len - 4
                          TData.mode,                       # 保存目标x轴坐标 高八位
                          0x00])                            # 数据和校验位

    elif TData.pack_flag == 2:
        data = bytearray([TData.head1,                      # 帧头1
                          TData.head2,                      # 帧头2
                          0x00,                             # 有效数据长度 0x00 + data_len - 4
                          TData.x>>8,                       # 保存目标x轴坐标 高八位
                          TData.x,                          # 保存目标x轴坐标 低八位
                          TData.y>>8,                       # 保存目标y轴坐标 高八位
                          TData.y,                          # 保存目标y轴坐标 低八位
                          TData.color,                      # 保存目标颜色标志位
                          TData.shape,                      # 保存目标形状标志位
                          TData.flag,                       # 保存目标标志位
                          0x00])                            # 数据和校验位

    return data

# 串口数据保存方法函数
def UART_Save_Method(PData):
    if PData.save_flag == 1 and PData.uart_buf[2] > 0:
        K210.mode =  PData.uart_buf[3]

    elif PData.save_flag == 2 and PData.uart_buf[2] > 6:
        K210.x      = PData.uart_buf[3]*256 + PData.uart_buf[4]
        K210.y      = PData.uart_buf[5]*256 + PData.uart_buf[6]
        K210.color  = PData.uart_buf[7]
        K210.shape  = PData.uart_buf[8]
        K210.flag   = PData.uart_buf[9]

#__________________________________________________________________
# 主函数
while(True):

    clock.tick()                                            # 跟踪运行时间

    # 模式选择______________________________________________________

    if K210.mode == 0x00:
        img=sensor.snapshot()                               # 拍摄一张照片

    elif K210.mode == 0x01:
        img=sensor.snapshot()                               # 拍摄一张照片
        K210_Find_Blobs(black,1)                            # 找黑色色块 LED标志为1 表示黑色
        motor.control_x = 160                               # 控制目标处于 x轴中心点 160
        Motor_Control(motor,black.cx)                       # 电机占空比控制函数获取电机控制占空比
        motor1.duty(motor.motor1)                           # 将获取到的电机1占空比 装载
        motor2.duty(motor.motor2)                           # 将获取到的电机2占空比 装载

    elif K210.mode == 0x02:
        img=sensor.snapshot()                               # 拍摄一张照片
        K210_Find_Blobs(red,2)                              # 找黑色色块 LED标志为1 表示黑色
        motor.control_x = 160                               # 控制目标处于 x轴中心点 160
        Motor_Control(motor,red.cx)                         # 电机占空比控制函数获取电机控制占空比
        motor1.duty(motor.motor1)                           # 将获取到的电机1占空比 装载
        motor2.duty(motor.motor2)                           # 将获取到的电机2占空比 装载

    elif K210.mode == 0x03:
        img=sensor.snapshot()                               # 拍摄一张照片

    else:
        img=sensor.snapshot()                               # 拍摄一张照片

    # 模式选择______________________________________________________

    Point_Control(key)                                      # 按键控制下的目标点获取函数

    lcd.display(img)                                        # LCD 显示图像
    LCD_Show()                                              # LCD 显示按键信息及目标点信息

    #LED_Control(red.led_flag)                              # LED 标记色块识别情况

    UART_Read(RSTM32,uart1)                                 # 串口1 数据接收
    UART_Read(ROpenMV,uart2)                                # 串口2 数据接收

    if timer0.cnt == 0:                                     # 如果 timer0.cnt 等于 0 此步骤的目的是控制打印周期 不要打印的太快
        Print_All()                                         # 打印各参数
        uart1.write(Pack_Data(TSTM32))                      # 串口1 数据发送
        uart2.write(Pack_Data(TOpenMV))                     # 串口2 数据发送

#__________________________________________________________________



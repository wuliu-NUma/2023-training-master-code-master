# Untitled - By: 10416 - 周日 7月 9 2023

import sensor, image, time, json
import utime
from fpioa_manager import fm
from Maix import GPIO
import lcd
from machine import Timer, PWM, UART
import math
import struct
import re
import utime as t
from machine import I2C

red_threshold=(23, 77, 18, 101, -16, 96)        #亮处红色阈值(25, 41, 14, 44, -12, 33)  (19, 56, 4, 76, 4, 127  (35, 80, -36, 44, 27, 5) (35, 63, 53, 7, 1, 32)
red_threshold_dark=(0, 21, 15, 40, -4, 28)    #暗处红色阈值(18, 26, 2, 41, 16, 33)
green_threshold =  (9, 87, -67, -11, -2, 66)    #亮处绿色阈值(32, 65, -49, -19, 30, -36)(23, 66, -59, -19, 13, 49)(52, 84, -38, -8, -2, 46) (52, 84, -36, -4, -1, 17) (43, 81, -38, -20, -12, 25)
green_threshold_dark=(3, 7, -51, -6, -20, 24)    #暗处绿色阈值(20, 41, -33, -18, -12, 33) (38, 66, -46, 1, 39, 7)
blue_threshold=(10, 35, -2, 41, -54, -19)   #亮处蓝色阈值(11, 63, -5, 36, -52, -35)  (39, 58, -4, 15, -53, -35)(44, 65, -26, 23, -52, -1)(19, 49, 0, 123, -119, -8)
blue_threshold_dark=(0, 23, 1, 10, -30, -5)    #暗处蓝色阈值(8, 15, -3, 14, -23, -8)
color_template=[red_threshold,green_threshold,blue_threshold]
color_template_dark=[red_threshold_dark,green_threshold_dark,blue_threshold_dark]
clor_name=["R","G","B"]
environment="light"                  #更改此参数为"dark"更改为暗光条件下的阈值（该阈值需手动调整）
target_blob_area=[3000,50000]           #物料捕捉面积最大值与最小值限制，防止捕捉到错误目标250000以上
target_blob_area2=[2000,60000]           #物料捕捉面积最大值与最小值限制，防止捕捉到错误目标
target_proportion = (1.1,2.5)        #色块长宽W/H比例限制 ，防止捕捉到错误目标
target_1=[[0,0],[0,0],[0,0]]        #下层物料坐标     按红绿蓝排列
target_2=[[0,0],[0,0],[0,0]]        #上层物料坐标
temp_sort_1=[1,2,3]
temp_sort_2=[1,2,3]
target_flag=[0,0,0]
store_up=[1,2,3]                                #原料区上层从左到右的摆放情况 1=R 2=G 3=B
store_down=[1,2,3]
find_target_time=0
min_degree = 80
max_degree = 100

graythreshold=[(100,255)]#二值化阈值
kernel_size = 1
kernel = [-1, -1, -1,\
          -1, +8, -1,\
          -1, -1, -1]
thresholds = [(42, 58, -11, 4, -4, 19)] # grayscale thresholds设置阈值



def compareBlob(blob1,blob2):#比较色块后返回最大值
    tmp = blob1.pixels() - blob2.pixels()
    if tmp == 0:
        return 0;
    elif tmp > 0:
        return 1;
    else:
        return -1;






class STM32_transmit():                              # 定义 STM32 发送类
    head1  = 0xAA                                    # uint8_t   帧头1
    head2  = 0xAA                                    # uint8_t   帧头2
    x      = 0                                       # uint16_t  目标x轴坐标
    y      = 0                                       # uint16_t  目标y轴坐标
    color  = 0                                       # uint8_t   目标颜色标志位

    #flag2 = 0

# 实例化类
TSTM32 = STM32_transmit()                            # 实例化 STM32_transmit() 为 TSTM32

TSTM32.x     = 0
TSTM32.y     = 0
TSTM32.color = 0




class K210_Property(object):                                # 定义 K210 接收类
    x      = 0                                              # uint16_t  目标x轴坐标
    y      = 0                                              # uint16_t  目标y轴坐标
    color  = 0                                              # uint8_t   目标颜色标志位
    mode   = 0                                              # uint8_t   工作模式位

# 实例化类
K210 = K210_Property()



# 定义打包函数
def TSTM32_data():                                   # 数据打包函数
    data=bytearray([TSTM32.head1,                    # 帧头1
                    TSTM32.head2,                    # 帧头2
                    0x00,                            # 有效数据长度 0x00 + data_len - 4
                    TSTM32.x>>8,TSTM32.x,            # 保存目标坐标x 将整形数据拆分成两个8位
                    TSTM32.y>>8,TSTM32.y,            # 保存目标坐标y 将整形数据拆分成两个8位
                    TSTM32.color,                    # 保存目标颜色标志位
                    #TSTM32.flag2,
                    0x00])                           # 数据和校验位

    # 数据包的长度
    data_len = len(data)                             # 获得数据包总长度
    data[2]  = data_len - 4                          # 有效数据的长度 扣去 帧头1 帧头2 有效数据长度位 校验位

    # 校验和
    sum = 0                                          # 和置零
    for i in range(0,data_len-1):
        sum = sum + data[i]                          # 和累加
    data[data_len-1] = sum                           # 和赋值 给数组最后一位发送 只保存低8位 溢出部分无效

    # 返回打包好的数据
    return data

def LX_data():                                   # 数据打包函数
    data=bytearray([LX.head1,                    # 帧头1
                    LX.head2,                    # 帧头2
                    0x00,                            # 有效数据长度 0x00 + data_len - 4
                    LX.x>>8,TSTM32.x,            # 保存目标坐标x 将整形数据拆分成两个8位
                    LX.y>>8,TSTM32.y,            # 保存目标坐标y 将整形数据拆分成两个8位
                    LX.color,                    # 保存目标颜色标志位
                    #TSTM32.flag2,
                    0x00])                           # 数据和校验位

    # 数据包的长度
    data_len = len(data)                             # 获得数据包总长度
    data[2]  = data_len - 4                          # 有效数据的长度 扣去 帧头1 帧头2 有效数据长度位 校验位

    # 校验和
    sum = 0                                          # 和置零
    for i in range(0,data_len-1):
        sum = sum + data[i]                          # 和累加
    data[data_len-1] = sum                           # 和赋值 给数组最后一位发送 只保存低8位 溢出部分无效

    # 返回打包好的数据
    return data



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


def UART_Read(RData, uart):
    RData.buf_len = uart.any()                              # 检查 串口 是否有内容需要读取 返回等待的字节数量（可能为0）
    for i in range(0, RData.buf_len):                       # 读取 RData.buf_len 个数据
        Receive_Data(RData, uart.readchar())                # 接收单个数据 uart.readchar() 然后将这个数据传递到函数 Receive_Data() 进行 数据接收

def UART_Read2(RData, uart):
    RData.buf_len = uart.any()                              # 检查 串口 是否有内容需要读取 返回等待的字节数量（可能为0）
    for i in range(0, RData.buf_len):                       # 读取 RData.buf_len 个数据
        Receive_Data2(RData, uart.readchar())                # 接收单个数据 uart.readchar() 然后将这个数据传递到函数 Receive_Data() 进行 数据接收

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

def Receive_Data2(RData, buf):
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
        Parse_Data2(RData)                                   # 解析数据
        #print(RData.uart_buf)                              # 打印接收数组 若接收结果不对 可取消注释查看数组
        RData.uart_buf = []                                 # 清空缓冲区 准备下次接收数据

    else:                                                   # 不满足以上条件 视为接收出错 重置状态为0 丢弃所有数据 准备下一次接收数据
        RData.state = 0                                     # 重置状态为0
        RData.uart_buf = []                                 # 清空缓冲区 准备下一次接收数据

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
    K210.mode =  PData.uart_buf[3]

def Parse_Data2(PData):
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
    TSTM32.x =  PData.uart_buf[3]*100+PData.uart_buf[4]*10+PData.uart_buf[5]
    TSTM32.y =  PData.uart_buf[6]*100+PData.uart_buf[7]*10+PData.uart_buf[8]
    uart2.write(TSTM32_data())



def qrcode_recive():
    img = sensor.snapshot()
    res = img.find_qrcodes()
    fps =clock.fps()
    if len(res) > 0:
        img.draw_string(2,2, res[0].payload(), color=(0,128,0), scale=2)
        print(res[0].payload())
    lcd.display(img)

def sort(a,length,b):
    for j in range(0,length):
        for i in range(0,length-1-j):
            if a[i]<a[i+1]:
                t=a[i]
                a[i]=a[i+1]
                a[i+1]=t
                t1=b[i]
                b[i]=b[i+1]
                b[i+1]=t1
    return b



sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_exposure(1)
sensor.set_auto_gain(0, gain_db = 19)
sensor.set_auto_whitebal(0, rgb_gain_db = (0,0,0))
sensor.skip_frames(30)
sensor.set_vflip(1)
sensor.set_hmirror(0)
lcd.init(freq=15000000)
fm.register(7, fm.fpioa.UART2_TX, force=True) #10为tx接rx
fm.register(6, fm.fpioa.UART2_RX, force=True) #11为rx接tx
fm.register(1, fm.fpioa.UART3_TX, force=True) #10为tx接rx
fm.register(0, fm.fpioa.UART3_RX, force=True) #11为rx接tx
io_led_red = 10
fm.register(io_led_red, fm.fpioa.GPIO0)
led_r=GPIO(GPIO.GPIO0, GPIO.OUT)
sensor.set_auto_gain(False)         #控制图像传感器是否自动调整增益来适应不同的光照条件


uart2 = UART(UART.UART2, 9600, 8, 0, 1)
uart3 = UART(UART.UART3, 9600, 8, 0, 1)

clock = time.clock()


def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob1=blob
            max_size = blob[2]*blob[3]
    return max_blob1




while(True):

    clock.tick()
    UART_Read(RSTM32,uart2)


    if K210.mode == 0x00:
        led_r.value(0)
        img=sensor.snapshot()
        sensor.set_pixformat(sensor.RGB565)

    elif K210.mode==0x09:

        if environment=="light":
               color_temp=color_template
        elif environment=="dark":
               color_temp=color_template_dark
        while 0  in target_flag:                    #不断捕捉直到标志全为1或超出次数上限
            find_target_time=find_target_time+1
            if (find_target_time>1000):
                print("颜色失败")
                break
            target_flag=[0,0,0]
            target_1=[[0,0],[0,0],[0,0]]
            target_2=[[0,0],[0,0],[0,0]]
            img=sensor.snapshot()
            for i in range(0,3):
                n=0
                blobs = img.find_blobs([color_temp[i]],merge=True,x_tride=10,y_tride=10,area_threshold=300,pixels_threshold=200,margin=1)
                if blobs:
                    for b in blobs:
                        if b.w()*b.h()>target_blob_area[0] and b.w()*b.h()<target_blob_area[1]:
                            if b.cy()>target_1[i][1]:#如果大于置为下层坐标
                                target_2[i][0]=target_1[i][0]
                                target_2[i][1]=target_1[i][1]
                                target_1[i][1]=b.cy()
                                target_1[i][0]=b.cx()
                            else:                    #否则置为上层坐标
                                target_2[i][1]=b.cy()
                                target_2[i][0]=b.cx()
                            tmp=img.draw_rectangle(b[0:4])
                            tmp=img.draw_cross(b[5], b[6])
                            n=n+1
                            if n>=2:                #一种颜色最多两个坐标
                                target_flag[i]=1    #对应物料捕捉标志位置1
                                break
            if 0 not  in target_flag:
                print("捕捉成功")
                temp=[target_1[0][0],target_1[1][0],target_1[2][0]]
                store_down=sort(temp,3,temp_sort_1)
                temp=[target_2[0][0],target_2[1][0],target_2[2][0]]
                store_up=sort(temp,3,temp_sort_2)

                print("上层物料摆放结果如下：")
                for c in store_up:
                    print(clor_name[c-1])
                print("下层物料摆放结果如下：")
                for c in store_down:
                    print(clor_name[c-1])




    elif K210.mode==0x01:

        img=sensor.snapshot()                        # 识别二维码
        res = img.find_qrcodes()
        if len(res) > 0:
            img.draw_string(2,2, res[0].payload(), color=(0,128,0), scale=2)
            img.draw_rectangle(res[0].x(), res[0].y(), res[0].w(), res[0].h(), color = (255, 0, 0), thickness = 2, fill = False)
            str_num = res[0].payload()
            str_num2 = str_num.replace('+','')
            int_num = int(str_num2)
            erweima1 = int_num//1000
            erweima2 = int_num%1000
            print(res[0].payload())
            TSTM32.x = erweima1
            TSTM32.y = erweima2
            uart2.write(TSTM32_data())
            print(erweima1)
            print(erweima2)
        t.sleep_ms(100)


    elif K210.mode==0x02:
        led_r.value(0)
        t.sleep_ms(100)
        img=sensor.snapshot()                        # 识别颜色
        if environment=="light":
                   color_temp=color_template
        elif environment=="dark":
                   color_temp=color_template_dark
        for i in range(0,3):
            blobs = img.find_blobs([color_temp[i]],roi=(79,69,240,171),x_tride=10,y_tride=10,area_threshold=2000,pixels_threshold=2000,merge=True,margin=1)
            if blobs:
                for b in blobs:
                    if b.w()*b.h()>target_blob_area[0] and b.w()*b.h()<target_blob_area[1]:
                        TSTM32.color = i+1
                        str_color = str(TSTM32.color)
                        img.draw_string(20,50, str_color, color=(0,128,0), scale=3)
                        uart2.write(TSTM32_data())
                        tmp=img.draw_rectangle(b[0:4])
                        tmp=img.draw_cross(b[5], b[6])


    elif K210.mode==0x05:
        t.sleep_ms(100)
        img=sensor.snapshot()                        # 识别颜色
        if environment=="light":
               color_temp=color_template
        elif environment=="dark":
               color_temp=color_template_dark

        blobs = img.find_blobs([color_temp[1]],merge=True,x_tride=10,y_tride=10,area_threshold=300,pixels_threshold=200,margin=1)
        if blobs:
            for b in blobs:
                max_blob = max(blobs, key=lambda b: b.pixels())
                if max_blob.w()*max_blob.h()>target_blob_area2[0] and max_blob.w()*max_blob.h()<target_blob_area2[1]:
                    TSTM32.x = max_blob.cx()
                    TSTM32.y = max_blob.cy()
                    uart2.write(TSTM32_data())

                    tmp=img.draw_rectangle(max_blob[0:4])
                    tmp=img.draw_cross(max_blob[5], max_blob[6])


    elif K210.mode==0x04:
        led_r.value(1)
        t.sleep_ms(10)
        sensor.set_pixformat(sensor.GRAYSCALE)
        img=sensor.snapshot()                        # 拍摄一张照片
        TSTM32.x = 0
        TSTM32.y = 0
        TSTM32.color = 0
        for c in img.find_circles(threshold = 4000, x_margin = 10, y_margin = 10, r_margin = 50,r_min = 30, r_max = 80, r_step = 2):
                img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0))
                TSTM32.x = c.x()
                TSTM32.y = c.y()
                uart2.write(TSTM32_data())


    elif K210.mode==0x03:
        t.sleep_ms(10)
        img=sensor.snapshot()
        blobs = img.find_blobs(thresholds,roi=(160,0,160,240),merge=True,x_tride=10,y_tride=10,area_threshold=300,pixels_threshold=200,margin=1)
        if blobs:
            for b in blobs:
                max_blob = max(blobs, key=lambda b: b.pixels())
                if max_blob.w()*max_blob.h()>target_blob_area2[0] and max_blob.w()*max_blob.h()<target_blob_area2[1]:
                    blobs_yellow2 = max_blob.pixels()
                    str_yellow2 = str(max_blob.pixels()/10)
                    img.draw_string(20,50, str_yellow2, color=(0,128,0), scale=3)
                    tmp=img.draw_rectangle(max_blob[0:4])
                    tmp=img.draw_cross(max_blob[5], max_blob[6])

        blobs = img.find_blobs(thresholds,roi=(0,0,160,240),merge=True,x_tride=10,y_tride=10,area_threshold=300,pixels_threshold=200,margin=1)
        if blobs:
            for b in blobs:
                max_blob = max(blobs, key=lambda b: b.pixels())
                if max_blob.w()*max_blob.h()>target_blob_area2[0] and max_blob.w()*max_blob.h()<target_blob_area2[1]:
                    blobs_yellow1 = max_blob.pixels()
                    str_yellow1 = str(max_blob.pixels()/10)
                    img.draw_string(20,30, str_yellow1, color=(0,128,0), scale=3)
                    tmp=img.draw_rectangle(max_blob[0:4])
                    tmp=img.draw_cross(max_blob[5], max_blob[6])
        err_yellow = (blobs_yellow2 - blobs_yellow1)/100
        TSTM32.x = int(err_yellow)
        uart2.write(TSTM32_data())
        str_err = str(err_yellow)
        img.draw_string(200,40, str_err, color=(0,128,0), scale=3)

    elif K210.mode==0x04:
        t.sleep_ms(10)
        img=sensor.snapshot()
        for l in img.find_lines(threshold = 1000, theta_margin = 25, rho_margin = 25):
            if (min_degree <= l.theta()) and (l.theta() <= max_degree):
                img.draw_line(l.line(), color = (255, 0, 0))


    else:
        img=sensor.snapshot()                        # 拍摄一张照片




    fps = clock.fps()
    str_fps = str(fps)
    img.draw_string(20,200, str_fps, color=(0,128,0), scale=2)
    str_mode = str(K210.mode)
    img.draw_string(40,100, str_mode, color=(0,128,0), scale=4)


    str_x = str(TSTM32.x)
    str_y = str(TSTM32.y)
    img.draw_string(250,100, str_x, color=(0,128,0), scale=4)
    img.draw_string(250,150, str_y, color=(0,128,0), scale=4)

    #print(K210.mode)
    lcd.display(img)
    TSTM32.x = 0
    TSTM32.y = 0
    TSTM32.color = 0










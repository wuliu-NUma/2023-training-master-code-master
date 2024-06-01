# Untitled - By: 10416 - 周二 7月 25 2023

import sensor, image, time

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()

x=0
y=0
i=0
#颜色识别阈值
red_threshold=(23, 77, 18, 101, -16, 96)        #亮处红色阈值(25, 41, 14, 44, -12, 33)  (19, 56, 4, 76, 4, 127  (35, 80, -36, 44, 27, 5) (35, 63, 53, 7, 1, 32)
red_threshold_dark=(0, 21, 15, 40, -4, 28)    #暗处红色阈值(18, 26, 2, 41, 16, 33)
green_threshold =  (9, 87, -67, -11, -2, 66)    #亮处绿色阈值(32, 65, -49, -19, 30, -36)(23, 66, -59, -19, 13, 49)(52, 84, -38, -8, -2, 46) (52, 84, -36, -4, -1, 17) (43, 81, -38, -20, -12, 25)
green_threshold_dark=(3, 7, -51, -6, -20, 24)    #暗处绿色阈值(20, 41, -33, -18, -12, 33) (38, 66, -46, 1, 39, 7)
blue_threshold=(31, 78, -16, 16, -39, -8)   #亮处蓝色阈值(11, 63, -5, 36, -52, -35)  (39, 58, -4, 15, -53, -35)(44, 65, -26, 23, -52, -1)(19, 49, 0, 123, -119, -8)
blue_threshold_dark=(0, 23, 1, 10, -30, -5)    #暗处蓝色阈值(8, 15, -3, 14, -23, -8)
color_template=[red_threshold,green_threshold,blue_threshold]
color_template_dark=[red_threshold_dark,green_threshold_dark,blue_threshold_dark]
clor_name=["R","G","B"]
environment="light"                  #更改此参数为"dark"更改为暗光条件下的阈值（该阈值需手动调整）
target_blob_area=[500,8000]           #物料捕捉面积最大值与最小值限制，防止捕捉到错误目标
target_proportion = (1.1,2.5)        #色块长宽W/H比例限制 ，防止捕捉到错误目标
target_1=[[0,0],[0,0],[0,0]]        #下层物料坐标     按红绿蓝排列
target_2=[[0,0],[0,0],[0,0]]        #上层物料坐标
temp_sort_1=[1,2,3]
temp_sort_2=[1,2,3]
target_flag=[0,0,0]
store_up=[1,2,3]                                #原料区上层从左到右的摆放情况 1=R 2=G 3=B
store_down=[1,2,3]
find_target_time=0


while(True):
    clock.tick()
    img = sensor.snapshot()
    print(clock.fps())

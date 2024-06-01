 #  2021-03-29
import sensor
import image
import lcd
import time
import math
#V1.2
#与原版相比变成一次性可以扫描六个物块的颜色并进行位置获取
#颜色识别不到可尝试将67行左右的find_target_time>100的100加大，即增加尝试次数，或修改target_blob_area与target_proportion值
#2022-04-03
# openMV下 无sensor.run(1)语句，因此先将该语句注释掉，对于K210下，多数情况在设置完sensor.set_pixformat(sensor.RGB565)
# sensor.set_framesize(sensor.QVGA)后也可以不使用该语句
# 注释掉白平衡，因为发现有些摄像头该设置下会出现黑屏情况,并将亮度设置由0该为3，去掉skip_frames的错误注释
# 若发现排序相反即从左到右的顺序变为从右至左则尝试注释sensor.set_vflip(True)或sort函数中的a[i]<a[i+1]改为a[i]>a[i+1]
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(20)
sensor.set_brightness(3)            #亮度
#sensor.set_auto_whitebal(False,(-6.02073, -5.243186, -1.972561))
#lcd 初始化
sensor.set_vflip(True)
sensor.run(1)
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
target_proportion = (1.1,1)        #色块长宽W/H比例限制 ，防止捕捉到错误目标
target_1=[[0,0],[0,0],[0,0]]        #下层物料坐标     按红绿蓝排列
target_2=[[0,0],[0,0],[0,0]]        #上层物料坐标
temp_sort_1=[1,2,3]
temp_sort_2=[1,2,3]
target_flag=[0,0,0]
store_up=[1,2,3]                                #原料区上层从左到右的摆放情况 1=R 2=G 3=B
store_down=[1,2,3]
find_target_time=0
#从大到小排序 输出排序下标
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
while 1:
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
                    if b.w()*b.h()>target_blob_area[0] and b.w()*b.h()<target_blob_area[1] and b.h()/b.w()>target_proportion [0] and b.h()/b.w()<target_proportion [1]:
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








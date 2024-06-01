from machine import I2C

import sensor, image, time,json

import json

#1:red 2:green 3:blue


threshold_index = 0
def compareBlob(blob1,blob2):#比较色块后返回最大值
    tmp = blob1.pixels() - blob2.pixels()
    if tmp == 0:
        return 0;
    elif tmp > 0:
        return 1;
    else:
        return -1;

#红绿蓝色环阈值

objthresholds=[((43, 26, -3, 10, 12, -5)),

            (20, 100, -128, -10, -7, 84),

            (49, 61, -12, 18, -27, -3)]
graythreshold=[(100,255)]#二值化阈值

sensor.reset()

sensor.set_pixformat(sensor.RGB565)

sensor.set_framesize(sensor.QVGA)

sensor.skip_frames(time = 1000)
sensor.set_vflip(1)                                         # 打开垂直翻转 如果是 01Studio 的 K210 不开启会导致画面方向与运动方向相反
sensor.set_hmirror(0)
sensor.set_auto_gain(False) # must be turned off for color tracking

sensor.set_auto_whitebal(False) # must be turned off for color tracking

clock = time.clock()
while(True):
    img = sensor.snapshot()
    img.binary([objthresholds[1]])
    img.dilate(2)

    blobs=img.find_blobs(graythreshold,pixels_threshold=2525,area_threashold=1600,merge=True)
    if blobs:


        bigBlob=blobs[0]
        #print('1')
        for blob in blobs:
            if compareBlob(bigBlob,blob) == -1:
                bigBlob=blob
            img.draw_rectangle(bigBlob.rect())
            #print(bigBlob.cx(),bigBlob.cy())
            #output_str="[%d,%d]" % (bigBlob.cx(),bigBlob.cy())
            #output_str=json.dumps([judge(bigBlob.cx()),judge(bigBlob.cy()),bigBlob.cx(),bigBlob.cy()])
            #uart.write(output_str+'\r\n')
    #lcd.display(img)


            #print('you send:',bigBlob.cx()-120)
        #uart.write(data_out)
end

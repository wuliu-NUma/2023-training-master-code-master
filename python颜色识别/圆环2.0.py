import sensor, image, time, lcd, math
obj1=[(255, 144)]
nima=2
banjin=0

#设置核函数滤波，核内每个数值值域为[-128,127],核需为列表或元组
kernel_size = 1
kernel = [-1, -1, -1,\
          -1, +8, -1,\
          -1, -1, -1]
# 这个一个高通滤波器。见这里有更多的kernel http://www.fmwconcepts.com/imagemagick/digital_image_filtering.pdf
thresholds = [(255, 144)] # grayscale thresholds设置阈值

def compareBlob(blob1,blob2):#比较色块后返回最大值
    tmp = blob1.pixels() - blob2.pixels()
    if tmp == 0:
        return 0;
    elif tmp > 0:
        return 1;
    else:
        return -1;

sensor.reset() # 初始化 sensor.
lcd.init(freq=15000000)
sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.RGB565 设置图像色彩格式，有RGB565色彩图和GRAYSCALE灰度图两种
sensor.set_framesize(sensor.QVGA) # or sensor.QVGA (or others)
sensor.skip_frames(10) # 让新的设置生效
clock = time.clock() # 跟踪FPS帧率


while(True):
    clock.tick() # 追踪两个snapshots()之间经过的毫秒数.
    img = sensor.snapshot() # 拍一张照片，返回图像

    img.morph(kernel_size, kernel)
    #morph(size, kernel, mul=Auto, add=0)，morph变换，mul根据图像对比度
    #进行调整，mul使图像每个像素乘mul；add根据明暗度调整，使得每个像素值加上add值。
    #如果不设置则不对morph变换后的图像进行处理。
    img.binary(thresholds)
    #利用binary函数对图像进行分割

    # Erode pixels with less than 2 neighbors using a 3x3 image kernel
    # 腐蚀像素小于2邻居使用3x3图像内核
    img.erode(1, threshold = 1)

    blobs = img.find_blobs(obj1,merge=True,x_tride=10,y_tride=10,area_threshold=300,pixels_threshold=200,margin=1)
    if blobs:
            bigBlob=blobs[0]
            #print('1')
            for blob in blobs:
                if compareBlob(bigBlob,blob) == -1:
                    bigBlob=blob
                banjin = bigBlob.w()/nima
                banjini = int(banjin)
                img.draw_circle(bigBlob.cx(), bigBlob.cy(), banjini, color = (255, 255, 0) ,thickness = 10)
                img.draw_rectangle(bigBlob.rect())
                print(bigBlob)
    #侵蚀函数erode(size, threshold=Auto)，去除边缘相邻处多余的点。threshold
    #用来设置去除相邻点的个数，threshold数值越大，被侵蚀掉的边缘点越多，边缘旁边
    #白色杂点少；数值越小，被侵蚀掉的边缘点越少，边缘旁边的白色杂点越多。
    print(clock.fps()) # 注意: 当连接电脑后，OpenMV会变成一半的速度。当不连接电脑，帧率会增加。


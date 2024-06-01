# image_V1.0 - By: FITQY - 周一 8 月 22 日 2022
#__________________________________________________________________
# 导入模块
import sensor, time, image                                  # 导入感光元件模块 sensor 跟踪运行时间模块 time 机器视觉模块 image

#__________________________________________________________________
# 感光元件设置
sensor.reset()                                              # 重置并初始化感光元件 默认设置为 摄像头频率 24M 不开启双缓冲模式
#sensor.reset(freq=24000000, dual_buff=True)                # 设置摄像头频率 24M 开启双缓冲模式 会提高帧率 但内存占用增加

sensor.set_pixformat(sensor.RGB565)                         # 设置图像格式为 RGB565 (彩色) 除此之外 还可设置格式为 GRAYSCALE 或者 YUV422
sensor.set_framesize(sensor.QVGA)                           # 设置图像大小为 QVGA (320 x 240) 像素个数 76800 K210最大支持格式为 VGA

sensor.set_auto_exposure(1)                                 # 设置自动曝光
#sensor.set_auto_exposure(0, exposure=120000)               # 设置手动曝光 曝光时间 120000 us

sensor.set_auto_gain(0, gain_db = 5)                       # 设置画面增益 17 dB 影响实时画面亮度
sensor.set_auto_whitebal(1, rgb_gain_db = (30,0,0))          # 设置RGB增益 0 0 0 dB 影响画面色彩呈现效果 在 K210 上无法调节增益 初步判定是感光元件 ov2640 无法支持

#sensor.set_contrast(0)                                     # 设置对比度 0 这个参数无法读取 且调这个参数对画面似乎不会产生影响 暂时注释
#sensor.set_brightness(0)                                   # 设置亮度 0 这个参数无法读取 且调这个参数对画面似乎不会产生影响 暂时注释
#sensor.set_saturation(0)                                   # 设置饱和度 0 这个参数无法读取 且调这个参数对画面似乎不会产生影响 暂时注释

sensor.set_vflip(0)                                         # 打开垂直翻转 如果是 01Studio 的 K210 不开启会导致画面方向与运动方向相反
sensor.set_hmirror(1)                                       # 打开水平镜像 如果是 01Studio 的 K210 不开启会导致画面方向与运动方向相反

sensor.skip_frames(time = 2000)                             # 延时跳过2s 等待感光元件稳定

#__________________________________________________________________
# 创建时钟对象
clock = time.clock()                                        # 创建时钟对象 clock

#__________________________________________________________________
# 寻找色块
# 定义类
class color_property():
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
black = color_property()
black.color_threshold         = (0, 50, -10, 10, -10, 10)
black.color_roi               = (0,0,320,240)
black.color_x_stride          =  1
black.color_y_stride          =  1
black.color_pixels_threshold  =  100
black.color_area_threshold    =  100
black.color_merge             =  True
black.color_margin            =  1

# 红色
red   = color_property()
red.color_threshold           = (43, 26, -3, 10, 12, -5)

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
green = color_property()

# 蓝色 预留
blue  = color_property()

# 定义寻找色块函数
def opv_find_blobs(color,led_flag):
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

# 定义打印色块参数函数
def print_blobs_property(color,name):
    print(name,"cx:",color.cx,"cy:",color.cy,"flag:",color.flag,"color:",color.color,"density:",color.density,"led_flag:",color.led_flag)

#__________________________________________________________________
# 打印sensor参数
def print_sensor():
    print("Exposure: "+str(sensor.get_exposure_us()))       # 打印 曝光时间
    print("Gain: "+str(sensor.get_gain_db()))               # 打印 画面增益
    print("RGB: "+str(sensor.get_rgb_gain_db()))            # 打印 RGB 增益

#__________________________________________________________________
# 调试区
mycnt       = 0                                             # 计数变量
mycnt_max   = 30                                            # 计数上限 此值越大 计数周期越长

#__________________________________________________________________
# 主函数
while(True):

    clock.tick()                                            # 跟踪运行时间

    img = sensor.snapshot()                                 # 拍摄一张照片
    opv_find_blobs(black,1)                                 # 找黑色色块 led标志为1 表示黑色
    opv_find_blobs(red,2)                                   # 找红色色块 led标志为2 表示红色

    if mycnt == 0:                                          # 如果 mycnt 等于 0 此步骤的目的是控制打印周期 不要打印的太快
        mycnt = 1                                           # 将 1 赋值给 mycnt 使下一次不再满足 mycnt == 0 进入 elif
        print_sensor()                                      # 打印sensor参数
        print_blobs_property(black,"Black-")                # 打印黑色色块参数
        print_blobs_property(red,  "Red-  ")                # 打印红色色块参数

    elif mycnt < mycnt_max:                                 # 计数变量 小于 计数上限 则 计数变量 自增
        mycnt = mycnt + 1

    else:                                                   # 计数变量 超出 计数上限 则 将0赋值给 mycnt 使下一次进入 if
        mycnt = 0

#__________________________________________________________________



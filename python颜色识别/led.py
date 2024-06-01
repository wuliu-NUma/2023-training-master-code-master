# Untitled - By: 10416 - 周三 8月 23 2023

import sensor, image, time
from fpioa_manager import fm
from Maix import GPIO


sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()

io_led_red = 10
fm.register(io_led_red, fm.fpioa.GPIO0)
led_r=GPIO(GPIO.GPIO0, GPIO.OUT)

while(True):



    led_r.value(1)


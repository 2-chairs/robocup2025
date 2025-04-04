# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Single Color RGB565 Blob Tracking Example
#
# This example shows off single color RGB565 tracking using the OpenMV Cam.

import sensor
import time
import math
import struct

threshold_index = 0  # 0 for red, 1 for green, 2 for blue

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green/blue things. You may wish to tune them...
thresholds = [
    (0, 100, 127, 33, 127, 25)
]  # generic_blue_thresholds

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
clock = time.clock()

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" merges all overlapping blobs in the image.

while True:
    clock.tick()
    img = sensor.snapshot()
    for blob in img.find_blobs(
        [thresholds[threshold_index]],
        pixels_threshold=200,
        area_threshold=200,
        merge=True,
    ):
        # These values depend on the blob not being circular - otherwise they will be shaky.
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints(
            [(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20
        )




    # Initialize serial port (adjust port and baudrate as needed)
    try:
        ser = serial.Serial("/dev/ttyACM0", 115200) #Replace with your port
        print("Serial port connected.")
    except serial.SerialException as e:
        print("Serial port error:", e)
        ser = None #set ser to none if there is an error.

    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames(time = 2000)
    sensor.set_auto_gain(False)
    sensor.set_auto_whitebal(False)

    # Define ball color thresholds (adjust as needed)
    ball_threshold = (30, 100, 15, 127, 15, 127) # Example: Adjust based on your ball's color

    while(True):
        img = sensor.snapshot()
        blobs = img.find_blobs([ball_threshold])
        if blobs:
            for blob in blobs:
                x = blob.cx()
                y = blob.cy()
                area = blob.area()
                print("Ball: X={}, Y={}, Area={}".format(x, y, area)) #print to the openMV IDE serial terminal.
                if ser: #Only try to send if the serial port was successfully opened.
                    try:
                        data = struct.pack("<fff", x, y, area) #pack the data into a byte array.
                        ser.write(data) #send the data over serial.
                    except serial.SerialException as e:
                        print("Serial send error:", e)

        time.sleep_ms(10) #small delay.

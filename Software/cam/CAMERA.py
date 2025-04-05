import sensor, time, math, machine
import array

# Camera setup
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)       # Already in your code, but ensure it's present
sensor.set_auto_whitebal(False)   # Already in your codesensor.set_auto_exposure(False, exposure_us=30000)  # 30 ms exposure
sensor.set_auto_exposure(False, exposure_us=60000)  # 30 ms exposure


# LAB color thresholds
red_thresholds = [(100, 0, 15, 127, 127, -128)]
blue_thresholds = [(100, 0, 4, -128, -40, -10)]
yellow_thresholds = [(100, 7, -103, 127, 127, 19)]

# Image and mirror center setup
x_mirror_center = 177
y_mirror_center = 124
image_width = 320
image_height = 240
image_center_x = image_width / 2
image_center_y = image_height / 2

# Normalize and reflect coordinates
def reflect_coords(x_cam, y_cam, image_center_x, image_center_y, image_width, image_height):
    x_rel = x_cam - image_center_x
    y_rel = y_cam - image_center_y
    x_reflected = x_rel
    y_reflected = y_rel
    x_normalized = int(((x_reflected + image_center_x) / image_width) * 255)
    y_normalized = int(((y_reflected + image_center_y) / image_height) * 255)
    x_normalized = max(0, min(255, x_normalized))
    y_normalized = max(0, min(255, y_normalized))
    y_inverted = 255 - y_normalized
    return x_normalized, y_inverted

# Calculate angle and distance
def calculate_angle(x, y, cx, cy):
    dx = x - cx
    dy = y - cy
    angle_rad = math.atan2(dx, dy)
    return math.degrees(angle_rad) % 360

def calculate_distance(x, y, cx, cy):
    dx = x - cx
    dy = y - cy
    return math.sqrt(dx**2 + dy**2)

# UART setup
uart_obj = machine.UART(1, 115200)

# For tracking red blob motion
x_values = array.array('i', [])
time_values = array.array('i', [])
start_time = time.ticks_ms()

while True:
    img = sensor.snapshot()
    output_list = []

    # Color detection config: (Name, Threshold, Draw Color)
    color_configs = [
        ("RED", red_thresholds, (255, 0, 0)),
        ("BLUE", blue_thresholds, (0, 0, 255)),
        ("YELLOW", yellow_thresholds, (255, 255, 0))
    ]

    for color_name, thresholds, draw_color in color_configs:
        blobs = img.find_blobs(thresholds)
        if blobs:
            largest_blob = max(blobs, key=lambda b: b.area())
            x_cam = largest_blob.cx()
            y_cam = largest_blob.cy()
            img.draw_rectangle(largest_blob.rect(), color=draw_color)

            x_norm, y_inv = reflect_coords(x_cam, y_cam, image_center_x, image_center_y, image_width, image_height)
            y_adj = int((127 - y_inv) * 255 / 127)
            angle = calculate_angle(x_cam, y_cam, x_mirror_center, y_mirror_center)
            distance = calculate_distance(x_cam, y_cam, x_mirror_center, y_mirror_center)

            output_list.extend([
                str(x_norm),
                str(y_adj),
                "{:.2f}".format(angle),
                "{:.2f}".format(distance)
            ])

            if color_name == "RED":
                current_time = time.ticks_ms() - start_time
                x_values.append(x_norm)
                time_values.append(current_time)
        else:
            # If blob not found, append "none" for all four values
            output_list.extend(["none", "none", "none", "none"])

    # Send UART line as a space-separated flat list
    uart_line = " ".join(output_list) + "\n"
    uart_obj.write(uart_line.encode('utf-8'))
    print(uart_line.strip())

    time.sleep_ms(0)

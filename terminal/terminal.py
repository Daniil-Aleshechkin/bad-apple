import time
from PIL import Image
import serial
import sys

def get_image_path_for_frame(frame: int):
    return f"./bad-apple/bad_apple_{frame:03}.png"

port = sys.argv[1]
baudrate = 1500000
ser = serial.Serial(port, baudrate, timeout=0.05)

CUT_OFF_LEVEL = 124

BYTE_LEVELS = [128, 64, 32, 16, 8, 4, 2, 1]

def get_byte_string_for_frame(frame: int):
    byte_string = []

    # Open the image file
    image_path = get_image_path_for_frame(frame)
    image = Image.open(image_path)

    # Resize the image
    image = image.crop((180, 0, image.width - 180, image.height))
    image = image.resize((32, 32))
    image = image.convert('L')

    for i in range(image.size[1]):  # for every row
        for j in range(0, image.size[0], 8):  # for every column
            byte = 0x0
            
            for byte_index in range(8):
                if (image.getpixel((j+byte_index, i)) > CUT_OFF_LEVEL):
                    byte += BYTE_LEVELS[byte_index]
            
            byte_string.append(byte)

    return byte_string

# Print the ASCII art
# for byte in get_byte_string_for_frame(538):
#     print("SENDING: ", byte)
    

#     byte_ack = ser.read()
#     print(byte_ack)
total_frames = 6562
frames = 6
byte_string = get_byte_string_for_frame(538)
sucessfullySent = False

print(total_frames // frames)

for x in range(total_frames // frames):
    sucessfullySent = False
    while(not sucessfullySent):
        for i in range(frames):
            ser.write(byte_string)
        expected =  frames*128
        actual = len([int(x) for x in ser.read(frames*128)])
        
        print("EXPECTED BYTES: ", expected)
        print("ACTUAL: ", actual)

        sucessfullySent = expected == actual

    time.sleep(0.1)
ser.close()
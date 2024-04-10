import time
from PIL import Image
import serial
import sys
import pygame
# Initialize the mixer module
pygame.mixer.init()
# Load the sound file
sound = pygame.mixer.Sound('./bad_apple.wav')
# Play the sound

def get_image_path_for_frame(frame: int):
    return f"./bad-apple/bad_apple_{frame:03}.png"

port = sys.argv[1]
baudrate = 1500000
ser = serial.Serial(port, baudrate, timeout=0.6)

CUT_OFF_LEVEL = 124

BYTE_LEVELS = [1, 2, 4, 8, 16, 32, 64, 128]

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
frames = 1

sucessfullySent = False

starting_frame = 9

print(total_frames // frames)

for x in range(1,starting_frame):
    byte_string = get_byte_string_for_frame(x)
    sucessfullySent = False
    while(not sucessfullySent):
        ser.write(b'\x0e')
        response = ser.read()

        print(response)

        if (response == b'\x51'):
            ser.write(byte_string)
            expected = len(byte_string)
            actual = ser.read(128)
            print(actual)
            sucessfullySent = expected == len(actual)

sound.play()
for x in range(starting_frame,total_frames // frames):
    byte_string = get_byte_string_for_frame(x)
    sucessfullySent = False
    while(not sucessfullySent):
        ser.write(b'\x0e')
        response = ser.read()

        print(response)

        if (response == b'\x51'):
            ser.write(byte_string)
            expected = len(byte_string)
            actual = ser.read(128)
            print(actual)
            sucessfullySent = expected == len(actual)

    #time.sleep(0.1)
ser.close()
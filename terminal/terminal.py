from PIL import Image
import numpy as np

# Open the image file
image_path = './bad-apple/bad_apple_1631.png'
image = Image.open(image_path)

# Resize the image
image = image.crop((180, 0, image.width - 180, image.height))
image = image.resize((32, 32))
image = image.convert('L')

# Define ASCII characters for different brightness levels
black_char = "."
white_char = "@"

# Convert image pixels to ASCII
ascii_str = ''
for i in range(image.size[1]):  # for every row
    for j in range(image.size[0]):  # for every column
        # Scale pixel value to 0-9
        ascii_str += white_char if image.getpixel((j,i)) > 124 else black_char
    ascii_str += '\n'

# Print the ASCII art
print(ascii_str)
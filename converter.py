#!/usr/local/bin/python3


import sys
import cv2
import numpy as np
import os

filename = sys.argv[1]
print(filename)
image = cv2.imread(filename)

down_width = 122
down_height = 250
down_points = (down_width, down_height)

image = cv2.rotate(image, cv2.ROTATE_90_CLOCKWISE)
image = cv2.flip(image, 0)

resized_down = cv2.resize(image, down_points, interpolation=cv2.INTER_LINEAR)

gray = cv2.cvtColor(resized_down, cv2.COLOR_BGR2GRAY)

ret,thresh = cv2.threshold(gray,127,255,cv2.THRESH_BINARY)

#cv2.imshow("binary", thresh)
#cv2.waitKey (0)

cv2.imwrite('out.bmp', thresh)

from PIL import Image
im = Image.open(f"out.bmp")
im = im.convert('RGBA')
data = im.getdata()
im.putdata(data)
im = im.convert('1')
filename_out = filename + "_converted.bmp"
im.save(filename_out)
os.remove("out.bmp")




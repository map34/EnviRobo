# For better accuracy, use binary images. So before finding contours, apply threshold or canny edge detection

# cv2.findContours() will modify the source image, if still want the original image, need to save it before doing operations
# Finding contours must find an object in white and background in black

# Find contours of a binary image:
import numpy as np
import cv2

im = cv2.imread('shapes.png')
imgray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)
ret,thresh = cv2.threshold(imgray,127,255,0)
# 1st arg: source image
# 2nd arg: contour retrieval mode
# 3rd arg: contour approximation method: cv2.CHAIN_APPROX_SIMPLE saves memory by removing all redundant points and compresses the contour
image, contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

# cv2.drawContours() 
cnt = contours[4]
img = cv2.drawContours(img, [cnt], 0, (0,255,0), 3)

# Mask and Pixel Points 
# This uses Numpy functions, gives coordinates in (row, column) format
mask = np.zeros(imgray.shape,np.uint8) 
cv2.drawContours(mask,[cnt],0,255,-1)
pixelpoints = np.transpose(np.nonzero(mask))
# This uses OpenCV function, gives coordinates in (x, y) format, basically interchanged the Numpy format
#pixelpoints = cv2.findNonZero(mask)
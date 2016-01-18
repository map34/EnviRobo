import cv2
import numpy as np

img = cv2.imread('shapes.png',0)

# MedianBlur computes the median of all the pixels under the kernel window and the central pixel is replaced with this median value
# This reduces the noise effectively, 5 indicates adding a 50% noise 
img = cv2.medianBlur(img, 5)

# cv2.cvtColor(input_image, flag) where flag is determines the type of conversion
cimg = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

# param1: the higher threshold of the two passed to the Canny() edge detector
# param2: the accumulator threshold for the circle centers at the detection state. The smaller it is, more false circles may be detected
circles = cv2.HoughCircles(img, cv2.HOUGH_GRADIENT, 1, 20, param1 = 50, param2 = 30, minRadius = 0, maxRadius = 0)

circles = np.uint16(np.around(circles))
for i in circles[0,:]:
	# Draw the outer circle
	# cv2.circle(input_image, center, radius, color, thickness, lineType, shift)
	cv2.circle(cimg, (i[0], i[1]), i[2], (0,255,0), 2)
	
	# Draw the center of the circle 
	cv2.circle(cimg, (i[0], i[1]), 2, (0, 0, 255), 3)

cv2.imshow('detected circles', cimg)
cv2.waitKey(0)
cv2.destroyAllWindows()

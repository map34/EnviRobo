#name it detect_color.py
#run the script python detect_color.py --image pokemon_games.png
# import the necessary packages
import numpy as np
import argparse
import cv2

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", help = "path to the image")
args = vars(ap.parse_args())

# load the image
image = cv2.imread(args["image"])

# define the list of boundaries which is the list of colors in RGB 
boundaries = [
	([17, 15, 100], [50, 56, 200]),
	([86, 31, 4], [220, 88, 50]),
	([25, 146, 190], [62, 174, 250]),
	([103, 86, 65], [145, 133, 128])
	#([0, 0, 0], [15, 15, 15])
]

# loop over the boundaries for performing the actual color detection
for (lower, upper) in boundaries:
    # create NumPy arrays from the boundaries
    lower = np.array(lower, dtype = "uint8")
    upper = np.array(upper, dtype = "uint8")
    # find the colors within the specified boundaries and apply
    # the mask
    mask = cv2.inRange(image, lower, upper) #image to perform color detection, then lower limit of the color we want to detect, a binary mask is returned, where white pixels (255) represent pixels that fall into the upper and lower limit range and black pixels (0) do not. 
    output = cv2.bitwise_and(image, image, mask = mask) #showing only pixels in the image that have a corresponding white value in the mask
    # show the images/ output
    cv2.imshow("images", np.hstack([image, output]))
    cv2.waitKey(0)

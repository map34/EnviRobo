# This code works but only for a certain amount of time, it is a live stream video with a preview
import io
import time
import picamera
import picamera.array
import cv2
import numpy as np

# Create the in-memory stream
stream = io.BytesIO()

# "with" will call close() implicitly
with picamera.PiCamera() as camera:
    camera.start_preview()                        #display a preview showing the camera feed on screen
    time.sleep(2)
    camera.capture(stream, format='jpeg')         #take a picture

# Construct a numpy array from the stream
data = np.fromstring(stream.getvalue(), dtype=np.uint8)

# "Decode" the image from the array, preserving color
# cv2.imdecode(input array, flag specifying the color type of the loaded image 1 for a 3-channel color image 0 for grayscale)
image = cv2.imdecode(data, 1)

# OpenCV returns an array with data in BGR order. If you want RGB use
# image = image[:, :, ::-1]

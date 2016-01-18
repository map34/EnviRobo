# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

camera = PiCamera()                              # initialize the camera
camera.resolution = (640, 480)			 # set camera resolution
camera.framerate = 32				 # set framerate: frames/second
rawCapture = PiRGBArray(camera, size=(640, 480)) # initialize PiRGBArray ojbect
camera.vflip = True
camera.hflip = True

time.sleep(0.1)					 # allow the camera to warmup
 
# capture frames from the camera
# The simplest way to capture long time-lapse sequences is with capture.continuous
# The camera captures images continually until you tell it to stop
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):    # grab the raw NumPy array 
	image = frame.array								  # representing the image, then 
 											  # initialize the timestamp
											  # and occupied/unoccupied text
	cv2.imshow("Frame", image)		 # show the frame on screen        
	key = cv2.waitKey(1) & 0xFF
 
	rawCapture.truncate(0)			 # must clear the stream in preparation for the next frame
 
	if key == ord("q"):			 # if the `q` key was pressed, break from the loop
		break

cv2.destroyAllWindows() # Added
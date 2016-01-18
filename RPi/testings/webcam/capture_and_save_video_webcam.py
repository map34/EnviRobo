import numpy as np
import cv2

# Creatibg a VideoCapture object, argument can be either the device index or the name of a video file
# argument = 0 in this case is the device index to specify which camera
cap = cv2.VideoCapture(0)

while(True):
	# Capture frame-by-frame
	# cap.read returns a bool, if frame is read correctly, it will be true. 
	ret, frame = cap.read()
	
	# Our operations on the frame come here where gray is the name of the video after operation
	# In this case we are converting the color into gray
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	
	# Display the resulting frame
	cv2.imshow('frame', gray)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()

# To save a video, need to create a VideoWriter object. Specify the output file name (eg: output.avi)
# Then specify the FourCC. It is a 4-byte code used to specify the video codec. It is platform dependent (use x264)
# Then specify the number of frames per second (fps) and frame size 
# Lastly, the isColor flag, if true, encoder expects color frame. 
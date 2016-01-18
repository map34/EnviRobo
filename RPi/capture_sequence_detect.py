# Based on the sample code from advanced recipes on picamera
# Capable of capturing a sequence of images extremely rapidly by utilizing 
# its video capture capabilities with a JPEG encoder and analyze each individual shot
# This module is combined with processing a frame/image 
# And can generate many images with face detection and print "found your face"
# Cleaned

import time
import picamera
import io
import cv2
import numpy as np

def outputs():
	stream = io.BytesIO()
	name = 0
	for i in range(10):
	# This returns the stream for the camera to capture to
		yield stream
		
		# Once the capture is complete, the loop continues here
		# Do some processing on the image, rewind the stream to the
		# beginning so we can read its content
		
		# construct a numpy array from the stream
		data = np.fromstring(stream.getvalue(), dtype=np.uint8)
		
		# Reset the stream for the next capture
		stream.seek(0)      # Change the stream position to the given byte offset
		stream.truncate()   # Resize the stream to the current position
		
		# "decode" the image from the array, preserving colour in BGR format
		image = cv2.imdecode(data, 1)
		
		#Load a cascade file for detecting faces
		face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_frontalface_alt.xml')
		#face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/banana.xml')
		
		# get a gray picture
		gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
			
		#Look for faces in the image using the loaded cascade file
		faces = face_cascade.detectMultiScale(gray, 1.1, 5)	
		
		print "Found "+str(len(faces))+" face(s)"
	
		#Draw a rectangle around every found face
		for (x,y,w,h) in faces:
	    		cv2.rectangle(image,(x,y),(x+w,y+h),(255,255,0),2)
	    		cv2.putText(image, "Found your face", (5, 220), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255)) 		
			coord = "x=%d, y=%d" % (x, y)
			cv2.putText(image, coord, (x, y+h+15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0))
		filename = "result%02d.jpg" % name  		#increment a number for filename
		filename = "result{0:02d}.jpg".format(name) 	#cast int to string
		
		#Save the result image
		cv2.imwrite(filename,image) 
		name += 1

with picamera.PiCamera() as camera:
    camera.resolution = (320, 240)
    camera.framerate = 10

    camera.start_preview()
    # Give the camera some warm-up time
    time.sleep(2)
    
    # capture_sequence accepts a sequence or iterator of outputs each of which must either be a string specifying a filename for output.
    # The use_video_port parameter controls whether the camera's image or video port is used to capture images. Default: false --> camera. 
    # When ture, the splitter_port parameter specifies the port of the video splitter that the image encoder will be attached to. 

    camera.capture_sequence(outputs(), 'jpeg', use_video_port=True)
    
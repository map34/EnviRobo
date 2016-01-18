# Based on the sample code from advanced recipes on picamera
# Capable of capturing a sequence of images extremely rapidly by utilizing 
# its video capture capabilities with a JPEG encoder and analyze each individual shot
# This module is combined with processing a frame/image 
# And can generate many images with face detection and print "found your face"
# Assuming one object a frame now 
# Cleaned

import time
import picamera
import io
import cv2
import numpy as np

def outputs():
	stream = io.BytesIO()
	name = 0
	# Each iteration generates one image
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
		#face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_frontalface_alt.xml')
		#face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_russian_plate_number.xml')
		#face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/bottle.xml')
		face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/clock.xml')
		
		# get a gray picture
		gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
			
		#Look for faces in the image using the loaded cascade file
		faces = face_cascade.detectMultiScale(gray, 1.1, 5)	
		
		print "Found "+str(len(faces))+" face(s)"
	
		#Draw a rectangle around every found face
		left = 0
		right = 0
		width = 0
		for (x,y,w,h) in faces:
	    		cv2.rectangle(image,(x,y),(x+w,y+h),(255,255,0),2)
	    		cv2.putText(image, "Found your face", (5, 220), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255)) 		
			coord = "x=%d, y=%d" % (x, y)
			cv2.putText(image, coord, (x, y+h+15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 128, 0))
			left = x
			right = 320 - x - w
			width = w
			# Assuming only one object will be found in each image, check if the object is centered enough
			# Left/right max: (horizontal window size - (rectangle width)) / 2
			#leftmax = (320-width)/2
			#rightmax = (320-width)/2
			diff = left-right
			if diff < 20 and diff >= 0:
				# send signal to move forward
				# can break from taking pictures? 
				cv2.putText(image, "within range", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
			#left>right, so move more left
			elif (diff) >= 20: 
				# move left by ???
				cv2.putText(image, "too left", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
			# diff < 0, right > left, so move more right
			elif (diff) < 0:   
				# move right by ???
				cv2.putText(image, "too right", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
			else:
				cv2.putText(image, "nonono", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
		
		filename = "result%02d.jpg" % name  		#increment a number for filename
		filename = "result{0:02d}.jpg".format(name) 	#cast int to string
		
		#Save the result image
		cv2.imwrite(filename,image) 
		name += 1

with picamera.PiCamera() as camera:
    camera.resolution = (480, 400)
    camera.framerate = 10
    camera.vflip = True
    camera.hflip = True

    camera.start_preview()
    # Give the camera some warm-up time
    time.sleep(2)
    
    # capture_sequence accepts a sequence or iterator of outputs each of which must either be a string specifying a filename for output.
    # The use_video_port parameter controls whether the camera's image or video port is used to capture images. Default: false --> camera. 
    # When ture, the splitter_port parameter specifies the port of the video splitter that the image encoder will be attached to. 

    camera.capture_sequence(outputs(), 'jpeg', use_video_port=True)
    
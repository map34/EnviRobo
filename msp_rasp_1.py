# Combine working object recognition with the serial UART
# outer while for waiting for the MSP instruction
# inner while for turning and sending TR/TL until object centered

import time
import picamera
import io
import cv2
import numpy as np
import serial

ser = serial.Serial("/dev/ttyAMA0", 115200)
go = 0
count = 0
first=True
y_face=0
y_banana=0

def outputs():
    global count
    global first
    global y_face
    global y_banana
    stream = io.BytesIO()
    name = 0
    # Each iteration generates one image
    for i in range(10):
        count = count+1
        # This returns the stream for the camera to capture to
        yield stream

        # construct a numpy array from the stream
        data = np.fromstring(stream.getvalue(), dtype=np.uint8)

        # Reset the stream for the next capture
        stream.seek(0)      # Change the stream position to the given byte offset
        stream.truncate()   # Resize the stream to the current position

        # "decode" the image from the array, preserving colour in BGR format
        image = cv2.imdecode(data, 1)

        #Load a cascade file for detecting target
        haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_frontalface_alt.xml')
        #haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_lefteye_2splits.xml')
        #face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/banana.xml')
        #haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/bottle.xml')
        #face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_russian_plate_number.xml')
	#face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/bottle.xml')
	face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/clock.xml')

        # get a gray picture
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

        #Look for target in the image using the loaded cascade file
        faces = haar.detectMultiScale(gray, 1.1, 6)
        bananas = face_cascade.detectMultiScale(gray, 1.1, 5)

	if (first):
		#Draw a rectangle around every found bottle
        	for (x,y,w,h) in faces:
        		y_face = y+h
        	
        	#Draw a rectangle around every found face
        	for (x,y,w,h) in bananas:
        		y_banana = y+h

        	first = False; 
        	print "faces's y is " + str(y_face)
        	print "banana's y is " + str(y_banana)
        	
		if (y_face > y_banana):
			#Create bottle file "0"
			file = open("bottleORbanana.txt", "w")
			file.write("0")
			file.close()
			print "go for face"
			
        	elif(y_face < y_banana):
			#Create face file "1"
			file = open("bottleORbanana.txt", "w")
			file.write("1")
			file.close()
			print "go for banana"

        #not the first time anymore so first=False
	else:
		if (y_face > y_banana):
        		for (x,y,w,h) in faces:
        			draw(x, y, w, h, image, 255, 51, 153)
        		print "Found "+ str(len(faces))+" face(s)"
        	
		elif(y_face < y_banana):
			for (x,y,w,h) in bananas:
				draw(x, y, w, h, image, 0, 255, 255)
			print "Found "+str(len(bananas))+" banana(s)"
		else:
			pass
		
	#Generate consecutive file names
	filename = "result%02d.jpg" % name              #increment a number for filename
	filename = "result{0:02d}.jpg".format(name)     #cast int to string

        #Save the result image
        cv2.imwrite(filename,image)
        name += 1

    #After 10 images, send UART to MSP
    if (go>6):
        ser.write("F")
        print "send Go"
    else:
        #nothing
        ser.write("M")
        print "Miss"
        
def draw(x, y, w, h, image, R, G, B):
    global go
    cv2.rectangle(image,(x,y),(x+w,y+h),(B,G,R),2)

    cv2.putText(image, "Found something", (5, 220), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))

    coord = "x=%d, y=%d" % (x, y)

    cv2.putText(image, coord, (x, y+h+15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 128, 0))
 
    go = go + 1

def main():
    global first
    global count
    global go
    global y_face	
    global y_banana
    while(1):
        print "Waiting for MSP......"
        flag = ser.read(1)                                               # Read 1 character from serial port
        print "MSP sent me a %s" % flag
    		
        if (str(flag) != "t"):
            with picamera.PiCamera() as camera:
                camera.resolution = (320, 240) #`:320, 240
                camera.framerate = 10
                camera.vflip = True
		camera.hflip = True

                camera.start_preview(fullscreen=False, window=(100, 20, 640, 480))
                # Give the camera some warm-up time
                time.sleep(2)

                # capture_sequence accepts a sequence or iterator of outputs each of which must either be a string specifying a filename for output.
                # The use_video_port parameter controls whether the camera's image or video port is used to capture images. Default: false --> camera.
                # When ture, the splitter_port parameter specifies the port of the video splitter that the image encoder will be attached to.
                # This while handles the robot position until it finds a in-range object
                while (count<10):
			camera.capture_sequence(outputs(), 'jpeg', use_video_port=True)
                camera.stop_preview()
                go = 0
                first=True
                count=0
                y_face=0
                y_banana=0
        # When flag is t (trapped)
        else:
            break
    ser.close()


if __name__ == "__main__":
    main()

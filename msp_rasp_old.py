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
miss=0
NotFound=True
go = 0
gl = 0
gr = 0
count = 0

def outputs():
    global miss
    global NotFound
    global count
    stream = io.BytesIO()
    name = 0
    # Each iteration generates one image
    for i in range(10):
        count = count+1
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

        #Load a cascade file for detecting target
        face_cascade = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_frontalface_alt.xml')
        haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_lefteye_2splits.xml')
        #haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/banana.xml')
        #haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/bottle.xml')

        # get a gray picture
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

        #Look for target in the image using the loaded cascade file
        bottles = haar.detectMultiScale(gray, 1.1, 6)
        faces = face_cascade.detectMultiScale(gray, 1.1, 5)

        if (len(bottles) > 0):
            #Draw a rectangle around every found bottle
            for (x,y,w,h) in bottles:
                draw(x, y, w, h, image, 255, 51, 153)
            print "Found "+ str(len(bottles))+" bottle(s)"
        elif (len(faces) > 0) and (len(bottles) == 0):
            #Draw a rectangle around every found face
            for (x,y,w,h) in faces:
                draw(x, y, w, h, image, 0, 255, 255)
            print "Found "+str(len(faces))+" face(s)"
        else:
            #At this point either (bottle=0 and faces = 0) or (bottle>0 and faces>0)
            pass
	
	#Generate consecutive file names
	filename = "result%02d.jpg" % name              #increment a number for filename
	filename = "result{0:02d}.jpg".format(name)     #cast int to string

        #Save the result image
        cv2.imwrite(filename,image)
        name += 1

    #After 10 images, send UART to MSP
    if (go>6):
        ser.write("GO")
        print "send Go"
    elif (gr>5):
        ser.write("GR")
        print "send Go Right"
    elif (gl>5):
        ser.write("GL")
        print "send Go Left"
    else:
        #nothing
        ser.write("MS")
        print "Miss"
        
def draw(x, y, w, h, image, R, G, B):
    global gr
    global go
    global gl
    cv2.rectangle(image,(x,y),(x+w,y+h),(B,G,R),2)

    cv2.putText(image, "Found something", (5, 220), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))

    coord = "x=%d, y=%d" % (x, y)

    cv2.putText(image, coord, (x, y+h+15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 128, 0))

    # left = x
    # right = 320 - x - w
    # Assuming only one object will be found in each image, check if the object is centered enough
    # Left/right max: (horizontal window size - (rectangle width)) / 2
    # leftmax = (320-width)/2
    # rightmax = (320-width)/2
    diff =  2 * x + w - 320        # basically diff = left-right
    if diff < 40 and diff >= 0:
        cv2.putText(image, "within range", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
        go = go + 1
    #left>right, so move more right
    elif (diff) >= 40:
        cv2.putText(image, "too left", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
        gr = gr + 1
    # diff < 0, right > left, so move more left
    elif (diff) < 0:
        cv2.putText(image, "too right", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))
        gl = gl + 1
    else:
        cv2.putText(image, "nonono", (5, 230), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 204, 102))

def main():
    while(1):
        #ser = serial.Serial("/dev/ttyAMA0", 115200)  # baud rate is 115200
        print "Waiting for MSP......"
        data = ser.read(1)                                               # Read 1 character from serial port to data
        print "MSP sent me a %s" % data

        with picamera.PiCamera() as camera:
            camera.resolution = (320, 240)
            camera.framerate = 10

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
            break
    ser.close()


if __name__ == "__main__":
    main()

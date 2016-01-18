import io
import picamera
import cv2
import numpy

#Create a memory stream so photos doesn't need to be saved in a file
stream = io.BytesIO()

#Get the picture (low resolution, so it should be quite fast)
#Here you can also specify other parameters (e.g.:rotate the image)
with picamera.PiCamera() as camera:
    camera.resolution = (480, 400)
    camera.vflip=True
    camera.hflip=True
    camera.capture(stream, format='jpeg')

#Convert the picture into a numpy array
buff = numpy.fromstring(stream.getvalue(), dtype=numpy.uint8)

#Now creates an OpenCV image
image = cv2.imdecode(buff, 1)

#Load a cascade file for detecting bottles
haar = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/banana.xml')

#Convert to grayscale
gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)

#Look for bottles in the image using the loaded cascade file
bottles = haar.detectMultiScale(gray, 1.1, 6)

print "Found "+str(len(bottles))+" banana(s)"

#Draw a rectangle around every found bottle
for (x,y,w,h) in bottles:
    cv2.rectangle(image,(x,y),(x+w,y+h),(255,0,0),2)

#Save the result image
cv2.imwrite('result.jpg',image)
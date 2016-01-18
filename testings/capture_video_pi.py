# import the necessary packages
import io
import picamera
import time

my_stream = io.BytesIO()
with picamera.PiCamera() as camera:
	camera.start_preview()
	
	time.sleep(2)
	camera.capture(my_stream, 'jpeg')
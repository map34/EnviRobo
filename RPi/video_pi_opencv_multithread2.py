import io
import time
import threading
import picamera
import cv2
import numpy as np

# Create a pool of image processors
done = False
lock = threading.Lock()
pool = []

class ImageProcessor(threading.Thread):
    def __init__(self):
        super(ImageProcessor, self).__init__()
        self.stream = io.BytesIO()
        self.event = threading.Event()
        self.terminated = False
        self.start()
        #self.camera = picamera.PiCamera()
        #self.cascadeXml = cv2.CascadeClassifier('/home/pi/opencv-3.0.0/data/haarcascades/haarcascade_frontalface_alt.xml')

    def run(self):
        # This method runs in a separate thread
        global done
        while not self.terminated:
            # Wait for an image to be written to the stream
            if self.event.wait(1):
                try:
                    self.stream.seek(0)
                    # Read the image and do some processing on it
                    #Image.open(self.stream)
                    frame = self.stream.array
                    cv2.imshow('stream', frame)                   
                    
                    
                    '''self.camera.capture(self.stream, format='jpeg', use_video_port=True)
                    data = np.fromstring(self.stream.getvalue(), dtype=np.uint8)
                    self.stream.truncate()
                    self.img = cv2.imdecode(data, cv2.CV_LOAD_IMAGE_COLOR)
                    gry = cv2.cvtColor(self.img, cv2.COLOR_BGR2GRAY)
                    haar = self.cascadeXml.detectMultiScale(gry, 1.1, 6)
                    if len(haar):
                    	bottleX = (haar[0][0] + haar[0][2] / 2);
			bottleY = self.height - (haar[0][1] + haar[0][3] / 2) ;
			self.dataQueue.put({"type": BOTTLE, "position": (bottleX, bottleY)})
			done = True
		    '''
		
                    # Set done to True if you want the script to terminate
                    # at some point
                    #done=True
                finally:
                    # Reset the stream and event
                    self.stream.seek(0)
                    self.stream.truncate()
                    self.event.clear()
                    # Return ourselves to the pool
                    with lock:
                        pool.append(self)

def streams():
    while not done:
        with lock:
            if pool:
                processor = pool.pop()
            else:
                processor = None
        if processor:
            yield processor.stream
            processor.event.set()
        else:
            # When the pool is starved, wait a while for it to refill
            time.sleep(0.1)

with picamera.PiCamera() as camera:
    pool = [ImageProcessor() for i in range(4)]
    camera.resolution = (640, 480)
    camera.framerate = 30
    camera.start_preview()
    time.sleep(2)
    camera.capture_sequence(streams(), use_video_port=True)

# Shut down the processors in an orderly fashion
while pool:
    with lock:
        processor = pool.pop()
    processor.terminated = True
    processor.join()
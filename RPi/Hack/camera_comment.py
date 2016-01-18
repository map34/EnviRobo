#!/usr/bin/env python
import io 			# provides the Python interfaces to stream handling. 
import picamera
import threading		# High level threading interface
import cv2
import numpy as np
import Queue                    # implements multi-producer, multi-consumer queues. 

# types of elements to detect
OBSTACLE, BOTTLE = range(2)

# threading.Thread is a class that represents a thread of control. 
# To specify the activity: (1) By passing a callable object to the constructor
# or (2) by overriding the run() method in subclass
class ImageProcessing(threading.Thread):   
	def __init__(self, width, height, preview):		
		super(ImageProcessing, self).__init__()

		self.obstacleThreshold = 100000
		
		self.preview = preview
		
		self.width = width
		self.height = height
		
		self.camera = picamera.PiCamera()
		
		self.camera.resolution = (self.width, self.height)
		
		# boost colors
		self.camera.saturation = 100
		
		# load the xml file for 
		self.cascadeXml = cv2.CascadeClassifier('bottle.xml')
		
		# load the mask that hides the non important parts of the image
		self.mask = cv2.imread("mask.png", 0)
		
		self.stream = io.BytesIO()     		# io.BytesIO()is a stream implementation using an in-memory bytes buffer.
		self.dataQueue = Queue.Queue() 		# Constructor for a FIFO queue. 
		
		# if enabled show the preview window
		if self.preview:
			cv2.namedWindow("Preview", flags=cv2.cv.CV_WINDOW_AUTOSIZE)
		
		# event to stop the thread
		self.running = threading.Event()        # A factory function that returns a new event object. An event manages a flag that can be set to true with the set() method. 

	def run(self):
		print "Image processing thread started"
		# make sure we're still running
		while not self.running.isSet():
			# take the picture
			self.camera.capture(self.stream, format='jpeg', use_video_port=True)

			# construct a numpy array from the stream
			data = np.fromstring(self.stream.getvalue(), dtype=np.uint8)

			# Reset the stream and event
			self.stream.truncate()		# Resize the stream to the current position
			self.stream.seek(0)		# Change the stream position to the given byte offset
			
			# "decode" the image from the array, preserving colour in BGR format
			self.img = cv2.imdecode(data, cv2.CV_LOAD_IMAGE_COLOR)

			# image that can be modified by the image processing for preview purposes
			if self.preview:
				self.previewImage = self.img
			
			#self.detectObstacles()
			self.detectBottles()

			if self.preview:
				try:
					cv2.imshow("Preview", self.previewImage)
				except:
					pass

		# destroy the preview window
		if self.preview:
			cv2.destroyAllWindows()

	def stop(self):
		print "Stopping image processing thread"
		self.running.set()
			
	def detectBottles(self):
		# get a gray picture
		gry = cv2.cvtColor(self.img, cv2.COLOR_BGR2GRAY)
		
		# run the haar cascade algorithm (slow)
		haar = self.cascadeXml.detectMultiScale(gry, 1.1, 6)

		# update preview
		if self.preview:
			for (x,y,w,h) in haar:
				cv2.rectangle(self.previewImage, (x,y), (x+w, y+h), (255, 255, 255), 1)

		# normalised value between 0 and 1 of the first bottle position
		if len(haar):
			bottleX = (haar[0][0] + haar[0][2] / 2);
			bottleY = self.height - (haar[0][1] + haar[0][3] / 2) ;
			self.dataQueue.put({"type": BOTTLE, "position": (bottleX, bottleY)})    #Put item into the queue
		
	def detectObstacles(self):
		colorFloorL = np.array([8, 30, 30], np.uint8)
		colorFloorH = np.array([28, 255, 255], np.uint8)

		# convert to hue, saturation, value format
		hsv = cv2.cvtColor(self.img, cv2.COLOR_BGR2HSV)
		
		colorThreshold = cv2.inRange(hsv, colorFloorL, colorFloorH)
		colorThreshold = np.invert(colorThreshold)

		cv2.erode(colorThreshold, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7)), colorThreshold, (-1, -1), 1)
		np.bitwise_and(colorThreshold, self.mask, colorThreshold)

		# average values vertically and split screen in 4 horizontally
		sumVertical = np.sum(colorThreshold, axis = 0)
		splitHorizontal = np.array([np.sum(sumVertical[0:63]), np.sum(sumVertical[64:127]), np.sum(sumVertical[128:191]), np.sum(sumVertical[192:255])])
		# print "H", splitHorizontal

		if self.preview:
			# show obstacle position with the overlay on the image
			mask = np.invert(colorThreshold)
			np.bitwise_and(self.previewImage[:,:,0], mask, self.previewImage[:,:,0])
			np.bitwise_and(self.previewImage[:,:,1], mask, self.previewImage[:,:,1])
			np.bitwise_and(self.previewImage[:,:,2], mask, self.previewImage[:,:,2])
			pass

		peakX = splitHorizontal.argmax()

		# obstacle threshold has been reached, obstacle has been detected
		if splitHorizontal[peakX] > self.obstacleThreshold:

			# average values horizontally and split screen in 4 vertically
			sumHorizontal = np.sum(colorThreshold, axis = 1)
			splitVertical = np.array([np.sum(sumHorizontal[0:31]), np.sum(sumHorizontal[32:63]), np.sum(sumHorizontal[64:95]), np.sum(sumHorizontal[96:127])])
			#print "V", splitVertical

			peakY = splitVertical.argmax()

			self.dataQueue.put({"type": OBSTACLE, "position": (peakX, peakY)})

	def checkDataQueue(self):
		if not self.dataQueue.empty():
			return self.dataQueue.get()   # Remove and return an iten from the queue. 
		else:
			return False
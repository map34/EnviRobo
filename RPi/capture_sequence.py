# A code from advanced recipes on picamera
# Capable of capturing a sequence of images extremely rapidly by utilizing 
# its video capture capabilities with a JPEG encoder.
# try to combine this with processing an image 
# do something like for each frame in camera, convert color and stuff 
# combine with the working image bottle detection

import time
import picamera

# number of frames to sample in the video
frames = 10

# Just a simple generator expression to provide the filenames for processing instead of 
# specifying every single filename manually

def filenames():
    frame = 0
    while frame < frames:
        yield 'image%02d.jpg' % frame
        frame += 1

with picamera.PiCamera() as camera:
    camera.resolution = (1024, 768)
    camera.framerate = 30
    camera.start_preview()
    # Give the camera some warm-up time
    time.sleep(2)
    start = time.time()
    
    # capture_sequence accepts a sequence or iterator of outputs each of which must either be a string specifying a filename for output.
    # The use_video_port parameter controls whether the camera's image or video port is used to capture images. Default: false --> camera. 
    # When ture, the splitter_port parameter specifies the port of the video splitter that the image encoder will be attached to. 
    
    camera.capture_sequence(filenames(), use_video_port=True)
    finish = time.time()
print('Captured %d frames at %.2ffps' % (
    frames,
    frames / (finish - start)))
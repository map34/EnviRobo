import RPi.GPIO as GPIO  
import time  
  
def blink(pin):                         # blinking function
	GPIO.output(pin,GPIO.HIGH)  
	time.sleep(1)  
        GPIO.output(pin,GPIO.LOW)  
	time.sleep(1)  
	return  
  
GPIO.setmode(GPIO.BOARD)                # to use Raspberry Pi board pin numbers
  
GPIO.setup(7, GPIO.OUT)                 # set up GPIO output channel

for i in range(0,50):  			# blink GPIO17 50 times  
	blink(7)  

GPIO.cleanup() 
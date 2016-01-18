import serial

while(1):
	ser = serial.Serial("/dev/ttyAMA0", 115200)
	#ser.write("Hello there my friends")
	print "Not stuck"
	data = ser.read(1)   #Read 1 character from serial port to data
	print "we are reading a %s" % data
	ser.write(data)	     #Send back the received data
	ser.close()

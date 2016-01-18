# Code to run the entire image processing and compass sensor data acquisition
# Must be run with python3
from i2clibraries import i2c_hmc5883l
from i2clibraries import i2c_adxl345
import time
import os
import serial

# get compass object to communicate through i2c
compass = i2c_hmc5883l.i2c_hmc5883l(1)
trap = 0
accel = i2c_adxl345.i2c_adxl345(1)
ser = None
read_data = ''
read_object = -1
[BANANA, BOTTLE] = ['0','1']
correct_heading = 0

def main():
    global compass
    global accel
    global trap
    global ser
    global read_object
    global read_data
    global correct_heading

    # Configuring the compass sensor
    compass.setContinuousMode()
    compass.setDeclination(16,0)
    
    while(1):
        ''' Running image processing code '''
        if (trap == 0):
            os.system('python msp_rasp_1.py') 
            trap = 1
            ''' WOpen Serial after image processing '''
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            fo = open('bottleORbanana.txt', 'r')
            read_object = fo.read(1)
            ''' 0 for banana and 1 for bottle '''
            print('Object code is ' + read_object)
        elif (trap == 1 and correct_heading == 0):
            ''' Sending heading signal '''
            print('correct_heading: ' + str(correct_heading))
            print('trap: ' + str(trap))
            while True:
                head = compass.getHeading()
                print('Heading: ' + str(head[0]) + ', ' + str(head[1]))
                if(head[0] >= 342 and head[0] < 348 ):
                    ser.write(bytes('s','UTF-8'))
                    correct_heading = 1
                    break
                elif (head[0] > 180):
                    ser.write(bytes('l', 'UTF-8'))
                elif (head[0] < 180):
                    ser.write(bytes('r', 'UTF-8'))
                elif (head[0] == 180):
                    ser.write(bytes('r','UTF-8'))
                ''' Wait until MSP sends a hit-a-wall signal '''
                print('Waiting for what-else command...')
                read_data = ser.read(1)
                print('what am I receiving from MSP:?' + read_data.decode('UTF-8'))
        ''' Robot now is heading towards North  '''
        elif (correct_heading == 1):
            read_data = ser.read(1)
            print(read_data)
            ''' Found object'''
            if (read_data.decode('UTF-8') == 'w'):
                print('Hit a wall!')
                if(read_object == BANANA):
                    ser.write(bytes('y', 'UTF-8'))
                elif (read_object == BOTTLE):
                    ser.write(bytes('z', 'UTF-8'))
            ''' MSP sent a successful drop signal '''
            elif (read_data.decode('UTF-8') == 'p'):
                print('Successfully dropped object')
                print('Restarting system')
                correct_heading = 0
                trap = 0
                ser.close()
                read_object = -1

if __name__ == "__main__":
    main()

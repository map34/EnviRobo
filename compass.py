from i2clibraries import i2c_hmc5883l
from i2clibraries import i2c_adxl345
import time
import os
import serial


compass = i2c_hmc5883l.i2c_hmc5883l(1)
trap = 0
accel = i2c_adxl345.i2c_adxl345(1)
ser = None
read_data = ''
read_object = -1
[BANANA, BOTTLE] = ['0','1']
correct_heading = 0

correct_heading = 0

def main():
    global compass
    global accel
    global trap
    global ser
    global read_object
    global read_data
    global correct_heading

    compass.setContinuousMode()
    compass.setDeclination(16,0)
    
    while(1):
        ''' Running Vivi's code '''
        if (trap == 0):
            #os.system("echo haha")
            # Running Vivi's code
            os.system('python msp_rasp_1.py') 
            trap = 1
            # Open serial after Vivi's done
            ser = serial.Serial("/dev/ttyAMA0", 115200)
            fo = open('bottleORbanana.txt', 'r')
            read_object = fo.read(1)
            # Read Vivi's object
            print('Object code is ' + read_object)
        elif (trap == 1 and correct_heading == 0):
            '''read_data = ser.read() # wait until msp sends a trap byte, good byte
            head = compass.getHeading()
            (x1,y1,z1) = compass.getAxes()
            (x, y, z) = accel.getAxes()
            print('x: ' + str(x1) + ', y: ' + str(y1))
            print(head[0])
            # Heading logic
            if (head[0] >= 0 and head[0] < 2):
                ser.write(bytes('s','UTF-8'))
                correct_heading == 1
            else if (head[0] > 180):
                ser.write(bytes('r','UTF-8'))
            else if (head[0] < 180):
                ser.write(bytes('l','UTF-8'))'''
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
                ''' Wait until Antaeus sends a hit-a-wall signal '''
                print('Waiting for what-else command...')
                read_data = ser.read(1)
                print('what am I receiving from Antaeus:?' + read_data.decode('UTF-8'))
        elif (correct_heading == 1):
            #print('Waiting to hit a wall!')
            read_data = ser.read(1)
            print(read_data)
            if (read_data.decode('UTF-8') == 'w'):
                print('Hit a wall!')
                if(read_object == BANANA):
                    ser.write(bytes('y', 'UTF-8'))
                elif (read_object == BOTTLE):
                    ser.write(bytes('z', 'UTF-8'))
            elif (read_data.decode('UTF-8') == 'p'):
                print('Successfully dropped object')
                print('Restarting system')
                correct_heading = 0
                trap = 0
                ser.close()
                read_object = -1
            #time.sleep(1)
            
        #time.sleep(1)
    
    #print('Successfully dropped an object!')
    #ser.close()



if __name__ == "__main__":
    main()

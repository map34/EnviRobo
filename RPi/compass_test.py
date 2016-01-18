from i2clibraries import i2c_hmc5883l
import time
compass = i2c_hmc5883l.i2c_hmc5883l(1)

def main():
    global compass
    compass.setContinuousMode()
    compass.setDeclination(16,0)

    while(1):
        heading = compass.getHeading()
        print('Heading: ' + str(heading[0]) + ', ' + str(heading[1]))
        time.sleep(1)

if __name__ == '__main__':
    main()

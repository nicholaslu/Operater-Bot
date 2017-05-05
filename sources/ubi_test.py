import math
import time
import serial
from ubidots import ApiClient


serial = serial.Serial('/dev/ttyACM0',9600,timeout=1)

# Create an ApiClient object

api = ApiClient(token='iezgWhoTrRDeIdul7oiZJjcXcm1iUv')

# Get a Ubidots Variable

fwdVar = api.get_variable('58eedc7176254207a757b9d4')
bwdVar = api.get_variable('58eedc7976254207a5b84397')
lftVar = api.get_variable('58eedca376254207a757bcfa')
rgtVar = api.get_variable('58eedca376254207a757bd06')

#Get the value from Ubidots

try:
    while(1):
        # Get commands
        last_fwdVar = fwdVar.get_values(1)
        last_fwdCmd = last_fwdVar[0]['value']
        
        last_bwdVar = bwdVar.get_values(1)
        last_bwdCmd = last_bwdVar[0]['value']
        
        last_lftVar = lftVar.get_values(1)
        last_lftCmd = last_lftVar[0]['value']

        last_rgtVar = rgtVar.get_values(1)
        last_rgtCmd = last_rgtVar[0]['value']
        
        print ("FORWARD COMMAND IS")
        print (last_fwdCmd)
        print ("BACKWARD COMMAND IS")
        print (last_bwdCmd)
        print ("TURN-LEFT COMMAND IS")
        print (last_lftCmd)        
        print ("TURN-RIGHT COMMAND IS")
        print (last_rgtCmd)

        # Contorl arduino's movement
        if last_fwdCmd == 1:
            serial.write('f')
            print("Go forward")
            fwdVar.save_value({"value":0})
        elif last_bwdCmd == 1:
            serial.write('b')
            print('Go backward')
            bwdVar.save_value({"value":0})
        elif last_lftCmd == 1:
            serial.write('l')
            print('Turn left')
            bwdVar.save_value({"value":0})
        elif last_rgtCmd == 1:
            serial.write('r')
            print('Turn right')
            bwdVar.save_value({"value":0})
        elif last_fwdCmd == 0 and last_bwdCmd == 0 and last_lftCmd == 0 and last_rgtCmd == 0:
            serial.write('s')
            print('Stopped')
            
        time.sleep(1)
except:
    serial.close()

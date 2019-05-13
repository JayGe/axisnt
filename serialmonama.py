#!/usr/bin/python
# Reads the serial port and writes whatever is seen to mqtt if looks okay

import serial
import paho.mqtt.client as paho
import re

serialdev = '/dev/amp'
rate = 115200
port = 1883

try:
    print "Connecting... ", serialdev
    ser = serial.Serial(serialdev, rate, timeout=20)
except:
    print "Failed to connect serial"
    raise SystemExit

try:
    ser.flushInput()
    client=paho.Client()
    client.connect("1.2.3.4")

    #read data from serial and publish
    while client.loop() == 0:
        line = ser.readline()
        #split line commas
        list = line.split(",")
	#check starts as expected
        if re.match("^\$ADCS$", list[0]):
            print line
            client.publish("/radio/amp/serialall", line)
            client.publish("/radio/amp/serial/logicV", list[1])
            client.publish("/radio/amp/serial/rightV", list[2])
            client.publish("/radio/amp/serial/leftV", list[3])
            client.publish("/radio/amp/serial/tempC", list[4])
            client.publish("/radio/amp/serial/leftForP", list[5])
            client.publish("/radio/amp/serial/rightForP", list[6])
            client.publish("/radio/amp/serial/leftRefP", list[7])
            client.publish("/radio/amp/serial/rightRefP", list[8])
        pass

# handle list index error (i.e. assume no data received)
except (IndexError):
    print "No data received within serial timeout period"
    ser.close()
    client.disconnect()
   
# handle app closure
except (KeyboardInterrupt):
    print "Interrupt received"
    ser.close()
    client.disconnect()

except (RuntimeError):
    print "uh-oh! time to die"
    ser.close()
    client.disconnect()

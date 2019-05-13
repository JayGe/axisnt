#!/usr/bin/python

import re
import paho.mqtt.client as mqtt
from time import sleep
import serial

eof = "\xff\xff\xff"

inc=999

port=serial.Serial(port='/dev/ttyAMA0',baudrate=9600, timeout=1.0)

'''
while True:
        sleep(1)
        #print "Read line"
        #rcv=port.readline()
        #print "received" + repr(rcv)
        print "***"
        port.write('lo.txt="' + str(inc) + '"' + eof)
        port.write('ro.txt="' + str(inc) + '"' + eof)
        port.write('lr.txt="' + str(inc) + '"' + eof)
        port.write('rr.txt="' + str(inc) + '"' + eof)
        port.write('lv.txt="' + str(inc) + '"' + eof)
        port.write('rv.txt="' + str(inc) + '"' + eof)
        port.write('ta.txt="' + str(inc) + '"' + eof)
        port.write('bv.txt="' + str(inc) + '"' + eof)
        port.write('tc.txt="' + str(inc) + '"' + eof)
        port.write('hc.txt="' + str(inc) + '"' + eof)
        inc=inc+1
'''

# The callback for when the client receives a CONNACK response from the server.
def main():
  def on_connect(client, userdata, flags, rc):
	client.subscribe("/radio/amp/#")
  def on_message(client, userdata, msg):
	if msg.topic == "/radio/amp/serialall" :
		line = msg.payload
		list = line.split(",")
		if re.match("^\$ADCS$", list[0]):
			port.write('lo.txt="' + str(list[5]) + '"' + eof)
			port.write('ro.txt="' + str(list[6]) + '"' + eof)
			port.write('lr.txt="' + str(list[7]) + '"' + eof)
			port.write('rr.txt="' + str(list[8]) + '"' + eof)
			port.write('lv.txt="' + str(list[3]) + '"' + eof)
			port.write('rv.txt="' + str(list[2]) + '"' + eof)
			port.write('ta.txt="' + str(list[4]) + '"' + eof)
			port.write('bv.txt="' + str(list[1]) + '"' + eof)
			
	if msg.topic == "/radio/amp/humid" :
		print msg.payload
		port.write('hc.txt="' + str(msg.payload) + '"' + eof)

	if msg.topic == "/radio/amp/temp" :
		port.write('tc.txt="' + str(msg.payload) + '"' + eof)

	if msg.topic == "/radio/amp/status1" :
		if msg.payload == "ON" :
			print "Amplifier 1 On"
		if msg.payload == "OFF" :
			print "Amplifier 1 Off"

	if msg.topic == "/radio/amp/status2" :
		if msg.payload == "ON" :
			print "Amplifier 2 On"
		if msg.payload == "OFF" :
			print "Amplifier 2 Off"

  client = mqtt.Client()
  client.on_connect = on_connect
  client.on_message = on_message

  client.connect("1.2.3.4", 1883, 60)

  client.loop_forever()

if __name__ == '__main__':
	try:
	  	main()
	except KeyboardInterrupt:
		print("Keyboard interrupt")
	except:
		print("some error")
	finally:
		print("clean up")


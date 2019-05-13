#!/usr/bin/python

import paho.mqtt.client as mqtt
from time import sleep
import RPi.GPIO as GPIO
import threading
from subprocess import call

GPIO.setmode(GPIO.BCM)

SleepTimeL = 0.2

pinList = [17,18]

for i in pinList:
	GPIO.setup(i, GPIO.OUT)
	GPIO.output(i, GPIO.HIGH)

def main():
  def ampl():
	GPIO.output(17, GPIO.HIGH)
	sleep(SleepTimeL);
	client.publish("/radio/amp/status1", "OFF")
	print "Amplifier 1 Off Timeout"

  def ampr():
	GPIO.output(18, GPIO.HIGH)
	sleep(SleepTimeL);
	client.publish("/radio/amp/status2", "OFF")
	print "Amplifier 2 Off Timeout"

  def on_connect(client, userdata, flags, rc):
	client.subscribe("/radio/amp/#")

  def on_message(client, userdata, msg):
	if msg.topic == "/radio/amp/shutdown" :
		if msg.payload == "OFF" :
			call("sudo shutdown -h now", shell=True)
		if msg.payload == "REBOOT" :
			call("sudo shutdown -r now", shell=True)
	if msg.topic == "/radio/amp/set1" :
		if msg.payload == "ON" :
			GPIO.output(17, GPIO.LOW)
			sleep(SleepTimeL);
			client.publish("/radio/amp/status1", "ON")
			print "Amplifier 1 On"
			global lt
			lt = threading.Timer(1800, ampl)
			lt.start()
		if msg.payload == "OFF" :
			GPIO.output(17, GPIO.HIGH)
			sleep(SleepTimeL);
			client.publish("/radio/amp/status1", "OFF")
			print "Amplifier 1 Off"
			lt.cancel()
	if msg.topic == "/radio/amp/set2" :
		if msg.payload == "ON" :
			GPIO.output(18, GPIO.LOW)
			sleep(SleepTimeL);
			client.publish("/radio/amp/status2", "ON")
			print "Amplifier 2 On"
                        global rt
                        rt = threading.Timer(1800, ampr)
                        rt.start()
		if msg.payload == "OFF" :
			GPIO.output(18, GPIO.HIGH)
			sleep(SleepTimeL);
			client.publish("/radio/amp/status2", "OFF")
			print "Amplifier 2 Off"
			rt.cancel()
	
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
		GPIO.cleanup() # cleanup all GPIO


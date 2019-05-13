#!/usr/bin/python
import Adafruit_DHT
import paho.mqtt.client as paho
import time

client=paho.Client()
client.connect("1.2.3.4")

pin = 27

while True:
	humidity,temperature = Adafruit_DHT.read_retry(Adafruit_DHT.AM2302,pin)

	client.publish("/radio/amp/temp", format(round(temperature,2)))
	time.sleep(1)
	client.publish("/radio/amp/humid", format(round(humidity,2)))
	time.sleep(5)

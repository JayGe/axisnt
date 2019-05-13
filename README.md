# axisnt amplifier bits

Some scripts for controlling the amplifier, could be neater but works fine as is. 

sudo apt-get install python-pip
sudo pip install Adafruit_DHT
sudo pip install paho-mqtt
sudo pip install pyserial

* serialmonama.py is serial usb -> mqtt
* temphumid.py reads dht11 -> mqtt
* relay.py reads/writes to mqtt for relay control
* screenrc file for starting the scripts called with su - pi -c "screen -dm -S ampcontrol -c /home/pi/amplifier/screenrc"
* nextion.py sets the nextion display if attached
* amp2.HMI file for the nextion display editor
* nextion-esp866-amplifier.ino arduino code for esp8266+nextion

On the Portsdown we need to add a couple of entries to have it send the relay message, for the Lime tue turn on is always in one place, for off needs to be in two places for both menu and touchscreen. 

In "scripts/lime_ptt.sh" to turn it on regardless:

  /usr/bin/mosquitto_pub -h 1.2.3.4 -t '/radio/amp/set2' -m 'ON' 

In scripts/menu.sh in do_stop_transmit():

  /usr/bin/mosquitto_pub -h 1.2.3.4 -t '/radio/amp/set2' -m 'OFF'

In src/gui/rpidatvtouch.c TransmitStop() 

  system("/usr/bin/mosquitto_pub -h 1.2.3.4 -t '/radio/amp/set2' -m 'OFF'");
 

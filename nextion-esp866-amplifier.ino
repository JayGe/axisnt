// V2 with the buttons working
// Loading the required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Nextion.h"

int lastConnectionAttempt = millis();
int connectionDelay = 5000; // try to reconnect every 5 seconds

const char* ssid = "ssid";
const char* password = "password";

const char* mqtt_server = "1.1.1.1";

WiFiClient espClient;
PubSubClient client(espClient);

NexText lo = NexText(0, 5, "lo");
NexText ro = NexText(0, 6, "ro");
NexText lr = NexText(0, 7, "lr");
NexText rr = NexText(0, 8, "rr");
NexText lv = NexText(0, 9, "lv");
NexText rv = NexText(0, 10, "rv");
NexText ta = NexText(0, 11, "ta");
NexText bv = NexText(0, 16, "bv");

NexText hc = NexText(0, 13, "hc");
NexText tc = NexText(0, 12, "tc");

NexDSButton lb = NexDSButton(0, 19, "lb");
NexDSButton rb = NexDSButton(0, 18, "rb");

NexTouch *nex_listen_list[] = {
  &lb,
  &rb,
  NULL
};

void lbPopCallback(void *ptr) {  
  uint32_t lbStat = 0;
  lb.getValue(&lbStat);
  if(lbStat == 1){
    client.publish("/radio/amp/button", "left pop on");
  } else {
    client.publish("/radio/amp/button", "left pop off");
  }
}

void rbPopCallback(void *ptr) {
  uint32_t rbStat = 0;
  rb.getValue(&rbStat);
  if(rbStat == 1){
    client.publish("/radio/amp/button", "right pop on");
  } else {
    client.publish("/radio/amp/button", "right pop off");
  }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length();

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
} 

void setup_wifi() {
  while (WiFi.status() != WL_CONNECTED){
  delay(10);

  if (millis() - lastConnectionAttempt >= connectionDelay) {
    lastConnectionAttempt = millis();
      
    WiFi.begin(ssid, password);
    }
  }
}


void callback(String topic, byte* message, unsigned int length) {
  String messageString;
  
  for (int i = 0; i < length; i++) {
    messageString += (char)message[i];
  }

  const char* messageC = messageString.c_str();
  
  if(topic=="/radio/amp/temp"){
     tc.setText(messageC);
  }
  else if(topic=="/radio/amp/humid"){
     hc.setText(messageC);
  }
  else if(topic=="/radio/amp/serialall"){
    if (messageString.startsWith("$ADCS")){
      bv.setText(getValue(messageString, ',', 1).c_str());
      rv.setText(getValue(messageString, ',', 2).c_str());
      lv.setText(getValue(messageString, ',', 3).c_str());
      ta.setText(getValue(messageString, ',', 4).c_str());
      lo.setText(getValue(messageString, ',', 5).c_str());
      ro.setText(getValue(messageString, ',', 6).c_str());
      lr.setText(getValue(messageString, ',', 7).c_str());
      rr.setText(getValue(messageString, ',', 8).c_str());
    }
  }
  else if(topic=="/radio/amp/status1"){
    if (messageString == "ON") {
      lb.setValue(1);
    } else {
      lb.setValue(0);
    }
  }
  else if(topic=="/radio/amp/status2"){
    if (messageString == "ON") {
      rb.setValue(1);
    } else {
      rb.setValue(0);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if(client.connect("Amplifier")) {
      client.subscribe("/radio/amp/temp");  
      client.subscribe("/radio/amp/humid");
      client.subscribe("/radio/amp/serialall"); 
      client.subscribe("/radio/amp/status1");             
      client.subscribe("/radio/amp/status2");    
    } 
    else {
      delay(5000);
    }
  }
}

void setup(void) {    
  nexInit();

  lb.attachPop(lbPopCallback, &lb);
  rb.attachPop(rbPopCallback, &rb);
      
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 

}

void loop(void){   
  nexLoop(nex_listen_list);
   
  if (!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect("Amplifier");
}

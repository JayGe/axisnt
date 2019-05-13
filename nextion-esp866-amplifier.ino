// Loading the required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Nextion.h"

int lastConnectionAttempt = millis();
int connectionDelay = 5000; // try to reconnect every 5 seconds

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "1.2.3.4";

WiFiClient espClient;
PubSubClient client(espClient);

NexText lo = NexText(0, 7, "lo");
NexText ro = NexText(0, 8, "ro");
NexText lr = NexText(0, 9, "lr");
NexText rr = NexText(0, 10, "rr");
NexText lv = NexText(0, 11, "lv");
NexText rv = NexText(0, 12, "rv");
NexText ta = NexText(0, 13, "ta");
NexText bv = NexText(0, 18, "bv");

NexText hc = NexText(0, 15, "hc");
NexText tc = NexText(0, 14, "tc");

NexButton lb = NexButton(0, 2, "lb");
NexButton rb = NexButton(0, 3, "rb");

//Register a button object to the touch event list  
NexTouch *nex_listen_list[] = {
  &lb,
  &rb,
  NULL
};

void lbPushCallback(void *ptr) {  
  client.publish("/radio/amp/button", "left");
}
void rbPushCallback(void *ptr) {
  client.publish("/radio/amp/button", "right"); 
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
        
    // We start by connecting to a WiFi network
    //Serial.println();
    //Serial.print("Connecting to ");
    //Serial.println(ssid);
    WiFi.begin(ssid, password);
    //while (WiFi.status() != WL_CONNECTED) {
    //  delay(500);
    //  Serial.print(".");
    //}
    }
  }
  
  //Serial.println("");
  //Serial.print("WiFi connected - ESP IP address: ");
  //Serial.println(WiFi.localIP());
}


void callback(String topic, byte* message, unsigned int length) {
  //Serial.print("Message arrived on topic: ");
  //Serial.print(topic);
  //Serial.print(". Message: ");
  String messageString;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageString += (char)message[i];
  }

  const char* messageC = messageString.c_str();
  
  //Serial.println(messageString);

  if(topic=="/radio/amp/temp"){
     //Serial.println("Temp came in");
     //Serial.println(messageString);
     tc.setText(messageC);
  }
  else if(topic=="/radio/amp/humid"){
     //Serial.println("Humidity came in");
     //Serial.println(messageString);
     hc.setText(messageC);
  }
  else if(topic=="/radio/amp/serialall"){
    //Serial.println(messageString);
    if (messageString.startsWith("$ADCS")){

      bv.setText(getValue(messageString, ',', 1).c_str());
      rv.setText(getValue(messageString, ',', 2).c_str());
      lv.setText(getValue(messageString, ',', 3).c_str());
      ta.setText(getValue(messageString, ',', 4).c_str());
      lo.setText(getValue(messageString, ',', 5).c_str());
      ro.setText(getValue(messageString, ',', 6).c_str());
      lr.setText(getValue(messageString, ',', 7).c_str());
      rr.setText(getValue(messageString, ',', 8).c_str());
      //Serial.println(s_init);
    }
  }
  else if(topic=="/radio/amp/status1"){
    Serial.print("AMP 1: ");
    Serial.println(messageString);
  }
  else if(topic=="/radio/amp/status2"){
    Serial.print("AMP 2: ");
    Serial.println(messageString);
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    if(client.connect("Amplifier")) {
      client.subscribe("/radio/amp/temp");  
      client.subscribe("/radio/amp/humid");
      client.subscribe("/radio/amp/serialall"); 
      client.subscribe("/radio/amp/status1");             
      client.subscribe("/radio/amp/status2");    
    } 
    else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //mySerial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void) {    
  //Set the baudrate which is for debug and communicate with Nextion screen
  nexInit();

  //Register the pop event callback function of the current button component
  lb.attachPush(lbPushCallback, &lb);
  rb.attachPush(rbPushCallback, &rb);
    
  //Serial.begin(115200);
  
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


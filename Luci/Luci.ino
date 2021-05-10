#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define LED_PIN 14   //D5
#define LAMP_PIN 2   //D4

#define LED_STATUS_PIN 12  //D6
#define LAMP_STATUS_PIN 13  //D7

#define WIFI_SSID  "NETGEAR34_EXT"
#define WIFI_PASSWORD  "cleverhippo630"

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVER_PORT 1883
#define AIO_USER "####" 
#define AIO_KEY "####" 

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVER_PORT, AIO_USER, AIO_KEY);

Adafruit_MQTT_Subscribe lampFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USER"/feeds/luci");

void MQTT_connect();
void setup() {
  //Serial.begin(9600);
  //Serial.println("Setup");
  pinMode(LED_PIN, OUTPUT);
  pinMode(LAMP_PIN, OUTPUT);
  pinMode(LED_STATUS_PIN, INPUT_PULLUP);
  pinMode(LAMP_STATUS_PIN, INPUT_PULLUP);
  digitalWrite(LAMP_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  //Serial.print("Indirizzo ip: "); 
  //Serial.println(WiFi.localIP());
  mqtt.subscribe(&lampFeed);
}

uint32_t x=0;

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while((subscription = mqtt.readSubscription(5000))){
    if(subscription == & lampFeed){
      String command = String((char *)lampFeed.lastread);
      if(command.equalsIgnoreCase("accendi") || command.equalsIgnoreCase("accendere")){
        //Serial.println("Accendo LED");
        toggleLed(true);
      }else if(command.equalsIgnoreCase("disattiva") || command.equalsIgnoreCase("disattivare")){
        toggleLed(false);
        //Serial.println("Spengo led");  
      }else if (command.equalsIgnoreCase("1")) {
        toggleLamp(true);
        //Serial.println("Accendo lampada");  
      }else if(command.equalsIgnoreCase("2")){
        toggleLamp(false);
        //Serial.println("Spengo lampada");  
      }
      //Serial.println(command);
      
    }
  }

  if(! mqtt.ping()){
    mqtt.disconnect();
  }
}


void toggleLed(bool state){
  if(state == digitalRead(LED_STATUS_PIN))
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

void toggleLamp(bool state){
  if(state == digitalRead(LAMP_STATUS_PIN))
  digitalWrite(LAMP_PIN, !digitalRead(LAMP_PIN));
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  //Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       //Serial.println(mqtt.connectErrorString(ret));
       //Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  //Serial.println("MQTT Connected!");
}

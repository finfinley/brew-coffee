#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

int waterLevel = 0;
int coffeePotActive = 0;

// Relay Info
#define relayPin 7

// Water Sensor Info
#define waterSensor A0

// Ultrasonic Info
#define trigPin 2
#define echoPin 0
long duration, cm, inches;

// Channel info from ThingsSpeak
unsigned long onOffChannelNumber = 000000;
const char * onOffWriteAPIKey = "AAAAAAAAAAAAAAAAA";

unsigned long waterChannel = 000000;
const char * waterWriteAPIKey = "AAAAAAAAAAAAAAAAA";

unsigned long sonarChannel = 000000;
const char * sonarWriteAPIKey = "AAAAAAAAAAAAAAAAA";


// WiFi Info

char ssid[] = "Wifiname";
char pass[] = "password";
WiFiClient client;

void setup() {
  // Pins for Ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // Pin for Relay
  pinMode(relayPin, OUTPUT);
    
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);

}

void wifiSetup(){
    // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.print(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
}


void brewCoffee(){
  digitalWrite(relayPin, HIGH);
}



void loop() {
  wifiSetup();
      
      // Water Sensor
   float adcValue = analogRead(waterSensor);
   Serial.print("Water level is at: ");
   Serial.print(waterSensor);
   if (adcValue < 95) {
    int httpCode = ThingSpeak.writeField(waterChannel, 1, 1, waterWriteAPIKey);
    if (httpCode == 200) {
      int waterLevel = 1;
      Serial.println("/nChannel write successful.");
    }
    else {
      Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
  }

   }
   else {
    int httpCode = ThingSpeak.writeField(waterChannel, 1, 0, waterWriteAPIKey);
        if (httpCode == 200) {
          int waterLevel = 0;
      Serial.println("Channel write successful.");
    }
    else {
      Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
   }

   }

  delay(10000);
  // Ultrasonic Sensor
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135

  if (inches > 165) {
    int httpCode = ThingSpeak.writeField(sonarChannel, 1, 1, sonarWriteAPIKey);
    if (httpCode == 200) {
      Serial.println("/nChannel write successful.");
      int coffeePotActive = 1;
    }
    else {
      Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
    }
  }
  else {
    int httpCode = ThingSpeak.writeField(sonarChannel, 1, 0, sonarWriteAPIKey);
        if (httpCode == 200) {
          Serial.println("/nChannel write successful.");
          int coffeePotActive = 0;
    }
        else {
          Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
    }
  }
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  if (waterLevel == 0 && coffeePotActive == 0){
    brewCoffee();
  }
  else {
    Serial.println("Error brewing coffee");
  }
  exit(0);
}

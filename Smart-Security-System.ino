/*
  Repeating WiFi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati

 http://www.arduino.cc/en/Tutorial/WifiWebClientRepeating
 This code is in the public domain.
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

const int pirPin = 2;
const int soundPin = 3;

// ... (rest of code unchanged)

// Initialize the WiFi client library
WiFiClient client;

// server address:
char server[] = "example.org";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
  pinMode(soundPin, INPUT);
  
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  bool motionDetected = digitalRead(pirPin) == HIGH;
  bool soundDetected = digitalRead(soundPin) == HIGH;

  if (motionDetected || soundDetected) {
    sendDataToServer(motionDetected, soundDetected);
    delay(60000);  // Wait 1 minute before sending
  }
}

void sendDataToServer(bool motion, bool sound) {
  WiFiClient client;
  
  StaticJsonDocument<200> doc;
  doc["motion"] = motion;
  doc["sound"] = sound;

  String jsonString;
  serializeJson(doc, jsonString);

  if (client.connect("192.168.1.100", 5000)) { //Change to your IP-adress
    client.println("POST /sensor HTTP/1.1");
    client.println("Host: 192.168.1.100"); //Change to your IP-adress
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.println(jsonString);
  } else {
    Serial.println("Connection failed");
  }
  client.stop();
}
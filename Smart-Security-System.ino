/*
  Repeating WiFi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati

 This code is in the public domain.

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-web-client-repeating
 */
#include <WiFiS3.h>

char ssid[] = "68fbef1";
char pass[] = "tas29qat21";

const int pirPin = 2;
const int soundPin = 4;

WiFiClient client;

char server[] = "192.168.0.40";  // Your server's IP address
int port = 5000;  // Your server's port

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("Starting setup...");
  
  pinMode(pirPin, INPUT);
  pinMode(soundPin, INPUT);
  
  connectToWiFi();
}

void loop() {
  bool motionDetected = digitalRead(pirPin) == HIGH;
  bool soundDetected = digitalRead(soundPin) == HIGH;

  sendDataToServer(motionDetected, soundDetected);
  delay(5000);  // Wait 1 minute before sending again

  // Check WiFi connection and reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }
}

void connectToWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    printWiFiStatus();
  } else {
    Serial.println("\nFailed to connect to WiFi. Restarting...");
    delay(3000);
    NVIC_SystemReset();  // Reset the board if it fails to connect
  }
}

void sendDataToServer(bool motion, bool sound) {
  Serial.println("Attempting to send data to server...");
  
  if (client.connect(server, port)) {
    String url = String("/sensor?motion=") + (motion ? "true" : "false") + "&sound=" + (sound ? "true" : "false");
    
    Serial.print("Sending request: ");
    Serial.println(url);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Server Timeout !");
        client.stop();
        return;
      }
    }
    
    // Read and print the response
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    
    Serial.println("\nData sent successfully");
  } else {
    Serial.println("Connection to server failed");
  }
  
  client.stop();
}

void printWiFiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
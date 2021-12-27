#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "esp-secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

String serverName = "http://192.168.1.10:3000/outputs";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

unsigned long lastValidResponse = 0;
unsigned long safetyDelay = 15000;

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    processRelayOutputs();
    
    lastTime = millis();
  }

  if ((millis() - lastValidResponse) > safetyDelay) {
    digitalWrite(D0, HIGH);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
  }
}

void processRelayOutputs () {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    
    return;
  }

  WiFiClient client;
  HTTPClient http;
  DynamicJsonDocument doc(200);

  if (!http.begin(client, serverName)) {
    Serial.printf("[HTTP} Unable to connect\n");

    return;
  }
   
  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();

  if (httpCode <= 0){
    http.end();
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

    return;
  } 
   
  Serial.printf("[HTTP] GET... code: %d\n", httpCode);

  if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
    String payload = http.getString();
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    lastValidResponse = millis();

    const bool firstOutputActive = doc["outputs"]["1"].as<bool>();
    const bool secondOutputActive = doc["outputs"]["2"].as<bool>();
    const bool thirdOutputActive = doc["outputs"]["3"].as<bool>();

    digitalWrite(D0,  firstOutputActive ? LOW : HIGH);
    digitalWrite(D1,  firstOutputActive ? LOW : HIGH);
    digitalWrite(D2,  firstOutputActive ? LOW : HIGH);
  }

  http.end();
}
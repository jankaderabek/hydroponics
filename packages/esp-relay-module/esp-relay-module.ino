#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "esp-secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

const int waterPumpOutput = D0;
const int mainLightOutput = D1;
const int seedLightOutput = D2;

const int RELAY_ENABLED = LOW;
const int RELAY_DISABLED = HIGH;

String serverName = "https://hydroponie-api.jankaderabek.eu/outputs";

unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

unsigned long lastValidResponse = 0;
unsigned long safetyDelay = 60000;

void setup() {
  pinMode(waterPumpOutput, OUTPUT);
  pinMode(mainLightOutput, OUTPUT);
  pinMode(seedLightOutput, OUTPUT);

  digitalWrite(waterPumpOutput, RELAY_DISABLED);
  digitalWrite(mainLightOutput, RELAY_DISABLED);
  digitalWrite(seedLightOutput, RELAY_DISABLED);

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
    digitalWrite(waterPumpOutput, RELAY_DISABLED);
    digitalWrite(mainLightOutput, RELAY_DISABLED);
    digitalWrite(seedLightOutput, RELAY_DISABLED);
  }
}

void processRelayOutputs () {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");

    return;
  }

  WiFiClientSecure client;
  client.setInsecure(); //the magic line, use with caution
  client.connect(serverName, 443);
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

    const bool firstOutputActive = doc["outputs"]["waterPump"].as<bool>();
    const bool secondOutputActive = doc["outputs"]["mainLight"].as<bool>();
    const bool thirdOutputActive = doc["outputs"]["seedsLight"].as<bool>();

    digitalWrite(waterPumpOutput, firstOutputActive ? RELAY_ENABLED : RELAY_DISABLED);
    digitalWrite(mainLightOutput, firstOutputActive ? RELAY_ENABLED : RELAY_DISABLED);
    digitalWrite(seedLightOutput, firstOutputActive ? RELAY_ENABLED : RELAY_DISABLED);
  }

  http.end();
}

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

const char* ssid = "xxx";
const char* password = "xxx";

String serverName = "http://192.168.1.10:3000/outputs";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

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

    const bool isLightRelayActive = doc["outputs"]["light_relay"].as<bool>();
    Serial.print("Is light relay active? ");
    Serial.println(isLightRelayActive);

    if (isLightRelayActive) {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  http.end();
}
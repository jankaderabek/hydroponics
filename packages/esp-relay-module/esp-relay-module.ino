#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

const char* ssid = "Vodafone-635A";
const char* password = "Z4wGTz4njw5bkNpu";

String serverName = "http://192.168.1.10:3000/outputs";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
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
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      if (http.begin(client, serverName)) {
        Serial.print("[HTTP] GET...\n");
        int httpCode = http.GET();

        if (httpCode > 0) {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();
            Serial.println(payload);

            JSONVar myObject = JSON.parse(payload);
  
            if (JSON.typeof(myObject) == "undefined") {
              Serial.println("Parsing input failed!");
              return;
            }
          
            Serial.print("JSON object = ");
            Serial.println(myObject);
          
            JSONVar keys = myObject.keys();
          
            for (int i = 0; i < keys.length(); i++) {
              JSONVar value = myObject[keys[i]];
              Serial.print(keys[i]);
              Serial.print(" = ");
              Serial.println(value);
            }
          }
        } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
      } else {
        Serial.printf("[HTTP} Unable to connect\n");
      }
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    
    lastTime = millis();
  }
}
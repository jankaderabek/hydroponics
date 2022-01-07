#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ESP8266mDNS.h>
#include "esp-secrets.h"
#include "ThingSpeak.h"

unsigned long thingSpeakChannelNumber = SECRET_CH_ID;
const char * thingSpeakWriteAPIKey = SECRET_WRITE_APIKEY;

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

AsyncWebServer webServer(80);
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 3600);

const int waterPumpOutput = D0;
const int mainLightOutput = D1;
const int seedLightOutput = D2;

bool lightsEnabled = false;
bool seedsLightsEnabled = false;
bool waterPumpEnabled = false;

bool lightShouldBeEnabled = false;
bool waterPumpShouldBeEnabled = false;

const int RELAY_ENABLED = LOW;
const int RELAY_DISABLED = HIGH;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

unsigned long lastThingSpeakUpdate = 0;
unsigned long thingSpeakUpdateDelay = 30000;

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

  timeClient.begin();
  timeClient.update();

  setTime(getNtpTime());
  setSyncProvider(getNtpTime);
  setSyncInterval(30);

  ThingSpeak.begin(client);

  if (!MDNS.begin("hydroponics-relay-control")) {
    Serial.println("Error setting up MDNS responder!");
  }

  MDNS.addService("http", "tcp", 80);

  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
  });

  webServer.on("/outputs", HTTP_GET, [](AsyncWebServerRequest *request){
    sendStatusResponse(request);
  });

  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler(
    "/outputs",
    [](AsyncWebServerRequest* request, JsonVariant& json) {
      if (not json.is<JsonObject>()) {
        request->send(400, "text/plain", "Not an object");
        return;
      }

      auto&& data = json.as<JsonObject>();

      if (data["waterPump"].is<bool>()) {
        waterPumpEnabled = data["waterPump"];
      }

      if (data["mainLights"].is<bool>()) {
        lightsEnabled = data["mainLights"];
      }

      if (data["seedsLights"].is<bool>()) {
        seedsLightsEnabled = data["seedsLights"];
      }

      processRelayOutputs();
      sendStatusResponse(request);      
    }
  );

  handler->setMethod(HTTP_PATCH);
  webServer.addHandler(handler);
  
  webServer.onNotFound(notFound);
  webServer.begin();
}

void loop() {
  MDNS.update();

  if ((millis() - lastTime) > timerDelay) {
    timeClient.update();
    Serial.printf("NTP time: %s\n", timeClient.getFormattedTime());

    processRelayOutputs();

    lastTime = millis();
  }


  if ((millis() - lastThingSpeakUpdate) > thingSpeakUpdateDelay) {
    lastThingSpeakUpdate = millis();
    updateThingSpeak();
  }
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendStatusResponse (AsyncWebServerRequest* request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  DynamicJsonDocument doc(1024);

  doc["waterPump"] = waterPumpEnabled;
  doc["mainLights"] = lightsEnabled;
  doc["seedsLights"] = seedsLightsEnabled;

  serializeJson(doc, *response);
  request->send(response);
}

time_t getNtpTime()
{
  return timeClient.getEpochTime();
}

void processRelayOutputs () {
  const int currentHours = hour();
  const int currentMinutes = minute();
  const bool lightsEnabledBefore = lightShouldBeEnabled;
  const bool waterPumpEnabledBefore = waterPumpShouldBeEnabled;

  lightShouldBeEnabled = shouldEnableLights(currentHours);
  waterPumpShouldBeEnabled = shouldEnableWaterPump(currentHours, currentMinutes);

  if (lightShouldBeEnabled != lightsEnabledBefore) {
    lightsEnabled = lightShouldBeEnabled;
    seedsLightsEnabled = lightShouldBeEnabled;
  }

  if (waterPumpShouldBeEnabled != waterPumpEnabledBefore) {
    waterPumpEnabled = waterPumpShouldBeEnabled;
  }

  Serial.printf("Current time is %d:%d\n", hour(), minute());
  Serial.printf("Lights enabled: %d\n", lightsEnabled);
  Serial.printf("Water pump enabled %d\n", waterPumpEnabled);

  digitalWrite(seedLightOutput, lightsEnabled ? RELAY_ENABLED : RELAY_DISABLED);
  digitalWrite(mainLightOutput, seedsLightsEnabled ? RELAY_ENABLED : RELAY_DISABLED);
  digitalWrite(waterPumpOutput, waterPumpEnabled ? RELAY_ENABLED : RELAY_DISABLED);

  if (lightsEnabledBefore != lightShouldBeEnabled || waterPumpEnabledBefore != waterPumpShouldBeEnabled) {
    updateThingSpeak();
  }
}

bool shouldEnableLights (int currentHours) {
  return currentHours >= 7 && currentHours < 19;
}

bool shouldEnableWaterPump (int currentHours, int currentMinutes) {
  if ((currentHours < 6) || (currentHours > 22)) {
    return false;
  }

  return currentMinutes >= 0 && currentMinutes <= 10;
}

void updateThingSpeak () {
  ThingSpeak.setField(1, lightsEnabled);
  ThingSpeak.setField(2, waterPumpEnabled);

  ThingSpeak.writeFields(thingSpeakChannelNumber, thingSpeakWriteAPIKey);
  Serial.println("Thingspeak updated");
}

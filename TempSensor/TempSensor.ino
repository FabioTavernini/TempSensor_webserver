#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "webpage.h"

#include "config.h"

// Data wire is plugged into port 4 on the ESP32
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(100);

  sensors.begin();              

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  sensors.requestTemperatures();
  float tempSensor1 = sensors.getTempCByIndex(0); // Gets the value of the temperature
  String html = SendHTML(tempSensor1);
  server.send(200, "text/html", html); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float tempSensor1){
  String html = String(webpage);
  html.replace("%TEMP%", String(tempSensor1));
  return html;
}

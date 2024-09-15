#include <WiFi.h>        // Use <WiFi.h> if you use ESP32
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>        // For JSON data
#include "config.h"

// Data wire is connected to pin 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

// Variable to store temperature
float temperatureC = 0.0;

// Create a route for sending temperature data in JSON format
String getTemperatureJSON() {
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperatureC;
  String json;
  serializeJson(doc, json);
  return json;
}

void setup() {
  // Start Serial communication
  Serial.begin(115200);

  // Start the DS18B20 sensor
  sensors.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi!");
  Serial.println(WiFi.localIP());

  // Serve the HTML page with the chart
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getHTMLPage());
  });

  // Serve the temperature data as JSON
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getTemperatureJSON());
  });

  // Start server
  server.begin();
}

void loop() {
  // Request temperature readings from sensor
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);

  // Delay before the next reading
  delay(2000);
}

// Function to send the HTML page with Chart.js integration
String getHTMLPage() {
  return R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Temperature Monitor</title>
      <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
      <style>#tempChart{max-height:80vh;max-width:90vw; }</style>
    </head>
    <body>
      <h1>Current Temp:</h1><br>
      <a id="currentTemp"></a>
      <h1>Temperature Over Time</h1>
      <canvas id="tempChart" width="400" height="200"></canvas>

      <script>
        const ctx = document.getElementById('tempChart').getContext('2d');
        const tempChart = new Chart(ctx, {
          type: 'line',
          data: {
            labels: [],
            datasets: [{
              label: 'Temperature',
              data: [],
              borderColor: 'rgba(75, 192, 192, 1)',
              borderWidth: 2,
              fill: false
            }]
          },
          options: {
            scales: {
              x: {
                title: {
                  display: true,
                  text: 'Time (s)'
                }
              },
              y: {
                title: {
                  display: true,
                  text: 'Temperature (°C)'
                }
              }
            }
          }
        });

        // Function to fetch temperature data
        async function fetchTemperature() {
          const response = await fetch('/temperature');
          const data = await response.json();
          const time = Math.floor(Date.now() / 1000 / 60);  // Get timestamp in seconds
          
          document.getElementById("currentTemp").innerHTML = data.temperature;

          // Add new data point to the chart
          tempChart.data.labels.push(time);
          tempChart.data.datasets[0].data.push(data.temperature);
          
          // Keep the last 20 points
        if (tempChart.data.labels.length > 20) {  
            tempChart.data.labels.shift();
            tempChart.data.datasets[0].data.shift();
          }

          tempChart.update();
        }

        // Fetch temperature data every 2 seconds
        setInterval(fetchTemperature, 2000);
      </script>
    </body>
    </html>
  )rawliteral";
}
    
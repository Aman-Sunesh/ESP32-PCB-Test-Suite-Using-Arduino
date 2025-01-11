#include <Wire.h>
#include "DHT.h"
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

#define DHTTYPE 22

// Network credentials
const char *ssid = "Wokwi-GUEST";
const char *password = "";

const int DHTPin = 3; // Pin for the voltage sensor
const int LED = 9;

float temperature_Celsius;
float temperature_Fahrenheit;
float temperature_Kelvin;
float Humidity;

const int voltageSensorPin = 0; // Pin for the voltage sensor
float sensorReading;            // Analog reading from the voltage sensor
const float maxAdcCount = 4095; // Maximum ADC count (assuming 12-bit ADC)
float voltageValue;             // Calculated voltage based on sensor reading
const float lowVoltageThreshold = 3.0; // Threshold for low voltage

unsigned long lastTime = 0;

AsyncWebServer server(80);
AsyncEventSource events("/events");

DHT DHTObject(DHTPin, DHTTYPE);

void getDHTReadings() {
  Humidity = DHTObject.readHumidity();
  
  // Read temperature as Celsius (the default)
  temperature_Celsius = DHTObject.readTemperature();
  
  // Read temperature as Fahrenheit (isFahrenheit = true)
  temperature_Fahrenheit = DHTObject.readTemperature(true);
  
  temperature_Kelvin = temperature_Celsius + 273.15;
}

void getVoltageReadings() {
  // Read analog voltage from the voltage sensor
  sensorReading = analogRead(voltageSensorPin);
  
  // Convert analog reading to voltage value
  voltageValue = (sensorReading / maxAdcCount) * 3.3 * 2;
}

void reactToLowVoltage() {
  Serial.println("Alert! Low Voltage");
  
  // Send the low voltage event to the website
  events.send("Low Voltage Alert", "low_voltage", millis());
  
  delay(1000);
  Serial.flush();
  esp_deep_sleep_start();
}

void reactToTemp(float temperature) {
  if (temperature < -10) {
    Serial.println("Alert! Low Temperature");
    
    // Send the low temperature event to the website
    events.send("Low Temperature Alert", "low_temperature", millis());
    
    delay(1000);
    Serial.flush();
    
    esp_deep_sleep_start();
  }
  else if (temperature > 45) {
    Serial.println("Alert! High Temperature");
    
    // Send the high temperature event to the website
    events.send("High Temperature Alert", "high_temperature", millis());
    
    delay(1000);
    Serial.flush();
    
    esp_deep_sleep_start();
  }
}

void reactToHumidity(float humidity) {
  if (humidity > 75) 
  {
    Serial.println("Alert! High Humidity");
    
    // Send the high humidity event to the website
    events.send("High Humidity Alert", "high_humidity", millis());
    
    delay(1000);
    Serial.flush();
    
    esp_deep_sleep_start();
  }
}

void connectToWiFi(const char *ssid, const char *password) {
  int attempts = 0;

  // Attempt to connect to WiFi for a maximum of 10 times or until connected
  do {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    attempts++;
    delay(5000);
  } while (attempts < 10 && WiFi.status() != WL_CONNECTED);

  // Check if ESP32 is connected to WiFi
  if (WiFi.status() == WL_CONNECTED) 
  {
    // Check if ESP32 is connected to WiFi
    Serial.println();
    Serial.println("WiFi Status: Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
  else 
  {
    Serial.println();
    Serial.println("WiFi Status: Failed to connect to WiFi. Check credentials and try again.");
    
    delay(1000);
    Serial.flush();
    
    esp_deep_sleep_start();
  }
}

void scanNetworks() {
  Serial.println("Scanning...");
  
  // Try scanning and connecting to the specified SSID for a maximum of 10 times
  for (int attempt = 0; attempt < 10; attempt++) {
    int n = WiFi.scanNetworks();
    Serial.println();

    if (n == 0) 
    {
      Serial.println("No networks found.");
    }
    else 
    {
      Serial.print(n);
      Serial.println(" networks found...");

      // Loop through the found networks
      for (int i = 0; i < n; i++) 
      {
        // Check if the specified SSID is found
        if (WiFi.SSID(i) == ssid) 
        {
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
          // Connect to WiFi
          connectToWiFi(ssid, password);
          // Stop scanning after connecting to the specified SSID
          return;
        }
        else 
        {
          Serial.print(ssid);
          Serial.println(" ,not found!");
        }
      }
    }
  }

  // Specified SSID not found after 10 attempts
  Serial.println();
  Serial.print(ssid);
  Serial.println(" not found after 10 attempts!");
}

void printWifiStatus() {
  // Print the SSID (Service Set Identifier) of the connected WiFi network
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Get and print the local IP address assigned to the ESP32
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Get and print the received signal strength indication (RSSI) of the WiFi connection
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String processor(const String& var) {
  // Reserve memory to prevent fragmentation
  String result;
  result.reserve(50);

  if (var == "TEMPERATURE_C" || var == "TEMPERATURE_F" || var == "TEMPERATURE_K") {
    getDHTReadings();
  }
  else if (var == "HUMIDITY") {
    getDHTReadings();
  }
  else if (var == "VOLTAGE") {
    getVoltageReadings();
  }
  else {
    return "Unknown variable";
  }

  // Check for temperature and humidity conditions
  if (temperature_Celsius < -10) {
    return "Alert! Low Temperature";
  }
  else if (temperature_Celsius > 45) {
    return "Alert! High Temperature";
  }
  else if (Humidity > 75) {
    return "Alert! High Humidity";
  }
  else if (voltageValue < lowVoltageThreshold) {
    return "Alert! Low Voltage";
  }

  // Serial.println(var);
  if (var == "TEMPERATURE_C") {
    return String(temperature_Celsius);
  }
  else if (var == "TEMPERATURE_F") {
    return String(temperature_Fahrenheit);
  }
  else if (var == "TEMPERATURE_K") {
    return String(temperature_Kelvin);
  }
  else if (var == "HUMIDITY") {
    return String(Humidity);
  }
  else if (var == "VOLTAGE") {
    return String(voltageValue);
  }

  return result;
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>DHT Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
    integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG
9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body { margin: 0;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem;
             grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.temperature { color: #0e7c7b; }
    .card.humidity { color: #17bebb; }
    .card.voltage { color: #0000CD; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>DHT WEB SERVER</h3>
  </div>
  <div id="alertMessage" style="background-color: white; padding: 10px;"></div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span
          class="reading"><span id="temp_celcius">%TEMPERATURE_C%</span> &deg;C</span></p>
      </div>
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span
          class="reading"><span id="temp_fahrenheit">%TEMPERATURE_F%</span>
          &deg;F</span></p>
      </div>
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span
          class="reading"><span id="temp_kelvin">%TEMPERATURE_K%</span> K</span></p>
      </div>
      <div class="card humidity">
        <h4><i class="fas fa-tint"></i> HUMIDITY</h4><p><span class="reading"><span
          id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card voltage">
        <h4><i class="fas fa-bolt"></i> VOLTAGE</h4>
        <p><span class="reading"><span id="voltage">%VOLTAGE%</span> V</span></p>
      </div>
    </div>
  </div>
  <script>
    if (!!window.EventSource) {
      var source = new EventSource('/events');
      source.addEventListener('open', function(e) {
        console.log("Events Connected");
      }, false);
      source.addEventListener('error', function(e) {
        if (e.target.readyState != EventSource.OPEN) {
          console.log("Events Disconnected");
        }
      }, false);
      source.addEventListener('message', function(e) {
        console.log("message", e.data);
      }, false);

      source.addEventListener('temperature_Celsius', function(e) {
        console.log("temperature", e.data);
        document.getElementById("temp_celcius").innerHTML = e.data;
        // Check for low and high temperature events
        if (parseFloat(e.data) < -10 || parseFloat(e.data) > 45) {
          alert("Temperature Alert");
          // Update the background color of the "alertMessage" div
          document.getElementById("alertMessage").style.backgroundColor = "orange";
          document.getElementById("alertMessage").innerHTML = "Temperature Alert";
        }
      }, false);

      source.addEventListener('temperature_Fahrenheit', function(e) {
        console.log("temperature", e.data);
        document.getElementById("temp_fahrenheit").innerHTML = e.data;
      }, false);

      source.addEventListener('temperature_Kelvin', function(e) {
        console.log("temperature", e.data);
        document.getElementById("temp_kelvin").innerHTML = e.data;
      }, false);

      source.addEventListener('humidity', function(e) {
        console.log("humidity", e.data);
        document.getElementById("hum").innerHTML = e.data;
        // Check for high humidity event
        if (parseFloat(e.data) > 75) {
          alert("Alert! High Humidity");
          // Update the background color of the "alertMessage" div
          document.getElementById("alertMessage").style.backgroundColor = "red";
          document.getElementById("alertMessage").innerHTML = "Alert! High Humidity";
        }
      }, false);

      source.addEventListener('voltage', function(e) {
        console.log("voltage", e.data);
        document.getElementById("voltage").innerHTML = e.data;
        // Check for low voltage event
        if (parseFloat(e.data) < 3.0) {
          alert("Alert! Low Voltage");
          // Update the background color of the "alertMessage" div
          document.getElementById("alertMessage").style.backgroundColor = "red";
          document.getElementById("alertMessage").innerHTML = "Alert! Low Voltage";
        }
      }, false);
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32-C3!");

  // Set the pinMode for the voltage sensor pin to INPUT
  pinMode(voltageSensorPin, INPUT);

  // Set the pinMode for the LED pin to OUTPUT
  pinMode(LED, OUTPUT);

  DHTObject.begin();
  Serial.println("DHT Initialized");

  scanNetworks();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });

  server.addHandler(&events);
  server.begin();
}

void loop() {
  digitalWrite(LED, LOW);

  getDHTReadings();
  getVoltageReadings();

  Serial.println("------------------------------------------------");
  Serial.printf("Temperature = %.2f ºC \n", temperature_Celsius);
  Serial.printf("Temperature = %.2f ºF \n", temperature_Fahrenheit);
  Serial.printf("Temperature = %.2f K \n", temperature_Kelvin);
  Serial.printf("Humidity= %.2f %\n", Humidity);
  Serial.printf("Voltage = %.2f V\n", voltageValue);

  if (voltageValue < lowVoltageThreshold) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(100);
    }
    reactToLowVoltage();
  }

  if (temperature_Celsius < -10 || temperature_Celsius > 45) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(100);
    }
    reactToTemp(temperature_Celsius);
  }

  if (Humidity > 75) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(100);
    }
    reactToHumidity(Humidity);
  }

  Serial.println();
  Serial.println("------------------------------------------------");

  // Send Events to the Web Server with the Sensor Readings
  events.send("ping", NULL, millis());
  events.send(String(temperature_Celsius).c_str(), "temperature_Celsius", millis());
  events.send(String(temperature_Fahrenheit).c_str(), "temperature_Fahrenheit", millis());
  events.send(String(temperature_Kelvin).c_str(), "temperature_Kelvin", millis());
  events.send(String(Humidity).c_str(), "humidity", millis());
  events.send(String(voltageValue).c_str(), "voltage", millis());

  lastTime = millis();
  delay(300);
}

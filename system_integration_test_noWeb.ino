#include <Wire.h>
#include "DHT.h"
#include <WiFi.h> // We keep WiFi for scanning & connecting, if needed.

#define DHTTYPE 22

// Network credentials
const char *ssid = "Wokwi-GUEST";
const char *password = "";

const int DHTPin = 3;  // Pin for the DHT sensor
const int LED = 9;

// Temperature & Humidity
float temperature_Celsius;
float temperature_Fahrenheit;
float temperature_Kelvin;
float Humidity;

// Voltage Sensor
const int voltageSensorPin = 0;
float sensorReading;
const float maxAdcCount       = 4095;
float voltageValue;
const float lowVoltageThreshold = 3.0;

unsigned long lastTime = 0;

// Create a DHT object
DHT DHTObject(DHTPin, DHTTYPE);

// -----------------------------------------------------------------
// SENSOR READING FUNCTIONS
// -----------------------------------------------------------------
void getDHTReadings() 
{
  Humidity = DHTObject.readHumidity();
  temperature_Celsius    = DHTObject.readTemperature();
  temperature_Fahrenheit = DHTObject.readTemperature(true);
  temperature_Kelvin     = temperature_Celsius + 273.15;
}

void getVoltageReadings() 
{
  sensorReading = analogRead(voltageSensorPin);
  voltageValue  = (sensorReading / maxAdcCount) * 3.3 * 2;
}

// -----------------------------------------------------------------
// REACTION FUNCTIONS (ON ERROR CONDITIONS)
// -----------------------------------------------------------------
void reactToLowVoltage() 
{
  Serial.println("Alert! Low Voltage");
  delay(1000);
  Serial.flush();

  esp_deep_sleep_start();
}

void reactToTemp(float temperature) 
{
  if (temperature < -10) {
    Serial.println("Alert! Low Temperature");
    delay(1000);
    Serial.flush();
    esp_deep_sleep_start();
  } 

  else if (temperature > 45) 
  {
    Serial.println("Alert! High Temperature");
    delay(1000);
    Serial.flush();
    esp_deep_sleep_start();
  }
}

void reactToHumidity(float humidity) 
{
  if (humidity > 75) {
    Serial.println("Alert! High Humidity");
    delay(1000);
    Serial.flush();
    esp_deep_sleep_start();
  }
}

// -----------------------------------------------------------------
// WIFI CONNECTION FUNCTIONS
// -----------------------------------------------------------------
void connectToWiFi(const char *ssid, const char *password) 
{
  int attempts = 0;
  do {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    attempts++;
    delay(5000);
  } while (attempts < 10 && WiFi.status() != WL_CONNECTED);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi Status: Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  } else {
    Serial.println();
    Serial.println("WiFi Status: Failed to connect to WiFi. Check credentials and try again.");
    delay(1000);
    Serial.flush();
    esp_deep_sleep_start();
  }
}

void scanNetworks() 
{
  Serial.println("Scanning...");
  for (int attempt = 0; attempt < 10; attempt++) {
    int n = WiFi.scanNetworks();
    Serial.println();

    if (n == 0) {
      Serial.println("No networks found.");
    } 
    else {
      Serial.print(n);
      Serial.println(" networks found...");

      for (int i = 0; i < n; i++) {
        if (WiFi.SSID(i) == ssid) {
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
          connectToWiFi(ssid, password);
          return;
        } 
        else {
          Serial.print(ssid);
          Serial.println(" ,not found!");
        }
      }
    }
  }
  Serial.println();
  Serial.print(ssid);
  Serial.println(" not found after 10 attempts!");
}

void printWifiStatus() 
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();

  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// -----------------------------------------------------------------
// SETUP AND LOOP
// -----------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  Serial.println("Hello, ESP32-C3!");

  pinMode(voltageSensorPin, INPUT);
  pinMode(LED, OUTPUT);

  DHTObject.begin();
  Serial.println("DHT Initialized");

  scanNetworks();
}

void loop() 
{
  digitalWrite(LED, LOW);

  getDHTReadings();
  getVoltageReadings();

  Serial.println("---------------------------------------------");
  Serial.printf("Temperature = %.2f ºC \n", temperature_Celsius);
  Serial.printf("Temperature = %.2f ºF \n", temperature_Fahrenheit);
  Serial.printf("Temperature = %.2f K \n", temperature_Kelvin);
  Serial.printf("Humidity= %.2f %%\n", Humidity);
  Serial.printf("Voltage = %.2f V\n", voltageValue);

  if (voltageValue < lowVoltageThreshold) 
  {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(100);
    }
    reactToLowVoltage();
  }

  if (temperature_Celsius < -10 || temperature_Celsius > 45) 
  {
    for (int i = 0; i < 3; i++) 
    {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(100);
    }
    reactToTemp(temperature_Celsius);
  }

  if (Humidity > 75) 
  {
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(100);
    }
    reactToHumidity(Humidity);
  }

  Serial.println();
  Serial.println("---------------------------------------------");

  lastTime = millis();
  delay(300);
}

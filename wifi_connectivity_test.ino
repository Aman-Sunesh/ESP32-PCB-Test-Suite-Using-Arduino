#include "WiFi.h"

// Network credentials
const char *ssid = "";       // Enter your wifi network name
const char *password = "";   // Enter password

void connectToWiFi(const char *ssid, const char *password)
{
  int attempts = 0;

  // Attempt to connect to WiFi for a maximum of 10 times or until connected
  do {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station...");
    attempts++;
  } while (attempts < 10 && WiFi.status() != WL_CONNECTED);

  // Check if ESP32 is connected to WiFi
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
  else
  {
    Serial.println("Failed to connect to WiFi. Check credentials and try again.");
  }
}


void scanNetworks()
{
  Serial.println("Scanning...");

  // Try scanning and connecting to the specified SSID for a maximum of 10 times
  for (int attempt = 0; attempt < 10; attempt++)
  {
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


void printWifiStatus()
{
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


void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing WiFi...");
  // Scan for available networks
  scanNetworks();
}


void loop()
{
  // Empty loop as no continuous processing is required
}

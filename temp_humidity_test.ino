 #include "DHT.h"

 #define DHTTYPE 22

 //Pin for the voltage sensor
 const int DHTPin = 3;
 float temperature;
 float humidity;

 //Create a DHT object with the specified pin and sensor type
 DHT DHTObject(DHTPin, DHTTYPE);

 void reactToTemp(float temperature)
 {
   if (temperature <-10)
   {
     Serial.println("Alert! Low Temperature");
   }
  
  else if (temperature > 45)
   {
     Serial.println("Alert! High Temperature");
   }
 }

 void reactToHumidity(float humidity)
 {
   if (humidity > 75)
   {
     Serial.println("Alert! High Humidity");
   }
 }

 void setup()
 {
   Serial.begin(115200);
   Serial.println("Hello, ESP32-C3!");
   
   DHTObject.begin();
   Serial.println("DHT Initialized");
 }

 void loop()
 {
   Serial.println("--------------------------------------");
     
   //Read temperature and humidity from the DHT sensor
   humidity = DHTObject.readHumidity();
   temperature = DHTObject.readTemperature();
     
   Serial.print("Temperature: ");
   Serial.print(temperature);
   Serial.println(" °C");
     
   Serial.print("Humidity: ");
   Serial.print(humidity);
   Serial.println(" %");
     
   //Check for temperature out of range and react accordingly
   if (temperature <-10 || temperature > 45)
   {
     reactToTemp(temperature);
   }
  
   //Check for high humidity and react accordingly
   if (humidity > 75)
   {
     reactToHumidity(humidity);
   }
   
   Serial.println("--------------------------------------");
   
   Serial.println();
   Serial.println();
   
   delay(500);
 }

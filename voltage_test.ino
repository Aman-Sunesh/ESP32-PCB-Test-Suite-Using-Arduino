 const int voltageSensorPin = 0; //Pin for the voltage sensor
 float sensorReading; //Analog reading from the voltage sensor
 const float maxAdcCount = 4095; //Maximum ADC count (assuming 12-bit ADC)
 float voltageValue; //Calculated voltage based on sensor reading
 const float lowVoltageThreshold = 3.0; //Threshold for low voltage

 void reactToLowVoltage()
 {
   Serial.println("Alert! Low Voltage");
 }

 void setup()
 {
   Serial.begin(115200);
   Serial.println("Hello, ESP32-C3!");
     
   //Set the pinMode for the voltage sensor pin to INPUT
   pinMode(voltageSensorPin, INPUT);
 }

 void loop()
 {
   //Read analog voltage from the voltage sensor
   sensorReading = analogRead(voltageSensorPin);
     
   //Convert analog reading to voltage value
   voltageValue = (sensorReading / maxAdcCount) * 3.3 * 2;
     
   //Check if the voltage is below the low voltage threshold
   if (voltageValue < lowVoltageThreshold)
   {
   reactToLowVoltage();
   }
     
   else
   {
      Serial.println(voltageValue);
   }
  
   delay(500);
 }

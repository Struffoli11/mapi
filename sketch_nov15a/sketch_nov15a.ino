
#include <I2Cdev.h> //I2C library
#include <Wire.h> //
#include <Arduino.h>

#define SDA A4 //Arduino UNO settings
#define CLK A5
#define Vout A0
#define FS2012_ADDRESS 0x07 // see datasheet 

uint16_t data = 0; //digital data from FS2012
float flow; //flow after convertion
// Variables
unsigned long delayMs = 500;

void digital_flow_sensor(){
    Wire.beginTransmission(FS2012_ADDRESS); // "Hey, SENSOR @ Address! Message for you"
    Wire.write(1);  // send a bit asking for register one, the data register (as specified by the pdf)
    Wire.endTransmission(); // "Thanks, goodbye..."
    Wire.requestFrom(FS2012_ADDRESS, 2);
    while(Wire.available() == 0);
    byte a = Wire.read(); // first received byte stored here ....Example bytes one: 00011001 10000000
    //delay(5);
    byte b = Wire.read(); // second received byte stored here ....Example bytes two: 11100111 00000000
    //delay(5);
	  data = (a << 8) | b;
    flow = data / 1000.0; // (Standard Liters per minute)
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(); 
  delay(500);
}

void loop() { 
  if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n'); // Read the command until newline
        serialEvent(command); // Process the received command
    }
  digital_flow_sensor(); //Renesas FS2012
  delay(delayMs);
  
  Serial.println(flow, 5); //print flow measured from FS2012
} 

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.

 Receives a new sampling frequency from the serial interface. Hence, a new delay is set between each loop exectuion.
 */
void serialEvent(String command) {
    command.trim(); // Remove any trailing or leading spaces or newline characters

    // Command to set sampling frequency: "SF:<frequency>"
    if (command.startsWith("SF:")) {
        String freqStr = command.substring(3); // Extract the frequency part
        float newFreq = freqStr.toFloat(); // Convert to float

        if (newFreq > 0) { // Ensure frequency is positive
            delayMs = (unsigned long)(1000.0 / newFreq); // Calculate delay in ms
        }
    }
}



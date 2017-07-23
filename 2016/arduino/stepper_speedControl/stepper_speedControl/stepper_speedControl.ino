
/*
 Stepper Motor Control - speed control

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.
 A potentiometer is connected to analog input 0.

 The motor will rotate in a clockwise direction. The higher the potentiometer value,
 the faster the motor speed. Because setSpeed() sets the delay between steps,
 you may notice the motor is less responsive to changes in the sensor value at
 low speeds.

 Created 30 Nov. 2009
 Modified 28 Oct 2010
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
int stepCount = 0; 
String inString = "";
 int sensorReading = 0;

 
void setup() {
  Serial.begin(9600);
 
}

void loop() {


  if (Serial.available() > 0)
  {
    char inChar = Serial.read();
    

    if (isDigit(inChar))
    {
      inString += (char)inChar;
     }

     if (inChar == ';')
     {

     sensorReading = inString.toInt();
      inString = "";

     }
   }
  
  int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
  
  if (motorSpeed > 0) {
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepsPerRevolution / 100);
  }
}



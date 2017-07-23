/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
float val_;

void setup(){
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(115200);
}

float map_float(int value, int min_, int max_, float min_o, float max_o)
  {
  return (float) (value - min_) * (max_o - min_o) / (max_ - min_) + min_o;
  }

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val_ = map_float(val, 0, 1023, 900, 2100);     // scale it to use it with the servo (value between 0 and 180)
  Serial.print("val: ");
  Serial.print(val);
  Serial.print("    duty: ");
  Serial.println(val_);
  myservo.writeMicroseconds(val_);                  // sets the servo position according to the scaled value
  delay(2);                           // waits for the servo to get there
}



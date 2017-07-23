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
int led = 13;

void setup(){
  pinMode(led, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(115200);

  digitalWrite(led, HIGH);
  val = 2100;
  myservo.writeMicroseconds((val));
  delay(2000);
  digitalWrite(led, LOW);

  val = 900;
  myservo.writeMicroseconds((val));
  delay(2000);
  digitalWrite(led, HIGH);

  val = 1500;
  myservo.writeMicroseconds((val));
  digitalWrite(led, LOW);
}


void loop() {
}



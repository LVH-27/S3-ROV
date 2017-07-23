/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;
Servo myservo3;

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
float val_;
int i;
int flag;

void setup(){
  myservo1.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(10);  // attaches the servo on pin 10 to the servo object
  myservo3.attach(11);  // attaches the servo on pin 11 to the servo object
  flag=1;
  Serial.begin(115200);
  myservo1.writeMicroseconds(1500);
  delay(2000);
}

float map_float(int value, int min_, int max_, float min_o, float max_o)
  {
  return (float) (value - min_) * (max_o - min_o) / (max_ - min_) + min_o;
  }

void loop() {
  //val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  //val_ = map_float(val, 0, 1023, 900, 2200);     // scale it to use it with the servo (value between 0 and 180)
  //Serial.print("val: ");
  //Serial.print(val);
  //Serial.print("    duty: ");
  //Serial.println((int)val_);
  //myservo1.writeMicroseconds((int)val_);                  // sets the servo position according to the scaled value
  
    myservo1.writeMicroseconds(1400);  
    myservo2.writeMicroseconds(1100);
    myservo3.writeMicroseconds(1900);

  delay(1000);                           // waits for the servo to get there  
    myservo1.writeMicroseconds(1500);  
    myservo2.writeMicroseconds(1400);
    myservo3.writeMicroseconds(1400);
  
  delay(1000);
  
                             // waits for the servo to get there  
    myservo1.writeMicroseconds(1600);  
    myservo2.writeMicroseconds(1900);
    myservo3.writeMicroseconds(1100);
delay(1000);

myservo1.writeMicroseconds(1500);  
    myservo2.writeMicroseconds(1400);
    myservo3.writeMicroseconds(1400);
  
  delay(1000);
}




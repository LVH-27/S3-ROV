
#include <TimerOne.h>


#define directPin 10
#define inputPin 9
#define enablePin 11

int period = 10000;
char numDir;


void setup() 
{
  Timer1.initialize(period);
  Timer1.attachInterrupt(callback);
  Serial.begin(9600);
  pinMode(directPin, OUTPUT);
  pinMode(inputPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);

 
 }

void callback()
{
  if(digitalRead(inputPin) == LOW){
    digitalWrite(inputPin, HIGH);
  }
  else {
    digitalWrite(inputPin, LOW);
  }
}

void loop() 
{ 
  if (Serial.available()) {
    
  
    numDir = Serial.read();
    
   digitalWrite(enablePin, LOW);
 
    if (numDir == 'b'){
    digitalWrite(directPin, HIGH);
    }

    if (numDir == 'a'){
    digitalWrite(directPin, LOW);
    }
  }
 
}
 

  
  
  
 


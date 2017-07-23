#include <SPI.h>
#include <stdint.h>

#define RDY_PIN 5 
#define START 255
#define LED 6
#define MOTOR_L 7
#define MOTOR_R 8
#define MOTOR_U 9

unsigned long timer;
uint8_t data[4];
unsigned char mask;
unsigned char extra = 0;
 //char speed = 0;
char esc = 1;
uint8_t speed = 0;

boolean verify_data()
{
  for (int i = 0; i < 3; ++i)
  {
    if (data[i] < 0 || data[i] > 100)
    {
      return false;
    }
  }
  
  return data[3] & 1;
}

void setup() {
  
  uint8_t speed = 0;
  Serial.begin(9600);
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  
  int i = 0;
  while(i < 250)
  {
  i++;
  digitalWrite(1, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  delayMicroseconds (1000);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
 
  delay(15);
  delay(4);
  }
}

void esc_speed(uint8_t speed, char esc)
{
  if(speed > 100)
  {
    speed = 100;
  }
  if(speed < 0)
  {
    speed = 0;  
  }
  if(esc == 1)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(1, HIGH);
    delayMicroseconds (speed);
    digitalWrite(1, LOW);
    delayMicroseconds (3000 - speed);
  } 
  if(esc == 2)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(2, HIGH);
    delayMicroseconds (speed);
    digitalWrite(2, LOW);
    delayMicroseconds (3000 - speed);
  }
  if(esc == 3)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(3, HIGH);
    delayMicroseconds (speed);
    digitalWrite(3, LOW);
    delayMicroseconds (3000 - speed);
  }
}

void loop() {
//noInterrupts();
uint8_t speed; 

SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
digitalWrite(RDY_PIN, HIGH);
speed = SPI.transfer(0);
if (speed == START) 
  { 
    for (int i = 0; i < 4; ++i)
    {
      data[i] = SPI.transfer(0);
      
      if (data[i] == START) 
      {
        break;
      }
    }
    if (verify_data())
    {
      for (int i = 0; i < 4; ++i)
      {
        Serial.print(data[i]);
        Serial.print(' ');
      }
      Serial.println(' ');
    }
  }
      
      mask = data[3];
      if(mask&0x80 != 0){
        digitalWrite(LED, HIGH);
      }
      if(mask&0x80 == 0){
        digitalWrite(LED, LOW);
      }  
      if(mask&0x08 != 0){
        digitalWrite(MOTOR_R, HIGH);
      }
      if(mask&0x08 == 0){
        digitalWrite(MOTOR_R, LOW);
      }
      if(mask&0x04 != 0){
        digitalWrite(MOTOR_L, HIGH);
      }
      if(mask&0x04 == 0){
        digitalWrite(MOTOR_L, LOW);
      }
      if(mask&0x02 != 0){
        digitalWrite(MOTOR_U, HIGH);
      }
      if(mask&0x02 == 0){
        digitalWrite(MOTOR_U, LOW);
      }  
      
    
SPI.endTransaction();
noInterrupts();
esc = 1;
long timer_2;
if(mask&0x08 != extra&0x08){
  timer = millis();
  timer_2 = timer;
}
  if (timer - timer_2 < 1000){  
  speed = 0;
  }  
esc_speed(esc, speed);
//Serial.println(speed);
esc = esc + 1;
if(mask&0x04 != extra&0x04){
  timer = millis();
  timer_2 = timer;
      }
  if (timer - timer_2 < 1000){  
  speed = 0;
  }  
esc_speed(esc, speed);
//Serial.println(speed);
esc = esc + 1;
if(mask&0x02 != extra&0x02){
  timer = millis();
  timer_2 = timer;
      }
if (timer - timer_2 < 1000){  
speed = 0;
  }  
esc_speed(esc, speed);
//Serial.println(speed);
extra = mask;
interrupts();
digitalWrite(RDY_PIN, LOW);
delayMicroseconds(11000);

}

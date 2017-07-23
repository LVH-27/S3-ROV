#include <SPI.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define START 255
#define LED 5
#define MOTOR_L 7
#define MOTOR_R 8
#define MOTOR_U 6

unsigned long timer[3];
volatile uint8_t data[4];
volatile int curr = 0;
volatile boolean has_started = false;
uint8_t mask;
uint8_t prev[3];
uint8_t extra = 0;
//char speed = 0;
char esc = 1;
uint8_t speed = 0;

void clear_data()
{
  for (int i = 0; i < 4; ++i)
  {
    data[i] = 0;
  }
}

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

ISR (SPI_STC_vect)
{
  noInterrupts();
  
  byte c = SPDR;
  
  if (c & 1) //mask
  {
    data[3] = c;
    ++curr;
  }
  else //motors
  {//data[0] = ((c >> 6) & 3) == 3;
    if ((c >> 6 & 3) == 3) //left motor
    {
      c &= ~((1 << 7) + (1 << 6));
      data[0] = c * 2;
      ++curr;
    }
    else if ((c >> 6 & 3) == 2) //right motor
    {
      c &= ~((1 << 7) + (1 << 6));
      data[1] = c * 2;
      ++curr;
    }
    else if ((c >> 6 & 3) == 1) //center motor
    {
      c &= ~((1 << 7) + (1 << 6));
      data[2] = c * 2;
      ++curr;
    }
  }
  
  if (curr == 5)
  {
    curr = 0;
    //digitalWrite(RDY_PIN, LOW);
  }
  /*if (SPDR == START)
  {
    has_started = true;
    curr = 0;
    //clear_data();
  }
  else if (has_started)
  {
    byte c = SPDR;
    
    if ((curr >= 0 && curr <= 2 && !(curr & 1)) || ((curr == 0 || curr == 3) && (curr & 1)))
    {
      data[curr] = c;
    }
    
    ++curr;
    if (curr == 4)
    {
      has_started = false; 
      curr = 0;
      digitalWrite(RDY_PIN, LOW);
    }
  }*/
  interrupts();
}

void setup() {
  
  uint8_t speed = 0;
  Serial.begin(9600);
  //pinMode(MOSI, INPUT);
  //pinMode(MISO, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  
  SPCR |= _BV(SPE);
  
  int i = 0;
  while(i < 250)
  {
    i++;
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(10, HIGH);
    delayMicroseconds (1000);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(10, LOW);
   
    delay(15);
    delay(4);
  }
  
  //SPI.attachInterrupt();
}

void esc_speed(char esc, int speed)
{//Serial.print("Speed: "); Serial.println(speed);
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
    digitalWrite(11, HIGH);
    delayMicroseconds (speed);
    digitalWrite(11, LOW);
    delayMicroseconds (5000 - speed);
  } 
  if(esc == 2)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(12, HIGH);
    delayMicroseconds (speed);
    digitalWrite(12, LOW);
    delayMicroseconds (5000 - speed);
  }
  if(esc == 3)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(10, HIGH);
    delayMicroseconds (speed);
    digitalWrite(10, LOW);
    delayMicroseconds (5000 - speed);
  }
}

void loop() {
  //noInterrupts();
  int speed; 
  //digitalWrite(RDY_PIN, HIGH);
  for (int i = 0; i < 4; ++i)
  {
    if (Serial.available() > 0)
    {
      int c = Serial.read();
      if (c & 1) //mask
      {
        data[3] = c;
      }
      else //motors
      {
        if ((c >> 6 & 3) == 3) //left motor
        {
          c &= ~((1 << 7) + (1 << 6));
          data[0] = c * 2;
        }
        else if ((c >> 6 & 3) == 2) //right motor
        {
          c &= ~((1 << 7) + (1 << 6));
          data[1] = c * 2;
        }
        else if ((c >> 6 & 3) == 1) //center motor
        {
          c &= ~((1 << 7) + (1 << 6));
          data[2] = c * 2;
        }
      }
    }
    
    //digitalWrite(RDY_PIN, LOW);  
  }
  //SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  //delayMicroseconds(1000);
  //speed = SPI.transfer(0);
  //Serial.println("got start");
  //unsigned long time = millis();
  /*if (speed == START) 
  {
    
    /*
    //if (verify_data())*/
      /*for (int i = 0; i < 4; ++i)
      {
        Serial.print(data[i]);
        Serial.print(' ');
      }
      Serial.println(' ');
      //Serial.println(curr);*/
  //}
      
  mask = data[3];
  if((mask&0x80) != 0){
    digitalWrite(LED, HIGH);
  }
  if((mask&0x80) == 0){
    digitalWrite(LED, LOW);
  }  
    
  noInterrupts();
  esc = 1;
  speed = data[0];
  if((mask&8) != (extra&8) && data[0] != 0){
    timer[0] = millis();
    speed = 0;
  }
  if ((millis() - timer[0]) < 1000 && timer[0] != 0){
    speed = 0;
  }
  else if ((millis() - timer[0]) >= 1000)
  {
    timer[0] = 0;
    if((mask&8) != 0){
      digitalWrite(MOTOR_R, HIGH);
    }
    else{
      digitalWrite(MOTOR_R, LOW);
    }
  }
  esc_speed(esc, speed);
  //Serial.println(speed);
  esc = esc + 1;
  speed = data[1];
  if((mask&4) != (extra&4) && data[1] != 0){
    timer[1] = millis();
    speed = 0;
  }
  if ((millis() - timer[1]) < 1000 && timer[1] != 0){ 
    speed = 0;
  } 
  else if ((millis() - timer[1]) >= 1000)
  {
    timer[1] = 0;
    if((mask&4) != 0){
      digitalWrite(MOTOR_L, HIGH);
    }
    else{
      digitalWrite(MOTOR_L, LOW);
    }
  }
  esc_speed(esc, speed); 
  //Serial.println(speed);
  esc = esc + 1;
  speed = data[2];
  //Serial.print((mask&2) != (extra&2));
  //Serial.println(millis() - timer[2]);
  if((mask&2) != (extra&2) && prev[2] != 0 && data[2] != 0){
    timer[2] = millis();
    speed = 0;
  }
  if ((millis() - timer[2]) < 1000 && timer[2] != 0){
    speed = 0;
  }
  else if ((millis() - timer[2]) >= 1000)
  {
    timer[2] = 0;
    if((mask&2) != 2){
      digitalWrite(MOTOR_U, HIGH);
    }
    else{
      digitalWrite(MOTOR_U, LOW);
    }  
  }
  //Serial.print(' ');
  //Serial.println(speed);
  //Serial.println(speed);
  esc_speed(esc, speed);
  //Serial.println(speed);

  extra = mask;
  prev[0] = data[0];
  prev[1] = data[1];
  prev[2] = data[2];
  interrupts();
  delayMicroseconds(5000);
  //Serial.println(millis() - time);
}

#include <SPI.h>
#include <stdint.h>
#include <string.h>

#define LISTEN_GPIO 0
#define START 255 //Start sign

uint8_t data[4], curr = 0, is_listening = false;

void handleData()
{
  for (int i = 0; i < 4; ++i)
  {
    Serial.print(data[i]);
    Serial.print(' ');
  }
  Serial.println(' ');
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(MOSI, INPUT);
  pinMode(MISO, OUTPUT);
  pinMode(5, OUTPUT);
}
  
/*char buff[280];
int x = 0;*/

void loop() {
  digitalWrite(5, LOW);
  uint8_t in;
  
  //SPI.transfer(0x08);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  //SPI.transfer(0x08);
  in = SPI.transfer(0);
  
  if (in == START)
  {
    curr = 0;
    is_listening = true;
  }
  else if (is_listening)
  {
    if (curr < 4) 
    {
      data[curr] = in;
      ++curr;
    }
    if (curr == 4) 
    {
      handleData();
      curr = 0;
      is_listening = false;
    }
  }
  
  SPI.endTransaction();
  /*Serial.print((char)in);
  ++x;
  
  if (x == 10)
  {
    x = 0;
    Serial.print('\n');
  }
  
  return;
  
  if (x < 200)
  {
    buff[x] = (char) in;
    ++x;
    
    if (in % 10 == 0) {buff[x] = '\n'; ++x;}
    if (x == 200) 
    {ž
    
      Serial.print("Buff => "); 
      Serial.println(buff);
    }
    
    return;
  }
  else 
  {
    return;
  }
  
  Serial.println(buff);
  return;
  
  if (curr == 4)
  {
    curr = 0;
    handleData();
  }
  else if ((curr == 0 && in == ((int) 'A')) || curr != 0) 
  {
    data[curr] = in;
    ++curr;
  }*/
}

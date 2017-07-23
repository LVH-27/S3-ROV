void setup() {
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  int i = 0;
  while(i < 250)
  {
  i++;
  digitalWrite(11, HIGH);
  delayMicroseconds (1000);
  digitalWrite(11, LOW);
  delay(15);
  delay(4);
  }
}

void esc_speed(int speed, int esc)
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
    digitalWrite(11, HIGH);
    delayMicroseconds (speed);
    digitalWrite(11, LOW);
    delayMicroseconds (5000 - speed);
    delayMicroseconds (15000);
  } 
  if(esc == 2)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(12, HIGH);
    delayMicroseconds (speed);
    digitalWrite(12, LOW);
    delayMicroseconds (5000 - speed);
    delayMicroseconds (15000);
  }
  if(esc == 3)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(13, HIGH);
    delayMicroseconds (speed);
    digitalWrite(13, LOW);
    delayMicroseconds (5000 - speed);
    delayMicroseconds (15000);
  }
}

void loop() {
 int k;
  while(k < 250)
  {
  k++;
  esc_speed(30, 1);
  delay(15);
  delay(4);
  }
 int j; 
  while(j < 250)
  {
  j++;
  esc_speed(50, 1);
  delay(15);
  delay(4);
  }

}

char speed = 0;
char esc = 1;
String inString;
String outString;
void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  int i = 0;
  while(i < 250)
  {
  i++;
  digitalWrite(4, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  delayMicroseconds (1000);
  digitalWrite(4, LOW);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  delay(15);
  delay(4);
  }
}

void esc_speed(char speed, char esc)
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
    digitalWrite(4, HIGH);
    delayMicroseconds (speed);
    digitalWrite(4, LOW);
    delayMicroseconds (2000 - speed);
  } 
  if(esc == 2)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(2, HIGH);
    delayMicroseconds (speed);
    digitalWrite(2, LOW);
    delayMicroseconds (2000 - speed);
  }
  if(esc == 3)
  {
    speed = map(speed, 0, 100, 1000, 2000);
    digitalWrite(3, HIGH);
    delayMicroseconds (speed);
    digitalWrite(3, LOW);
    delayMicroseconds (2000 - speed);
  }
}

void loop() {
  
  while( Serial.available() > 0){
    esc = Serial.read(); 
    speed = esc;
    if(isDigit(esc)){
      inString += esc;
    }
      if(esc == '\n'){
      
      outString = inString.substring (0,1);
      esc = outString.toInt();
      //Serial.println (esc/1);
      outString = "";
     }  
    switch (esc){
       case 1:
         if(isDigit(speed)){
      inString += speed;
    }
      if(esc == '\n'){
      
      outString = inString.substring (1);
      esc = outString.toInt();
      //Serial.println (speed/1);
      outString = "";
      inString = "";
    }  
         esc_speed(speed, esc);
         //Serial.println(esc);
         //Serial.println(speed/2);
         break; 
       case 2:
          if(isDigit(speed)){
      inString += speed;
    }
      if(esc == '\n'){
      
      outString = inString.substring (1);
      esc = outString.toInt();
      //Serial.println (speed/1);
      outString = "";
      inString = "";
    }  
         esc_speed(speed, esc);
         //Serial.println(esc);
         //Serial.println(speed/2);
         break; 
       case 3:
         
          if(isDigit(speed)){
      inString += speed;
    }
      if(esc == '\n'){
      
      outString = inString.substring (1);
      esc = outString.toInt();
      //Serial.println (speed/1);
      outString = "";
      inString = "";
    }  
         esc_speed(speed, esc);
         //Serial.println(esc);
         //Serial.println(speed/2);
         break;   
    }
  }
delayMicroseconds (14000); 
}

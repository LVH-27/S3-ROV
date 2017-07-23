void setup(){
  pinMode (3, OUTPUT);
  for (int i=1; i<100; i++){
    digitalWrite(3, HIGH);
    delayMicroseconds(1000);
    digitalWrite(3, LOW);
    delayMicroseconds(1000);
    delay(18);
  }
}

void loop(){
  digitalWrite(3, HIGH);
  delayMicroseconds(1500);
  digitalWrite(3, LOW);
  delayMicroseconds(500);
  delay(18);
}



#define AIN A0

void setup()
{
 Serial.begin(9600);
}

void loop() {
 
 int ain;
 int sensorValue = analogRead(A0);
 float voltage = sensorValue * (5.0 / 1023.0);
 
 Serial.print(voltage);
 Serial.print ("\n\r");

 if (voltage < 2.75)
    {
         int fq = 4000;
         long duration = 5000;

         tone(3, fq, duration); 
      }
 
 
 delay(10000);
}

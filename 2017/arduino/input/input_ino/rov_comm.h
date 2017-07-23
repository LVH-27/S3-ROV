#define MSGLEN 10
#define MOTOR_COUNT 3
#define STOP_BYTE 001001100
#define RECV_TIMEOUT_MS 10000000

typedef struct 
    {
    int16_t motor[MOTOR_COUNT];
    uint16_t flags;
    uint8_t check_sum;
    uint8_t stop_byte;
    }
msg;


int checkSum(char raw_msg[]); //checksum check

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



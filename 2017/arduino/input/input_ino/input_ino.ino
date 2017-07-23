// mapping input structure to motor stuff
#include "rov_comm.h"
#include <Servo.h>

#define GET_ROV_STATUS 7
#define LED_CONTROL 8
#define ESC_LEFT 9
#define ESC_RIGHT 10
#define ESC_CENTER 11


Servo esc_left;
Servo esc_right;
Servo esc_center;
msg input_msg;

void setup(){
	pinMode(GET_ROV_STATUS, OUTPUT);
	pinMode(LED_CONTROL, OUTPUT);

	esc_left.attach(ESC_LEFT);
	esc_right.attach(ESC_RIGHT);
	esc_center.attach(ESC_CENTER);

	Serial.begin(9600);
}

float map_float(int value, int min_, int max_, float min_o, float max_o){
  return (float) (value - min_) * (max_o - min_o) / (max_ - min_) + min_o;
}


void loop(){
	



}

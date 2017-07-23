#include "rov_comm.h"
#include <Servo.h>

// PWM constants
#define FULL_REVERSE 900
#define FULL_FORWARD 2100
#define LEER 1500

// Pin definitions
#define BATTERY_1 7
#define LED_CONTROL 8
#define ESC_LEFT 9
#define ESC_RIGHT 10
#define ESC_CENTER 11


Servo esc_left;
Servo esc_right;
Servo esc_center;
char raw_msg[MSGLEN];


void setup(){
	pinMode(GET_ROV_STATUS, OUTPUT);
	pinMode(LED_CONTROL, OUTPUT);

	esc_left.attach(ESC_LEFT);
	esc_right.attach(ESC_RIGHT);
	esc_center.attach(ESC_CENTER);

	Serial.begin(9600);
	Serial.setTimeout(RECV_TIMEOUT_MS);
}


void loop(){
	int len = Serial.readBytes(raw_msg, MSGLEN);
    msg command;
    int errorFlag = 0;
    
    if(len == MSGLEN)
        {
        fillMsg(raw_msg, command);
        uint8_t sum = calcSum(raw_msg);
        
        
        // TODO: implement error check in its own routine
        if(!sumOk(sum, command.check_sum))
            {
            Serial.println("[err] incorrect checksum!");
            errorFlag = 1;
            }
        if(!stopOk(command.stop_byte, STOP_BYTE))
            {
            Serial.println("[err] incorrect stop byte!");
            errorFlag = 1;
            }
        }
    else
        {
        Serial.println("[err] wrong msg length (serial timeout probably occured)");
        errorFlag = 1;
        }
    
    if(!errorFlag)
        {
        	thr_left = map_float(command.motor[0], 0, 1023, FULL_REVERSE, FULL_FORWARD);
        	thr_right = map_float(command.motor[1], 0, 1023, FULL_REVERSE, FULL_FORWARD);
        	thr_center = map_float(command.motor[2], 0, 1023, FULL_REVERSE, FULL_FORWARD);
        	esc_left.writeMicroseconds(thr_left);
        	esc_right.writeMicroseconds(thr_right);
        	esc_center.writeMicroseconds(thr_center);
        }
    else
        {
        //else
        waitForStop(STOP_BYTE);
        }



}

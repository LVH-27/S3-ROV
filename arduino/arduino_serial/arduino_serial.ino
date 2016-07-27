#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

struct __attribute((__packed__)) rpi_msg_t {
	int header;
	unsigned char throttle[3];
	char mask;
};

rpi_msg_t rpi_msg;

/*
	Arduino mask format is defined as follows
	  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  
	RESER PWR_DN  x    LED  C_REV R_REV L_REV RESER
*/
#define POWER_DOWN 6
#define LED_ON 4
#define LEFT_REVERSE 1
#define RIGHT_REVERSE 2
#define CENTER_REVERSE 3

#define LED_PIN 9
#define LEFT_REV_PIN 8
#define RIGHT_REV_PIN 7
#define CENTER_REV_PIN 6

#define ESC_LEFT_PIN 10
#define ESC_RIGHT_PIN 11
#define ESC_CENTER_PIN 12

struct motor_t {
	int esc;
	unsigned short throttle;
	char direction;
};

motor_t motors[3];
// So we don't have to use indexes everywhere ...
enum {left_motor, right_motor, center_motor};

/**
	Timer1 interrupt routine
	ESC throttle function
*/
ISR(TIMER1_COMPA_vect)
{
	unsigned int timers[3] = {0};
	
	// Sort by duration
	motor_t *sorted = (motor_t *) malloc(3*sizeof(motor_t));
	memcpy(sorted, motors, 3*sizeof(motor_t));
	motor_t tmp_motor;
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 2 - i; j++){
			if (sorted[j].throttle > sorted[j+1].throttle){
				tmp_motor = sorted[j];
				sorted[j] = sorted[j+1];
				sorted[j+1] = tmp_motor;
			}
		}
	}
	
	// All signals high
	for (int i = 0; i < 3; i++){
		digitalWrite(sorted[i].esc, HIGH);
		timers[i] = (sorted[i].throttle * 1000) / 255 + 1000;
	}
	
	// Consecutively drop signals
	int i = 0;
	while (i < 3){
		if (timers[i] > 0)
		  delayMicroseconds(timers[i]);
		
		digitalWrite(sorted[i].esc, LOW);
		
		for(int j = i; j < 3; j++)
			timers[j] -= timers[i];
		i++;
	}
}

void setup(){
	noInterrupts();
	
	Serial.begin(9600);
	
	// Init motors
	motors[0].esc = ESC_LEFT_PIN;
	motors[1].esc = ESC_RIGHT_PIN;
	motors[2].esc = ESC_CENTER_PIN;
	for (int i = 0; i < 3; i++){
		motors[i].throttle = 0;
		motors[i].direction = 0;
	}
	
	// Timer1 interrupt
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	
	TCCR1B |= ((1 << CS11) | (1 << CS10));	// Prescaler 64
	OCR1A = 1250;							// 5000 ms (ESC clock) = 200 Hz
											// 16 MHz / 64 / 200 Hz
	TCCR1B |= (1 << WGM12);					// CTC mode of operation
	TIMSK1 |= (1 << OCIE1A);				// Enable interrupt
	
	interrupts();
}

char direction_halt[3] = {0};
unsigned long halt_timers[3] = {0};

void loop(){
	
	// If there is serial data, wait for header and receive
	if (Serial.available() > 0){
		
		unsigned char serial_input = Serial.read();
		rpi_msg.header <<= 8;
		rpi_msg.header &= (int) serial_input;
				
		if ( rpi_msg.header != 0xFF00FF00 )
			break;
		
		for (int i = 0; i < 3; i++)
			rpi_msg.throttle[i] = Serial.read();
		rpi_msg.mask = Serial.read();
		
		rpi_msg.header = 0;
		
	}
	
	for (int i = 0; i < 3; i++){
		char direction = ((rpi_msg.mask & (1 << (i+1))) == 0);
		
		// If the direction has changed, wait for 1s before changing direction
		if (direction != motors[i].direction){
			if (direction_halt[i] == 0){
				direction_halt[i] = 1;
				halt_timers[i] = millis();
			} else {
				if ((millis() - halt_timers[i]) >= 1000) {
					motors[i].direction = direction;
					direction_halt[i] = 0;
				}
			}
		} else
			direction_halt[i] = 0;
		
		// If the direction change is in progress halt the motor
		if ((direction_halt[i] != 0) && (direction == motors[i].direction))
			motors[i].throttle = rpi_msg.throttle[i];
		else
			motors[i].throttle = 0;
		
	}
	
}

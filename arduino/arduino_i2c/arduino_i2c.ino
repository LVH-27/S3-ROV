#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>

#define I2C_ADDR 4

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

#define LED_PIN 5
#define LEFT_REV_PIN 8
#define RIGHT_REV_PIN 7
#define CENTER_REV_PIN 6

#define ESC_LEFT_PIN 11
#define ESC_RIGHT_PIN 10
#define ESC_CENTER_PIN 9

struct motor_t {
	int esc;
	unsigned short throttle;
	char direction;
	char direction_pin;
};

motor_t motors[3];
// So we don't have to use indexes everywhere ...
enum {left_motor, right_motor, center_motor};

/**
	Timer1 interrupt routine
	ESC throttle function
*/
ISR(TIMER1_COMPA_vect) {
	for (int i = 0; i < 3; i++){
		unsigned int duty = map(motors[i].throttle, 0, 255, 1000, 2000);
		
		digitalWrite(motors[i].esc, HIGH);
		delayMicroseconds(duty);
		digitalWrite(motors[i].esc, LOW);
		delayMicroseconds(2000 - duty);
	}
}

void setup(){
	noInterrupts();
	
	Serial.begin(9600);
//	Wire.begin(4);
//  Wire.onReceive(rpi_msg_recv);
	
	// Init motors
	motors[0].esc = ESC_LEFT_PIN;
	motors[1].esc = ESC_RIGHT_PIN;
	motors[2].esc = ESC_CENTER_PIN;
	motors[0].direction_pin = LEFT_REV_PIN;
	motors[1].direction_pin = RIGHT_REV_PIN;
	motors[2].direction_pin = CENTER_REV_PIN;
	
	// Timer1 interrupt
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	
	TCCR1B |= (1 << CS11 | 1 << CS10);		// Prescaler 64
	OCR1A = 5150;							// 20 ms (ESC clock) = 50 Hz
											// 16 MHz / 64 / 50 Hz = 5000 + measured time
	TCCR1B |= (1 << WGM12);					// CTC mode of operation
	TIMSK1 |= (1 << OCIE1A);				// Enable interrupt
	
	interrupts();
}

char direction_halt[3] = {0};
unsigned long halt_timers[3] = {0};

char led_status = 0;
char led_latch = 0;

void rpi_msg_recv(int){
  
  // If there is serial data, wait for header and receive
  
    unsigned char serial_input = Wire.read();
    rpi_msg.header <<= 8;
    rpi_msg.header &= (unsigned int) serial_input;
        
    if ( rpi_msg.header == 0xFF00FF00 ){
      for (int i = 0; i < 3; i++)
        rpi_msg.throttle[i] = Wire.read();
      rpi_msg.mask = Wire.read();
      
      rpi_msg.header = 0;
    }
  
}

void loop(){
	for (int i = 0; i < 3; i++){
		char direction = ((rpi_msg.mask & (1 << (i+1))) != 0);
		
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
		if (direction_halt[i] == 0)
			motors[i].throttle = rpi_msg.throttle[i];
		else
			motors[i].throttle = 0;
		
	}
	
	// Check for lights change
	if ((rpi_msg.mask & (1 << LED_ON)) != 0){
		if (led_latch != 1){
			led_status = ~led_status;
			led_latch = 1;
		}
	} else
		led_latch = 0;
	
	digitalWrite(LED_PIN, (led_status == 0 ? LOW : HIGH));
	
	// If the RPI server quits reset motors and wait a bit
	if ((rpi_msg.mask & (1 << POWER_DOWN)) != 0){
		for (int i = 0; i < 3; i++)
			motors[i].throttle = 0;
		delay(1000);
		for (int i = 0; i < 3; i++)
			motors[i].direction = 0;
		delay(1000);
	}
	
}

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>

#define I2C_ADDR 4

struct __attribute((__packed__)) rpi_msg_t {
	unsigned char header;
	unsigned char throttle[3];
	unsigned char mask;
	unsigned char tail;
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
enum {left_motor, right_motor, center_motor};


void setup(){
	noInterrupts();
	
	//Serial.begin(250000);
	Wire.begin(4);
    Wire.onReceive(msg_recv);
	
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

/**
	Timer1 interrupt routine
	ESC throttle function
*/
ISR(TIMER1_COMPA_vect) {
	for (int i = 0; i < 3; i++){
		digitalWrite(motors[i].direction_pin, motors[i].direction ? HIGH : LOW);
		unsigned int duty = map(motors[i].throttle, 0, 255, 1000, 2000);
		
		digitalWrite(motors[i].esc, HIGH);
		delayMicroseconds(duty);
		digitalWrite(motors[i].esc, LOW);
		delayMicroseconds(2000 - duty);
	}
}

rpi_msg_t recv;
int position;
/**
	I2C data receive function
*/
void msg_recv(int size){
	while (Wire.available() > 0){
		
		unsigned char buf = Wire.read();
		
		if (rpi_msg.header != 0xb5){
			rpi_msg.header = buf;
		} else {
			if (position < 3){
				recv.throttle[position] = buf;
				position++;
			} else if (position == 3){
				recv.mask = buf;
				position++;
			} else if (position == 4){
				recv.tail = buf;
				
				if (recv.tail == 0x5b){
					rpi_msg = recv;
					
					// DEBUG OUTPUT
					/*
					char a[32];
					sprintf(a, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x END", rpi_msg.header, rpi_msg.throttle[0], rpi_msg.throttle[1], rpi_msg.throttle[2], rpi_msg.mask, rpi_msg.tail);
					Serial.println(a);
					*/
				}
				
				recv.header = 0;
				recv.tail = 0;
				
				position = 0;
			}
			
			if (position < 4 && (buf == 0xb5 || buf == 0x5b)){
				recv.header = 0;
				recv.tail = 0;
				
				position = 0;
			}
			
		}
	}
}


char direction_halt[3] = {0};
unsigned long halt_timers[3] = {0};

char led_status = 0;
char led_latch = 0;

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
		for (int i = 0; i < 3; i++){
			motors[i].throttle = 0;
			rpi_msg.throttle[i] = 0;
		}
		delay(1000);
		for (int i = 0; i < 3; i++)
			motors[i].direction = 0;
		delay(1000);
	}
	
}

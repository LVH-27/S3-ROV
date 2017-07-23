#include "movement.h"

#include <math.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define _USE_MATH_DEFINES
#define M_PI_8 (0.5 * M_PI_4)

int movement::pins[7] = {7, 8, 11, 9, 10, 25, 24};
short movement::led_state = 0;

void movement::init()
{
	wiringPiSetupGpio();
	for(int i=0;i<7;i++)
	{
		softPwmCreate(pins[i], 0, 5);
	}
}

enum {left_motor, right_motor, center_motor};

void movement::motor_intensity(int id, float intensity)
{
	intensity*=5;
	if(intensity>=0)
	{
		if(id==left_motor)
		{
			softPwmWrite(pins[left_fw], intensity);
			softPwmWrite(pins[left_bw], 0);
		}
		else if(id==right_motor)
		{
			softPwmWrite(pins[right_fw], intensity);
			softPwmWrite(pins[right_bw], 0);
		}
		else
		{
			softPwmWrite(pins[center_down], intensity);
			softPwmWrite(pins[center_up], 0);
		}
	}
	else
	{
		if(id==left_motor)
		{
			softPwmWrite(pins[left_bw], -intensity);
			softPwmWrite(pins[left_fw], 0);
		}
		else if(id==right_motor)
		{
			softPwmWrite(pins[right_bw], -intensity);
			softPwmWrite(pins[right_fw], 0);
		}
		else
		{
			softPwmWrite(pins[center_down], 0);
			softPwmWrite(pins[center_up], -intensity);
		}
	}
}

void movement::led_switch(){
	softPwmWrite(pins[led], !led_state);
	led_state = !led_state;
}

void movement::handle_input(float axis0, float axis1, float axis2, float axis3, unsigned mask)
{
	if(mask&1u<<8) //select dugme - naredba quit
		exit(0);
	float angle = atan2(-axis1, axis0);
	float r = std::hypot(axis0, -axis1); if(r>1) r=1;
	fprintf(stderr, "angle: %.2f, r: %.2f\n, r2: %.2f\n", angle, r, fabs(axis2));

	if(fabs(axis2)<0.05) 
		motor_intensity(center_motor, 0);
	else
		motor_intensity(center_motor, axis2);
	
	if(mask & (1u<<6))
	{
		motor_intensity(left_motor, -1);
		motor_intensity(right_motor, 1);
	}
	else if(mask& (1u<<7))
	{
		motor_intensity(left_motor, 1);
		motor_intensity(right_motor, -1);
	}
	else if(mask& (1u<<3))
	{
		led_switch();
	}
	
	
	if(mask&(3u<<6))
	{
		//bilo je pritisnuto okretanje, necemo da gledamo analognu kapicu
		return;
	}
	

	if(r<0.05) 
	{
		motor_intensity(left_motor, +0);
		motor_intensity(right_motor, +0);
	}
	else
	{
		if(0 <= angle && angle < M_PI_2)
		{
			motor_intensity(left_motor, r);
			motor_intensity(right_motor, r*(1-axis0));
		}
		else if(M_PI_2 <= angle && angle < M_PI)
		{
			motor_intensity(right_motor, r);
			motor_intensity(left_motor, r*(1+axis0));
		}
		else if(-7 * M_PI_8 <= angle && angle < -5 * M_PI_8)
		{
			motor_intensity(right_motor, -r);
			motor_intensity(left_motor, 0);
		}
		else if(-3 * M_PI_8 <= angle && angle < - M_PI_8)
		{
			motor_intensity(left_motor, -r);
			motor_intensity(right_motor, 0);
		}
		else if(-5 * M_PI_8 <= angle && angle <-3*M_PI_8)
		{
			motor_intensity(left_motor, -r );
			motor_intensity(right_motor, -r );
		}
	}
}

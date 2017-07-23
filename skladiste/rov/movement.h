#pragma once

#include <wiringPi.h>
#include <softPwm.h>

namespace movement
{
	enum  {center_up, center_down, left_fw, left_bw, right_fw, right_bw, led};
	extern int pins[7];
	extern short led_state;

	void init();
	void handle_input(float axis0, float axis1, float axis2, float axis3, unsigned mask);
	void led_switch();
	void motor_intensity(int id, float intensity);
}

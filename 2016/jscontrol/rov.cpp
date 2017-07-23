#include <iostream>
#include <fstream>
#include <cmath>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <sys/time.h>

#define _USE_MATH_DEFINES
#define M_PI_8 (0.5 * M_PI_4)

using namespace std;

/**
	Server receive packet structure
*/
struct __attribute__((__packed__)) srv_msg_t {
    float f1, f2, f3, f4;
    unsigned int mask;
};

/**
	Arduino message
*/
struct ard_msg_t {
	unsigned char thr_left, thr_right, thr_center;
	unsigned char mask;
};

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

enum {left_motor, right_motor, center_motor};

int sock, port = 0;
srv_msg_t msg;
ard_msg_t ard_msg;

void socket_setup() {

	int socket_desc;
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	
	sockaddr_in sock_name;
	sock_name.sin_family = AF_INET;
	sock_name.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_name.sin_port = htons(1234);
	
	bind(socket_desc, (sockaddr*)& sock_name, sizeof(sock_name));
	listen(socket_desc, 1);
	fprintf(stderr, "Listening on port %d\n", port);
	
	sock = accept(socket_desc, 0, 0);
}

/**
	Stores absolute throttle for each motor and determines the direction for each motor
*/
void motor_intensity(int id, float throttle_float) {
	
	unsigned char throttle;
	throttle = (unsigned char) abs((255 * throttle_float));
	
	if (throttle_float >= 0) {
		if (id == left_motor) {
			ard_msg.mask &= ~(1 << LEFT_REVERSE);
			ard_msg.thr_left = throttle;
		}
		else if (id == right_motor) {
			ard_msg.mask &= ~(1 << RIGHT_REVERSE);
			ard_msg.thr_right = throttle;
		}
		else if (id == center_motor) {
			ard_msg.mask &= ~(1 << CENTER_REVERSE);
			ard_msg.thr_center = throttle;
		}
	} else {
		if (id == left_motor) {
			ard_msg.mask |= 1 << LEFT_REVERSE;
			ard_msg.thr_left = throttle;
		}
		else if (id == right_motor) {
			ard_msg.mask |= 1 << RIGHT_REVERSE;
			ard_msg.thr_right = throttle;
		} else if (id == center_motor) {
			ard_msg.mask |= 1 << CENTER_REVERSE;
			ard_msg.thr_center = throttle;
		}
	}
}


/**
	Determines the throttle for each motor
*/
void handle_input(float axis0, float axis1, float axis2, float axis3, unsigned int mask) {

	// The angle and the radius calculated below adhere to the
	// left analog stick as represented by the image below
	//      --- PI/2 ---
	//     /            \ r=1
	//    /              \
	// +-PI      o r=0    0
	//    \              /
	//     \            /
	//      -- -PI/2 ---
	float angle = atan2(-axis1, axis0);
	float r = std::hypot(axis0, -axis1); if(r>1) r=1;
	//fprintf(stderr, "angle: %.2f, r: %.2f, r2: %.2f\n", angle, r, fabs(axis2));
	
	
	// LEFT AND RIGHT MOTOR
	// Interval r < 0.05 is taken as a dead zone
	if (r<0.05) {
		motor_intensity(left_motor, +0);
		motor_intensity(right_motor, +0);
	} else {
		// MOVING FORWARD
		if (0 <= angle && angle < M_PI_2) {
			// axis0 > 0
			motor_intensity(left_motor, r*(1+axis0)/2);
			motor_intensity(right_motor, r*(1-axis0)/2);
		}
		else if ((M_PI_2 <= angle && angle < M_PI) || angle == ((float) -M_PI)) {
			// axis0 < 0
			motor_intensity(right_motor, r*(1-axis0)/2);
			motor_intensity(left_motor, r*(1+axis0)/2);
		}
		// MOVING BACKWARD
		// Intervals <-PI_8, 0> and <-PI, -7/8 PI> are taken as dead zones.
		// Therefore, the axis0 is normalized to cos(M_PI_8) so as to preserve the throttle range.
		else if (-M_PI_2 <= angle && angle < -M_PI_8) {
			axis0 /= cos(M_PI_8);
			// axis0 > 0
			motor_intensity(right_motor, -r*(1+axis0)/2);
			motor_intensity(left_motor, -r*(1-axis0)/2);
		}
		else if (-7*M_PI_8 < angle && angle < -M_PI_2) {
			axis0 /= cos(M_PI_8);
			// axis0 < 0
			motor_intensity(left_motor, -r*(1-axis0)/2);
			motor_intensity(right_motor, -r*(1+axis0)/2);
		}
	}
	
	// CENTER MOTOR
	if(fabs(axis2)<0.05)
			motor_intensity(center_motor, 0);
	else
			motor_intensity(center_motor, axis2);
	
	// SERVER QUIT
	if (mask & 1u<<8)
		ard_msg.mask |= 1 << POWER_DOWN;
}

char buf[1024];

int main (int argc, char** argv)
{

	// Initialise server socket
	if(argc>1)
		port = atoi(argv[1]);
    else
		port = 1234;
    socket_setup();
	
	// Initialise Arduino communication
	int fd;
	fd = wiringPiI2CSetup(0x04);

	while (1) {
		// Receiving packet
		int len = recv(sock, buf, 4*sizeof(float)+sizeof(unsigned int), MSG_WAITALL);
        msg = *(srv_msg_t*)(void*)(&buf);
		
		// Init mask
		ard_msg.mask = 0x81;
		
		handle_input(msg.f1, msg.f2, msg.f3, msg.f4, msg.mask);
		
		// Send data to Arduino
		wiringPiI2CWrite(fd, 0xB5);
			wiringPiI2CWrite(fd, ard_msg.thr_left);
			wiringPiI2CWrite(fd, ard_msg.thr_right);
			wiringPiI2CWrite(fd, ard_msg.thr_center);
			wiringPiI2CWrite(fd, ard_msg.mask);
		wiringPiI2CWrite(fd, 0x5B);
		
		printf("Voltage: %f\n", wiringPiI2CRead(fd)/1000.);	
		//printf("%d\n%d\n%d\n%d\n", ard_msg.thr_left, ard_msg.thr_right, ard_msg.thr_center, ard_msg.mask);	
		// Quit on command
		if (msg.mask & 1u<<8)
			return 0;
	}
	
	return 1;
}

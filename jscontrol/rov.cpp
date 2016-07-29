#include <iostream>
#include <fstream>
#include <cmath>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#include<wiringPi.h>
#include<wiringSerial.h>

#include <sys/time.h>

#define _USE_MATH_DEFINES
#define M_PI_8 (0.5 * M_PI_4)

#define READY_GPIO 24

using namespace std;

/**
	Server receive packet structure
*/
struct __attribute__((__packed__)) srv_msg_t {
    float f1, f2, f3, f4;
    unsigned mask;
};

/**
	Arduino send packet structure
*/
struct __attribute__((__packed__)) ard_msg_t {
	unsigned int header;
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

int sock, port, ready = 0;
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
	close(socket_desc);
}

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
			ard_msg.thr_left = -throttle;
		}
		else if (id == right_motor) {
			ard_msg.mask |= 1 << RIGHT_REVERSE;
			ard_msg.thr_right = -throttle;
		} else if (id == center_motor) {
			ard_msg.mask |= 1 << CENTER_REVERSE;
			ard_msg.thr_center = -throttle;
		}
	}
}

void handle_input(float axis0, float axis1, float axis2, float axis3, unsigned mask) {

	if (mask&1u<<8) //select dugme - naredba quit
			exit(0);
			
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
	// Promjena smjerova releja ne bi se smjela dogadjati unutar bilo koje mrtve zone
	//
	// Interval r < 0.05 is taken as a dead zone
	if (r<0.05) {
		motor_intensity(left_motor, +0);
		motor_intensity(right_motor, +0);
	} else {
		// MOVING FORWARD
		if (0 <= angle && angle < M_PI_2) {
			// axis0 > 0
			motor_intensity(left_motor, r*axis0);
			motor_intensity(right_motor, r*(1-axis0));
		}
		else if ((M_PI_2 <= angle && angle < M_PI) || angle == ((float) -M_PI)) {
			// axis0 < 0
			motor_intensity(right_motor, r*(-axis0));
			motor_intensity(left_motor, r*(1+axis0));
		}
		// MOVING BACKWARD
		// Intervals <-PI_8, 0> and <-PI, -7/8 PI> are taken as dead zones.
		// Therefore, the axis0 is normalized to cos(M_PI_8) so as to preserve the throttle range.
		else if (-M_PI_2 <= angle && angle < -M_PI_8) {
			axis0 /= cos(M_PI_8);
			// axis0 > 0
			motor_intensity(left_motor, -r*axis0);
			motor_intensity(right_motor, -r*(1-axis0));
		}
		else if (-7*M_PI_8 < angle && angle < -M_PI_2) {
			axis0 /= cos(M_PI_8);
			// axis0 < 0
			motor_intensity(right_motor, -r*(-axis0));
			motor_intensity(left_motor, r*(1+axis0));
		}
	}
	
	// CENTER MOTOR
	if(fabs(axis2)<0.05)
			motor_intensity(center_motor, 0);
	else
			motor_intensity(center_motor, axis2);
	
	
	// KONTROLA LEDICA
	
	// POWER DOWN SISTEM
}

char buf[1024];

int main (int argc, char** argv)
{
	unsigned mask = 255;
	char path[] = "/dev/ttyUSBx";

	// Default port is 1234
	if(argc>1) port = atoi(argv[1]);
        else port = 1234;
        socket_setup();
	
	wiringPiSetupGpio();
	
	// Signaling READY_GPIO
	pinMode(READY_GPIO, INPUT);
	pinMode(24, OUTPUT);
	digitalWrite(24, HIGH);

	
	// We use the first responsive ttyUSB device
	int fd;	
	for (int i = 0; i < 10; ++i) {
		path[11] = ((char)(i + '0'));
		
		fd = serialOpen(path, 9600);
		
		if (fd != -1){
			fprintf(stderr, "%s", path);
			break;
		}
	}
	if (fd == -1)
		return 1;

	while (1) {
		// Receiving packet
		int len = recv(sock, buf, 4*4+4, MSG_WAITALL);
                msg = *(srv_msg_t*)(void*)(&buf);
		//fprintf(stderr, "%1.2f %1.2f %1.2f 0x%2x\n", msg.f1, msg.f2, msg.f3, msg.mask);
		
		// Mask has the low bit 1
		// This is an attempt to determine the position of the mask in the struct received on Arduino
		msg.mask |= 1;
		
		if (mask == 255)
			mask = msg.mask;

		char spi_data[5];
		unsigned in;		
		
		ard_msg.header = 0xFF00FF00;
		ard_msg.mask = 0x81;
		
		handle_input(msg.f1, msg.f2, msg.f3, msg.f4, msg.mask);
		
		digitalWrite(24, !digitalRead(24));
		
		//spi_data[0] = (char) 255;
		if (msg.f1 < 0) {
			//spi_data[2] = ((char) 0);
            msg.mask |= (mask & 1 << 3);
	    }
		else {
			//spi_data[2] = ((char) ((int) (msg.f1 <= 1 ? msg.f1 * 100 : msg.f1)));
            mask |= (msg.mask & 1 << 3);                
	    }
	    
		if (msg.f2 < 0) {
			//spi_data[3] = ((char) 0);
            msg.mask |= (mask & 1 << 1);
        }
        else {
			//spi_data[3] = ((char) ((int) (msg.f2 <= 1 ? msg.f2 * 100 : msg.f2)));
            mask |= (msg.mask & 1 << 1);
        }
        
		//spi_data[1] = ((char) ((int) msg.f3));
        //spi_data[4] = (char) msg.mask;

		//spi_data[1] |= (1 << 7) + (1 << 6);
		//spi_data[2] |= (1 << 7);
		//spi_data[2] &= ~(1 << 6);
		//spi_data[3] |= (1 << 6);
		//spi_data[3] &= ~(1 << 7);

		//spi_out << spi_data[0] << flush;
		//for (int i = 1; i < 4; ++i) {
		//	spi_data[i] &= ~(1);
		//	serialPutchar(fd, spi_data[i]);
		//}
		
		//serialPutchar(fd, spi_data[4]);
		
		for (int i = 0; i < 4; i++)
			serialPutchar(fd, (char) ((ard_msg.header >> 8*(4-i)) & 0xFF));
		serialPutchar(fd, ard_msg.thr_left);
		serialPutchar(fd, ard_msg.thr_right);
		serialPutchar(fd, ard_msg.thr_center);
		serialPutchar(fd, ard_msg.mask);
		
	}	
	
	return 0;
}

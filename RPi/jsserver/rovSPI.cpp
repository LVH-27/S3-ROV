#include <iostream>
#include <fstream>
#include <cmath>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#include<wiringPi.h>

#include <sys/time.h>

#define _USE_MATH_DEFINES
#define M_PI_8 (0.5 * M_PI_4)

#define READY_GPIO 25

using namespace std;

struct msg_t
{
        float f1, f2, f3, f4;
        unsigned mask;
};

enum {left_motor, right_motor, center_motor};

int sock, port, ready = 0;
msg_t msg;

void socket_setup()
{
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

void motor_intensity(int id, float intensity)
{
        intensity*=100;
        if(intensity>=0)
        {
                if(id==left_motor)
                {
			msg.mask &= ~(1 << 2);
			msg.f1 = intensity;
                }
                else if(id==right_motor)
                {
			msg.mask &= ~(1 << 3);
			msg.f2 = intensity;
                }
                else
                {
			msg.mask &= ~(1 << 1);
			msg.f3 = intensity;
                }
        }
        else
        {
                if(id==left_motor)
                {
			msg.mask |= 1 << 2;
			msg.f1 = -intensity;
                }
                else if(id==right_motor)
                {
			msg.mask |= 1 << 3;
			msg.f2 = -intensity;
                }
                else
                {
			msg.mask |= 1 << 1;
			msg.f3 = -intensity;
		}
        }
}

void handle_input(float axis0, float axis1, float axis2, float axis3, unsigned mask)
{
	msg.f1 = msg.f1 < 0 ? -1 : 0;
	msg.f2 = msg.f2 < 0 ? -1 : 0;

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
                msg.mask |= 1 << 7;
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
                else if((M_PI_2 <= angle && angle < M_PI) || angle == ((float) -M_PI))
                {
                        motor_intensity(right_motor, r);
                        motor_intensity(left_motor, r*(1+axis0));
                }
                else if(-7 * M_PI_8 <= angle && angle < -5 * M_PI_8)
                {
                        motor_intensity(right_motor, -r);
                        motor_intensity(left_motor, 0);
                }
                else if(-3 * M_PI_8 <= angle && angle < -M_PI_8)
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

char buf[1024];

int main (int argc, char** argv)
{
	unsigned mask = 255;

	if(argc>1) port = atoi(argv[1]);
        else port = 1234;
        socket_setup();
	
	wiringPiSetupGpio();
	
	pinMode(READY_GPIO, INPUT);

	ofstream spi("/dev/spidev0.0");

	while (1) 
	{
		cout << "GPIO: " << digitalRead(READY_GPIO) << endl;
		int len = recv(sock, buf, 4*4+4, MSG_WAITALL);
                msg = *(msg_t*)(void*)(&buf);
		msg.mask |= 1;

		if (mask == 255)
		{
			mask = msg.mask;
		}

		unsigned char spi_data[5];
		handle_input(msg.f1, msg.f2, msg.f3, msg.f4, msg.mask);

		if (digitalRead(READY_GPIO))
                {
			++ready;
			//sleep(0.01);
/*
			spi << (char) 255;
			if (msg.f1 < 0)
			{
				spi << ((char) 0) << flush;
				cout << "0 ";
				msg.mask |= (mask & 1 << 2);
			}
			else
			{
				spi << ((char) ((int) msg.f1)) << flush;
				cout << (int) msg.f1 << " ";
				mask |= (msg.mask & 1 << 2);
				
			}
			if (msg.f2 < 0)
                        {
                                spi << ((char) 0) << flush;
				cout << "0 ";
                                msg.mask |= (mask & 1 << 3);
                        }
                        else
                        {
                                spi << ((char) ((int) msg.f2)) << flush;
				cout << (int) msg.f2 << " ";
				mask |= (msg.mask & 1 << 3);

                        }
			spi << ((char) ((int) msg.f3)) << flush;
			cout << (int) msg.f3 << " ";
			spi << (char) msg.mask << flush;
			cout << msg.mask << endl;
*/			
			if (ready == 5)
			{
				spi_data[0] = (char) 255;
				if (msg.f1 < 0)
        	                {
                	                spi_data[1] = ((char) 0);
                        	        msg.mask |= (mask & 1 << 2);
	                        }
        	                else
                	        {
                        	        spi_data[1] = ((char) ((int) (msg.f1 <= 1 ? msg.f1 * 100 : msg.f1)));
                                	mask |= (msg.mask & 1 << 2);                
	                        }
	                        if (msg.f2 < 0)
        	                {
        	                        spi_data[2] = ((char) 0);
                	                msg.mask |= (mask & 1 << 3);
                       	 	}
                       	 	else
                        	{
                                	spi_data[2] = ((char) ((int) (msg.f2 <= 1 ? msg.f2 * 100 : msg.f2)));
                                	mask |= (msg.mask & 1 << 3);
                        	}
                        	spi_data[3] = ((char) ((int) msg.f3));
                        	spi_data[4] = (char) msg.mask;

				for (int i = 0; i < 5; ++i)
				{
					cout << (int) spi_data[i] << ' ';
					spi << spi_data[i] << flush;
				}
				cout << endl;
				ready = 1;
				//sleep(0.01);
			}
		}
	}	
	
	return 0;
}

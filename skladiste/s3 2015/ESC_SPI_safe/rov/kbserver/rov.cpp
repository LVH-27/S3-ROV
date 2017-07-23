#include "pi_lib.h"
#include "movement.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#include <iostream>

using namespace std;

int sock, port;

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

void signal_handler(int signum)
{
	exit(0);
}

char buf[1024];

int main(int argc, char** argv)
{
	if(argc>1) port = atoi(argv[1]);
	else port = 1234;

	//signal(SIGINT, signal_handler)

	socket_setup();
	init_pins();
	while(1)
	{
		size_t len;
		if((len = recv(sock, buf, sizeof(buf), 0))>0)
		{
			switch(buf[0])
			{
				case '+':
				command_press(buf[1]);
				break;
				case '-':
				command_release(buf[1]);
				break;
				default:
				fprintf(stderr, "Quitting\n");
				destroy_gpio();
				exit(0);
			}
			do_commands();
		}
	}

	return 0;
}
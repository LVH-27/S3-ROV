#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>

#include <iostream>

#include "movement.h"

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

char buf[1024];

struct msg_t
{
	float f1, f2, f3, f4;
	unsigned mask;
};

int main(int argc, char** argv)
{
	if(argc>1) port = atoi(argv[1]);
	else port = 1234;
	socket_setup();
	movement::init();

	while(1)
	{
		int len = recv(sock, buf, 4*4+4, MSG_WAITALL);
		msg_t msg = *(msg_t*)(void*)(&buf);
		if(len>0)
		{
			//printf("%f %f %f %f %u\n", msg.f1, msg.f2, msg.f3, msg.f4, msg.mask);
			movement::handle_input(msg.f1, msg.f2, msg.f3, msg.f4, msg.mask);
		}
		
	}

	return 0;
}
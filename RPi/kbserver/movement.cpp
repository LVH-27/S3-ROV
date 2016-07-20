#include "pi_lib.h"
#include "movement.h"

int pins[6] = {7, 8, 11, 9, 10, 25};
bool command_active[6];

void init_pins()
{
	for(int i=0;i<6;i++)
	{
		gpio::open(pins[i], 'w');
	}
}

void destroy_gpio()
{
	for(int i=0;i<6;i++)
	{
		gpio::close(pins[i]);
	}
}

void command_press(char c)
{
	switch(c)
	{
		case 'w':
			command_active[(int)commands::left_fw] = true;
			command_active[(int)commands::right_fw] = true;
			command_active[(int)commands::left_bw] = false;
			command_active[(int)commands::right_bw] = false;
			break;
		case 's':
			command_active[(int)commands::left_bw] = true;
			command_active[(int)commands::right_bw] = true;
			command_active[(int)commands::left_fw] = false;
			command_active[(int)commands::right_fw] = false;
			break;
		case 'a':
			command_active[(int)commands::right_fw] = true;
			command_active[(int)commands::right_bw] = false;
			break;
		case 'd':
			command_active[(int)commands::left_fw] = true;
			command_active[(int)commands::left_bw] = false;
			break;
		case 'q':
			command_active[(int)commands::left_bw] = true;
			command_active[(int)commands::left_fw] = false;
			command_active[(int)commands::right_fw] = true;
			command_active[(int)commands::right_bw] = false;
			break;
		case 'e':
			command_active[(int)commands::left_bw] = false;
			command_active[(int)commands::left_fw] = true;
			command_active[(int)commands::right_fw] = false;
			command_active[(int)commands::right_bw] = true;
			break;
		case 'o':
			command_active[(int)commands::center_up] = true;
			command_active[(int)commands::center_down] = false;
			break;
		case 'l':
			command_active[(int)commands::center_down] = true;
			command_active[(int)commands::center_up] = false;
			break;
		default:
			fprintf(stderr, "Invalid command byte %d\n", c);
	}
}

void command_release(char c)
{
	switch(c)
	{
		case 'w':
			command_active[(int)commands::left_fw] = false;
			command_active[(int)commands::right_fw] = false;
			break;
		case 's':
			command_active[(int)commands::left_bw] = false;
			command_active[(int)commands::right_bw] = false;
			break;
		case 'a':
			command_active[(int)commands::right_fw] = false;
			break;
		case 'd':
			command_active[(int)commands::left_fw] = false;
			break;
		case 'q':
			command_active[(int)commands::left_bw] = false;
			command_active[(int)commands::right_fw] = false;
			break;
		case 'e':
			command_active[(int)commands::left_fw] = false;
			command_active[(int)commands::right_bw] = false;
			break;
		case 'o':
			command_active[(int)commands::center_up] = false;
			break;
		case 'l':
			command_active[(int)commands::center_down] = false;
			break;
		default:
			fprintf(stderr, "Invalid command byte %d\n", c);
	}
}

void do_commands()
{
	for(int i=0;i<6;i++)
	{
		gpio::write(pins[i], command_active[i]);
	}
}
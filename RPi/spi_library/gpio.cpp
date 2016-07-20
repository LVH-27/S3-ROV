#include "gpio.h"

int gpio::gpio_fdesc[30];

void gpio::open(int num, char mode)
{
	FILE* desc = fopen("/sys/class/gpio/export", "w");
	fprintf(desc, "%d", num);
	fclose(desc);
	char direction_fname[100], value_fname[100];
	sprintf(direction_fname, "/sys/class/gpio/gpio%d/direction", num);
	sprintf(value_fname, "/sys/class/gpio/gpio%d/value", num);
	desc = fopen(direction_fname, "w");
	if(mode=='w')
	{
		fprintf(desc, "out");
		gpio_fdesc[num] = ::open(value_fname, O_WRONLY);
	}
	else
	{
		fprintf(desc, "in");
		gpio_fdesc[num] = ::open(value_fname, O_RDONLY);
	}
	fclose(desc);
}

void gpio::close(int num)
{
	::close(gpio_fdesc[num]);
	FILE* desc = fopen("/sys/class/gpio/unexport", "w");
	fprintf(desc, "%d", num);
	fclose(desc);
}

void gpio::write(int id, unsigned short val) //da se zna, ovo je samo 1 bit
{
	unsigned char byte = val?'1':'0';
	printf("writing %c to %d\n", byte, id);
	::write(gpio_fdesc[id], &byte, 1);
}

unsigned short gpio::read(int id)
{
	unsigned short byte;
	::read(gpio_fdesc[id], &byte, 1);
	return byte;
}


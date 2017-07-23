#pragma once

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <map>
#include <string>

using namespace std;

namespace gpio
{
	extern int gpio_fdesc[30];

	void open(int num, char mode);
	void close(int num);
	void write(int id, unsigned short val);
}

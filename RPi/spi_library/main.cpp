#include "spi.h"
#include <stdio.h>

using namespace std;

int main(){
	spi::handle spi = spi::setup(23, 24, 19, 21, 1000000);
	unsigned char buf = 'T';
	for(int i = 0; i < 500000; i++){
		spi::transfer(&spi, &buf, 8);
		usleep(5000);
	}
	spi::destroy(&spi);
}

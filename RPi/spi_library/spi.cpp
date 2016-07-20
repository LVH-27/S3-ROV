#include "spi.h"
#include "gpio.h"
#include <math.h>

spi::handle spi::setup(int pinCLK, int pinSS, int pinMOSI, int pinMISO, unsigned long frequency){
	spi::handle handle;
	
	handle.pinCLK = pinCLK;
		gpio::open(pinCLK, 'w');
	handle.pinSS = pinSS;
		gpio::open(pinSS, 'w');
	handle.pinMOSI = pinMOSI;
		gpio::open(pinMOSI, 'w');
	handle.pinMISO = pinMISO;
		gpio::open(pinMISO, 'r');
	handle.frequency = frequency;
	
	return handle;	
}


void spi::destroy(spi::handle* handle){
	gpio::close(handle -> pinCLK);
	gpio::close(handle -> pinSS);
	gpio::close(handle -> pinMOSI);
	gpio::close(handle -> pinMISO);
}


void spi::transfer(spi::handle* handle, void* data, int length){
	gpio::write(handle -> pinSS, LOW);

	for (unsigned int bit = 0, unsigned short *b = data; bit < length; bit++, b++){
		gpio::write(handle -> pinCLK, LOW);
		gpio::write(handle -> pinMOSI, *data & 0x80 );
		
		usleep( floor(0.5 / handle -> frequency) );
		
		// Rotiranje registara
		for (unsigned int byte = 0; byte < ceil(length/8); byte++){
			*(data + byte) = *(data + byte) << 1;
			if ( byte == ceil(length/8) - 1) {
				*(data + byte) &= 0xfe;
			} else {
				*(data + byte) |= ( (*(data + byte + 1) & 0x80) >> 7);
			}
		}
		
		*(data + length/8) &= (gpio::read(handle -> pinMISO) & 0x01);
		gpio::write(handle -> pinCLK, HIGH);
		
		usleep( ceil(0.5 / handle -> frequency) );
	}
	
	gpio::write(handle -> pinCLK, LOW);
	gpio::write(handle -> pinSS, HIGH);
}


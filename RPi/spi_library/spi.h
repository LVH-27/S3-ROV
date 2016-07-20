#include "gpio.h"
#define LOW 0
#define HIGH 1

using namespace std;

namespace spi {
	
	typedef struct {
		int pinCLK, pinSS, pinMOSI, pinMISO;
		unsigned long frequency;
	} handle;
	
	handle setup(int pinCLK, int pinSS, int pinMOSI, int pinMISO, unsigned long frequency);
	void destroy(handle* handle);
	void transfer(handle* handle, void* data, int length);

}

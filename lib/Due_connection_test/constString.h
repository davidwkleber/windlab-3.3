// constString.h

#ifndef _CONSTSTRING_h
#define _CONSTSTRING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

String constStringLength(long value, int valueSize);
String constStringLength(unsigned long value, int valueSize);

String constStringLength(uint8_t value, int valueSize);
String constStringLength(uint16_t value, int valueSize);
String constStringLength(uint32_t value, int valueSize);

String constStringLength(int8_t value, int valueSize);
String constStringLength(int16_t value, int valueSize);


#endif


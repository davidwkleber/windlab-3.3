// 
// 
// 

#include "constString.h"

String constStringLength(long value, int valueSize){
	char charBuffer[valueSize+2];
	sprintf(charBuffer,"%*ld",valueSize,value);
	return String(charBuffer);
}

String constStringLength(unsigned long value, int valueSize){
	char charBuffer[valueSize+2];
	sprintf(charBuffer,"%*lu",valueSize,value);
	return String(charBuffer);
}

String constStringLength(uint8_t value, int valueSize){
	char charBuffer[valueSize+2];
	sprintf(charBuffer,"%*u",valueSize,value);
	return String(charBuffer);
}

String constStringLength(uint16_t value, int valueSize){
	char charBuffer[valueSize+2];
	sprintf(charBuffer,"%*u",valueSize,value);
	return String(charBuffer);
}
String constStringLength(int8_t value, int valueSize){
	char charBuffer[valueSize+2];
	sprintf(charBuffer,"%*d",valueSize,value);
	return String(charBuffer);
}

String constStringLength(int16_t value, int valueSize){
	char charBuffer[valueSize+2];
	sprintf(charBuffer,"%*d",valueSize,value);
	return String(charBuffer);
}

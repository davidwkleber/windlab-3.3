/* 
* MCP4726.h
*
* Created: 13.08.2015 13:58:39
* Author: mw7
* Controlling the DAC MCP4726
*/


#ifndef __MCP4726_H__
#define __MCP4726_H__

#include "Arduino.h"
#include <Wire.h>
#include "MCP4726_SETUP_VREF.h"

#if defined(USE_VREF_REF_INPUT)
	#define MCP4726_CMD_WRITEDAC	(0x58)	//Write data to the DAC and uses Vref input
#elif defined(USE_VCC_REF_INPUT)
	#define MCP4726_CMD_WRITEDAC    (0x40)  // Writes data to the DAC with Vcc as Vref
#endif // #if defined

class MCP4726
{

private:
	//variables
	uint8_t _address;
	//functions
	void setupI2C(void);	
	byte LByte(uint16_t sendByte);
	byte HByte(uint16_t sendByte);

public:
	//variables
	
	//functions
	MCP4726(uint8_t address);
	void begin(void);
	void setVoltage(uint16_t vBit);
}; //MCP4726

#endif //__MCP4726_H__



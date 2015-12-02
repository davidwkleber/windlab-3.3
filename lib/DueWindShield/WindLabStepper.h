/* 
* WindLabStepper.h
*
* Created: 17.09.2015 11:29:02
* Adapted and stripped down AccelStepper Library
* Source: http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
* Author: Maik Wagner
* E-Mail: Wagner@mf.tu-berlin.de
* alternative E-Mail: Maik.Wagner@gmx.eu
*/

#include<Arduino.h>

#ifndef __WINDLABSTEPPER_H__
#define __WINDLABSTEPPER_H__


class WindLabStepper
{
//variables
protected:
// Symbolic names for the direction the motor is turning
typedef enum
{
	DIRECTION_CCW = 0,  ///< Clockwise
	DIRECTION_CW  = 1   ///< Counter-Clockwise
} Direction;
private:
	int32_t _currentPos;
	int32_t _targetPos;
	uint32_t _stepInterval;
	uint16_t _pulseWidth;
	uint8_t _enablePin;
	uint8_t _pin[2];
	uint32_t _lastStepTime;
	boolean _direction;
	float _speed;
	float _maxSpeed;
	int32_t _n;
	float _c0;
	float _cn;
	float _cmin;
	
	
//functions
public:
	WindLabStepper(uint8_t dirPin, uint8_t stpPin, uint8_t enPin);
	void moveTo(int32_t absolute);
	boolean runSpeed();
	void setSpeed(float speed);
	int32_t distanceToGo(void);
	int32_t targetPosition();
	int32_t currentPosition();
	void setCurrentPosition(int32_t position);
	void enable(void);
	void disable(void);
	void setPulseWidth(uint8_t usPulseWidth);	
	void setMaxSpeed(float speed);
	float maxSpeed(void);
protected:
	void step();
	void setOutputPins(uint8_t mask);
}; //WindLabStepper

#endif //__WINDLABSTEPPER_H__

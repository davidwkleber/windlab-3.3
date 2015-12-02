/* 
* WindLabStepper.cpp
*
* Created: 17.09.2015 11:29:02
* Adapted and stripped down AccelStepper Library
* Source: http://www.airspayce.com/mikem/arduino/AccelStepper/index.html
* Author: Maik Wagner
* E-Mail: Wagner@mf.tu-berlin.de
* alternative E-Mail: Maik.Wagner@gmx.eu
*/


#include "WindLabStepper.h"

// default constructor
WindLabStepper::WindLabStepper(uint8_t dirPin, uint8_t stpPin, uint8_t enPin)
{
    _currentPos = 0;
    _targetPos = 0;
    _stepInterval = 0;
	_lastStepTime = 0;
    _pulseWidth = 1;
    _enablePin = enPin;
    _pin[0] = stpPin;
    _pin[1] = dirPin;
	_speed =0.0;
	_maxSpeed = 500.0;
	
	// NEW
	_n = 0;
	_c0 = 0.0;
	_cn = 0.0;
	_cmin = 1.0;
	_direction = DIRECTION_CCW;

} //WindLabStepper

void WindLabStepper::setOutputPins(uint8_t mask){
	digitalWrite(_pin[0],(mask&(1 << 0)) ? (HIGH) : (LOW));
	digitalWrite(_pin[0],(mask&(1 << 1)) ? (HIGH) : (LOW));
}

void WindLabStepper::moveTo(int32_t absolute){
	if (_targetPos!=absolute){
		_targetPos = absolute;
		
	}
}

long WindLabStepper::distanceToGo()
{
	return _targetPos - _currentPos;
}

long WindLabStepper::targetPosition()
{
	return _targetPos;
}

long WindLabStepper::currentPosition()
{
	return _currentPos;
}

void WindLabStepper::setCurrentPosition(long position)
{
	_targetPos = _currentPos = position;
	//_n = 0;
	_stepInterval = 0;
}

boolean WindLabStepper::runSpeed()
{
	// Dont do anything unless we actually have a step interval
	if (!_stepInterval)
	return false;

	unsigned long time = micros();
	unsigned long nextStepTime = _lastStepTime + _stepInterval;
	// Gymnastics to detect wrapping of either the nextStepTime and/or the current time
	if (   ((nextStepTime >= _lastStepTime) && ((time >= nextStepTime) || (time < _lastStepTime)))	|| ((nextStepTime < _lastStepTime) && ((time >= nextStepTime) && (time < _lastStepTime))))	{
		if (_direction == DIRECTION_CW)
		{
			// Clockwise
			_currentPos += 1;
		}
		else
		{
			// Anticlockwise
			_currentPos -= 1;
		}
		step();

		_lastStepTime = time;
		return true;
	}
	else
	{
		return false;
	}
}

void WindLabStepper::step()
{
	// _pin[0] is step, _pin[1] is direction
	setOutputPins(_direction ? 0b10 : 0b00); // Set direction first else get rogue pulses
	setOutputPins(_direction ? 0b11 : 0b01); // step HIGH
	// Caution 200ns setup time
	// Delay the minimum allowed pulse width
	delayMicroseconds(_pulseWidth);
	setOutputPins(_direction ? 0b10 : 0b00); // step LOW

}

void WindLabStepper::setSpeed(float speed)
{
	if (speed == _speed){
		return;
	}
	speed = constrain(speed, -_maxSpeed, _maxSpeed);
	if (speed == 0.0){
		_stepInterval = 0;
	}
	
	else{
		_stepInterval = fabs(1000000.0 / speed);
		_direction = (speed > 0.0) ? DIRECTION_CW : DIRECTION_CCW;
	}
	_speed = speed;
}

void WindLabStepper::setMaxSpeed(float speed)
{
	if (_maxSpeed != speed)
	{
		_maxSpeed = speed;
		_cmin = 1000000.0 / speed;
		// Recompute _n from current speed and adjust speed if accelerating or cruising
		//if (_n > 0)
		//{
			//_n = (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16
			//computeNewSpeed();
		//}
	}
}

float WindLabStepper::maxSpeed(){
	return _maxSpeed;
}
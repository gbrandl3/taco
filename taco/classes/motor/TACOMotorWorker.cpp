// Server for a (stepper/servo) motor 
// Copyright (C) 2007 Jens Krüger

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#if HAVE_SYS_TIME_H
#	include <sys/time.h>
#endif

#include <cmath>
#include "TACOMotorWorker.h"

/**
 * Constructor
 */
TACO::Motor::Worker::Worker() throw (::TACO::Exception)
	: m_speed(10)
	, m_accel(1)
	, m_userMin(-1000)
	, m_userMax(1000)
	, m_currentPos(0)
	, m_backlash(0)
	, m_target(0)
	, m_startTime(0)
	, m_unit("mm")
{
}
 
/**
 * Destructor.
 */
TACO::Motor::Worker::~Worker() throw ()
{
}

/**
 * Initiate the motor to decelerate with a ramp
 */
void TACO::Motor::Worker::stop() throw (::TACO::Exception)
{
	DevDouble currentTime = getTime();
	switch(internalState(currentTime))
	{
		case 1 :
			m_accelTime = currentTime - m_startTime;	// stop the acceleration 
			m_accelDist = (m_accel / 2 ) * (m_accelTime * m_accelTime);
			m_constTime = m_constDist = 0;			// no const speed move
			m_target = m_currentPos + 2 * m_accelDist;
		case 3 :
		default:
			break;
		case 2 :
			m_constTime = (currentTime - m_startTime) - m_accelTime;
			m_constDist = m_constTime / m_speed;
			m_target = m_currentPos + 2 * m_accelDist + m_constDist;
			break;
	}
	m_moveTime = 2 * m_accelTime + m_constTime;
}

/**
 * stops the motor device immediatly
 */
void TACO::Motor::Worker::abort() throw (::TACO::Exception)
{
	m_target = read();
	m_accelTime = 0;
	m_accelDist = 0;
	m_constTime = 0;
	m_constDist = 0;
}

/**
 * Moves the motor relative from the current position input units away.
 * It checks the acceleration and speed values. If one of them is 0.0 it throws 
 * an exception. The value of distance is given in units.
 * 
 * @param input distance to move away from current position, values > 0 mean 
 *	movement in positive direction, values < 0 in negative direction
 * @exception INVALID_VALUE in case of acceleration or speed are 0
 * @exception RANGE_ERROR in case of new position is outside the limits
 * @see move
 * @see unit
 * @see acceleration
 * @see speed
 */ 
void TACO::Motor::Worker::move(const DevDouble input) throw (::TACO::Exception)
{
	if (input == 0.0)
		return;
	if (m_accel == 0.0)
		throw ::TACO::Exception(::TACO::Error::INVALID_VALUE, "no acceleration value set");
	if (m_speed == 0.0)
		throw ::TACO::Exception(::TACO::Error::INVALID_VALUE, "no speed value set");
	if (m_currentPos + input > m_userMax)
		throw ::TACO::Exception(::TACO::Error::RANGE_ERROR, "target > user maximum");
	if (m_currentPos + input < m_userMin)
		throw ::TACO::Exception(::TACO::Error::RANGE_ERROR, "target < user minimum");

	m_dir = input < 0 ? -1 : 1;	
//
// Get a starting time and calculate the times and distances 
// for acceleration constant movement and deceleration 
//
	m_accelTime = m_speed / m_accel;
	m_accelDist = (m_accel / 2 ) * (m_accelTime * m_accelTime);
	m_constDist = ::fabs(input) - 2 * m_accelDist;
//
// No const move
//
	if (m_constDist <= 0)
	{
		m_constDist = m_constTime = 0;
		m_accelDist = ::fabs(input) / 2.;
		m_accelTime = ::sqrt(input / m_accel);
	}
	else
		m_constTime = m_constDist / m_speed;
	m_moveTime = 2 * m_accelTime + m_constTime;
	m_target = m_currentPos + input;
	m_startTime = getTime();
//
// sleep 2 ms to ensure a movement
//
	usleep (2000);
}

/**
 * Moves the motor device to the absolute position. It takes the current position
 * and calculates the difference to the new position and calls move with this. The
 * value is given in units.
 *
 * @param input new desired position
 * @see move
 * @see unit
 */
void TACO::Motor::Worker::moveTo(const DevDouble input) throw (::TACO::Exception)
{
	this->move(input - this->read());
}

/**
 * Sets the current position to the given value. The value is given in units.
 *
 * @param input the new position 
 * @exception RANGE_ERROR in case the new position is outside the limits.
 * @see unit
 * @see read
 */
void TACO::Motor::Worker::setpos(const DevDouble input) throw (::TACO::Exception)
{
	if (input < m_userMin) 
		throw ::TACO::Exception(::TACO::Error::RANGE_ERROR, "target < user minimum");
	if (input > m_userMax)	
		throw ::TACO::Exception(::TACO::Error::RANGE_ERROR, "target > user maximum");
	m_target = input;
}

/**
 * Sets the speed for the constant movement part. The input value is given
 * in units per second.
 *
 * @param input new speed value in units per second
 * @see unit
 * @see setUnit
 * @see speed
 */
void TACO::Motor::Worker::setSpeed(const DevDouble input) throw (::TACO::Exception)
{
	m_speed = input;
}

/**
 * Returns the currently set value for the speed. This value is given in
 * units per second.
 *
 * @return currently set speed value.
 * @see unit
 * @see setSpeed
 */
DevDouble TACO::Motor::Worker::speed() throw (::TACO::Exception)
{
#if 0
	DevDouble currentTime = getTime();
	switch (internalState(currentTime))
	{
		default:
			return 0;
		case 1:
			currentTime -= m_startTime;
			return m_accel * currentTime;
		case 2:
			return m_speed;
		case 3:
			currentTime -= m_startTime + m_constTime;
			currentTime = m_accelTime - currentTime;
			return m_accel * currentTime;
	}
#endif
	return m_speed;
}

DevDouble TACO::Motor::Worker::read() throw (::TACO::Exception)
{
	DevDouble currentTime = getTime();
	switch (internalState(currentTime))
	{
		default:
			if (m_currentPos != m_target)
				m_currentPos = m_target;
			return m_currentPos;
		case 1 :
			currentTime -= m_startTime;
			return m_currentPos + m_dir * (m_accel / 2.) * (currentTime * currentTime);
		case 2 :
			currentTime -= m_startTime + m_accelTime;
			return m_currentPos + m_dir * (m_accelDist + currentTime * m_speed);
		case 3 :
			currentTime -= m_startTime + m_accelTime + m_constTime;
			currentTime = m_accelTime - currentTime;
			return m_currentPos + m_dir * (m_accelDist + m_constDist + (m_accelDist - (m_accel / 2.0) * (currentTime * currentTime)));
	}
}

/**
 * Sets the acceleration slope. All inputs will currently ignored. The only 
 * supported type is 'linear'. 
 * 
 * @param input the slope of the acceleration/deceleration
 * @see accSlope
 */
void TACO::Motor::Worker::setAccSlope(const std::string &input) throw (::TACO::Exception)
{
}

/**
 * Returns the value of the acceleration slope. Currently always 'linear'
 *
 * @return value of the acceleration/deceleration slope
 * @see setAccSlope
 */
std::string TACO::Motor::Worker::accSlope(void) throw (::TACO::Exception)
{
	return "linear";
}

/**
 * Checks the current state of the motor device. 
 *
 * @return MOVING or DEVICE_NORMAL if not moving
 */
DevShort TACO::Motor::Worker::state(void) throw (::TACO::Exception)
{
	if (internalState(getTime()))
		return ::TACO::State::MOVING;
	return ::TACO::State::DEVICE_NORMAL;
}

/**
 * Determines the current state of the 'motor device'
 *
 * @return 0 if not started or move finished
 *	   1 if in acceleration period
 *	   2 if in constant speed move period
 *	   3 if in deceleration period
 */
DevShort TACO::Motor::Worker::internalState(const DevDouble currentTime)
{
	DevDouble testTime = m_startTime + m_accelTime;
	if (currentTime < testTime)
		return 1;
	testTime += m_constTime;
	if (currentTime < testTime)
		return 2;
	testTime += m_accelTime;
	if (currentTime < testTime)
		return 3;
	return 0;
}

/**
 * @return the current time since (1.1.1970) in seconds as double value
 */
DevDouble TACO::Motor::Worker::getTime(void) throw (::TACO::Exception)
{
        struct timeval  tp;
        struct timezone tzp;

	if (gettimeofday(&tp, &tzp) != -1)
	{
		DevDouble tmp = DevDouble(tp.tv_sec) + tp.tv_usec / 1000000.0;
		return tmp;
	}
	throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "Could not get the time of day");
}

/**
 * Sets the unit value. This value is simly a string and should be used to
 * indicate the value of the movement.
 * 
 * @param input new value of the unit (should be an abbrevation of a SI unit)
 * @see unit
 */
void TACO::Motor::Worker::setUnit(const std::string &input) throw (::TACO::Exception)
{
	if (input == "")
		m_unit = "mm";	
	else
		m_unit = input;
}

/**
 * @return currently set value for unit
 * @see setUnit
 */
std::string TACO::Motor::Worker::unit(void) throw (::TACO::Exception)
{
	return m_unit;
}

/**
 * Sets the backlash in units of the motor (or axis). A positive value means the 
 * motor will always reach the position from lower side, negative from the upper.
 *
 * @param input backlash value
 * @see backlash
 * @see unit
 */
void TACO::Motor::Worker::setBacklash(const DevDouble input) throw (::TACO::Exception)
{
	m_backlash = input;
}

/**
 * @return backlash of the motor or motor axis (in units)
 * @see setBacklash
 * @see unit
 */ 
DevDouble TACO::Motor::Worker::backlash(void) throw (::TACO::Exception)
{
	return m_backlash;
}

/**
 * Sets the value for the acceleration/deceleration of the motor device. The value is
 * given in units per square second.
 *
 * @param input acceleration/deceleration value
 * @see acceleration
 * @see unit
 */
void TACO::Motor::Worker::setAcceleration(const DevDouble input) throw (::TACO::Exception)
{
	if (input < 0)
		throw ::TACO::Exception(::TACO::Error::INVALID_VALUE, "acceleration must be greater 0");
#if 0
	if (input > m_accelMax)
		throw ::TACO::Exception(::TACO::Error::INVALID_VALUE, "acceleration value is to large");
#endif
	m_accel = input;
}

/**
 * Returns the currently set value of the acceleration/deceleration. This value is
 * given in units per square seconds.
 *
 * @return the current value of acceleration/deceleration
 * @see unit
 * @see setAcceleration
 */
DevDouble TACO::Motor::Worker::acceleration() throw (::TACO::Exception)
{
	return m_accel;
}


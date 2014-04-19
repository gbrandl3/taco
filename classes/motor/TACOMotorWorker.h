// Server for a (stepper/servo) motor 
// Copyright (C) 2007-2014 Jens Krüger

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

#ifndef TACO_WORKER_MOTOR_H
#define TACO_WORKER_MOTOR_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TACOServer.h>

namespace TACO {
	namespace Motor {
		class Worker;
	}
}

class TACO::Motor::Worker
{
public:
	Worker() throw (::TACO::Exception);

	~Worker() throw ();

	virtual void stop() throw (::TACO::Exception);

	virtual void abort() throw (::TACO::Exception);

	virtual void move(const DevDouble input) throw (::TACO::Exception);

	virtual void moveTo(const DevDouble input) throw (::TACO::Exception);

	virtual void setpos(const DevDouble input) throw (::TACO::Exception);

	virtual void setSpeed(const DevDouble input) throw (::TACO::Exception);

	virtual DevDouble speed() throw (::TACO::Exception);

	virtual DevDouble read() throw (::TACO::Exception);

	virtual void setAccSlope(const std::string &input) throw (::TACO::Exception);

	virtual std::string accSlope(void) throw (::TACO::Exception);

	DevShort state(void) throw (::TACO::Exception);

	virtual DevDouble acceleration(void) throw (::TACO::Exception);

	virtual void setAcceleration(const DevDouble input) throw (::TACO::Exception);

	virtual void setUnit(const std::string &input) throw (::TACO::Exception);

	virtual std::string unit(void) throw (::TACO::Exception);

	virtual DevDouble backlash(void) throw (::TACO::Exception);

	virtual void setBacklash(const DevDouble input) throw (::TACO::Exception);

private:
	DevDouble getTime(void) throw (::TACO::Exception);

	DevShort internalState(DevDouble currentTime);

protected:
	DevDouble m_speed;	//< current value for speed

	DevDouble m_accel;	//< current value for acceleration/deceleration

	DevDouble m_userMin;	//< minimum for the position
	
	DevDouble m_userMax;	//< maximum for the position

	DevDouble m_limitMin;	//< absolute minimum for the position, is a hardware limit

	DevDouble m_limitMax;	//< absolute maximum for the position, is a hardware limit

	DevDouble m_backlash;	//< value for the backlash

	std::string m_unit;	//< stores the unit

private:
	DevDouble m_accelTime;	//< time for the acceleration/deceleration of the current movement
	
	DevDouble m_constTime;	//< time for the distance with constant speed of the current movement

	DevDouble m_accelDist;	//< distance to accelerate/decelerate the motor in the current movement

	DevDouble m_constDist;	//< distance with constant speed of the current movement

	DevDouble m_currentPos;	//< position before/after a complete movement

	DevDouble m_moveTime;	//< calculated time for the whole movement

	DevDouble m_target;	//< target of the movement

	DevDouble m_startTime;	//< start time of the movement

	int	  m_dir;	//< direction of the movement (1 - in positive direction, -1 - in negative)
};

#endif // TACO_CLASS_MOTOR_H

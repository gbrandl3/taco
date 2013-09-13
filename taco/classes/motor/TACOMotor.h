// Server for a (stepper/servo) motor 
// Copyright (C) 2007-2013 Jens Krüger

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

#ifndef TACO_CLASS_MOTOR_H
#define TACO_CLASS_MOTOR_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TACOServer.h>

namespace TACO {
	namespace Motor {
		class Motor;
		class Worker;
	}
}

class TACO::Motor::Motor
	: public ::TACO::Server
//	: public IPCSMS::IO::AnalogOutput
{
public:
	Motor(const std::string& name, DevLong& error) throw (::TACO::Exception);

	~Motor() throw ();

protected:
	const char* GetClassName(void);

	const char *GetDevType(void);

//	void deviceUpdate() throw (::TACO::Exception);

//	void deviceQueryResource() throw (::TACO::Exception);

	DevShort deviceState(void) throw (::TACO::Exception);

	// TACODEVEL CODEGEN METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

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

	virtual void moveToReference(void) throw (::TACO::Exception);

	virtual DevDouble acceleration(void) throw (::TACO::Exception);
	
	virtual void setAcceleration(const DevDouble input) throw (::TACO::Exception);

	virtual void setUnit(const std::string &input) throw (::TACO::Exception);

	virtual std::string unit(void) throw (::TACO::Exception);

	virtual DevDouble backlash(void) throw (::TACO::Exception);

	virtual void setBacklash(const DevDouble input) throw (::TACO::Exception);

//	void deviceReset() throw (::TACO::Exception);

	virtual void v_Init(void) throw (::TACO::Exception);

private:
	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

// for AnalogInput
	static void tacoRead(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoSetUnit(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoUnit(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

// for AnalogOutput
	static void tacoStop(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoAbort(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoMove(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoMoveTo(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoSetpos(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoSetSpeed(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoSpeed(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoSetAcceleration(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoAcceleration(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

// for Motor
	static void tacoSetAccelerationSlope(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoAccelerationSlope(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoMoveReference(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoSetBacklash(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	static void tacoReadBacklash(::TACO::Server *server, void *argin, void *argout) throw (::TACO::Exception);

	// TACODEVEL CODEGEN TACO METHOD DECLARATIONS END

protected:
	DevDouble	m_speedMax;

	Worker		*m_motor;
};

#endif // TACO_CLASS_MOTOR_H

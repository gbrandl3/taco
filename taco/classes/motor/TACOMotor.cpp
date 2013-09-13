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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TACOConverters.h>
#include "TACOMotor.h"

#include <Admin.h>
#include <iostream>

#include "TACOMotorCommon.h"
#include "TACOMotorWorker.h"

TACO::Motor::Motor::Motor( const std::string& name, DevLong& error) throw (::TACO::Exception)
	: ::TACO::Server(name, error)
//	: TACO::IO::AnalogOutput( name, error)
	, m_motor(NULL)
{
	addDeviceType( ::Motor::MOTOR_ID);
// Analog commands
	addCommand(DevReadPosition, &tacoRead, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS, "Read");

	addCommand(DevSetUnits, &tacoSetUnit, D_VAR_CHARARR, D_VOID_TYPE, SI_WRITE_ACCESS, "SetUnit");
	addCommand(DevReadUnits, &tacoUnit, D_VOID_TYPE, D_VAR_CHARARR, READ_ACCESS, "Unit");

// Analog output commands
	addCommand(DevLoadPosition, &tacoSetpos, D_DOUBLE_TYPE, D_VOID_TYPE, SI_WRITE_ACCESS, "SetPosition");

	addCommand(DevStop, &tacoStop, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "Stop");
	addCommand(DevStopImm, &tacoAbort, D_VOID_TYPE, D_VOID_TYPE, WRITE_ACCESS, "Abort");

	addCommand(DevMoveRelative, &tacoMove, D_DOUBLE_TYPE, D_VOID_TYPE, WRITE_ACCESS, "Move");
	addCommand(DevMoveAbsolute, &tacoMoveTo, D_DOUBLE_TYPE, D_VOID_TYPE, WRITE_ACCESS, "MoveTo");

	addCommand(DevSetVelocity, &tacoSetSpeed, D_DOUBLE_TYPE, D_VOID_TYPE, SI_WRITE_ACCESS, "SetSpeed");
	addCommand(DevReadVelocity, &tacoSpeed, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS, "Speed");

	addCommand(DevSetAcceleration, &tacoSetAcceleration, D_DOUBLE_TYPE, D_VOID_TYPE, ADMIN_ACCESS, "SetAcceleration");
	addCommand(DevReadAcceleration, &tacoAcceleration, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS, "Acceleration");

	addCommand(DevSetAccSlope, &tacoSetAccelerationSlope, D_VAR_CHARARR, D_VOID_TYPE, ADMIN_ACCESS, "SetAccelerationSlope");
	addCommand(DevReadAccSlope, &tacoAccelerationSlope, D_VOID_TYPE, D_VAR_CHARARR, READ_ACCESS, "AccelerationSlope");

	addCommand(DevMoveReference, &tacoMoveReference, D_VOID_TYPE, D_VOID_TYPE, SI_WRITE_ACCESS, "MoveToReferencePoint");

	addCommand(DevSetBacklash, &tacoSetBacklash, D_DOUBLE_TYPE, D_VOID_TYPE, SI_WRITE_ACCESS, "SetBacklash");
	addCommand(DevReadBacklash, &tacoReadBacklash, D_VOID_TYPE, D_DOUBLE_TYPE, READ_ACCESS, "Backlash");

// set the device version, helpful for debugging or client/server compatibility
	setDeviceVersion( VERSION );

// Standard resources for a motor device
	addResource("usermax", D_DOUBLE_TYPE, "max. position allowed for user access" );
	addResource("usermin", D_DOUBLE_TYPE, "min. position allowed for user access" );
	addResource("speed", D_DOUBLE_TYPE, "speed in units/s" );
	addResource("unit", D_STRING_TYPE, "unit of the movement");
	addResource("gear", D_DOUBLE_TYPE, "ratio between output and input of axis");
	addResource("backlash", D_DOUBLE_TYPE, "backlash of the axis");

	try
	{
		v_Init();
		setDeviceState(::TACO::State::DEVICE_NORMAL);
		logStream->noticeStream() << GetClassName() << " : " << deviceName() << " : init complete." << log4cpp::eol;
	}
	catch (const ::TACO::Exception &e)
	{
		logStream->noticeStream() << GetClassName() << " : " << deviceName() << " : init failed. " << e.what() << log4cpp::eol;
		Server::setDeviceState(DEVFAULT);
	}
}

const char *TACO::Motor::Motor::GetClassName(void)
{
	return "TACO::Motor";
}

const char *TACO::Motor::Motor::GetDevType()
{
	return "Motor";
}

void TACO::Motor::Motor::v_Init() throw (::TACO::Exception)
{
	try
	{
		m_speedMax = 100;
		m_motor = new ::TACO::Motor::Worker();
	}
	catch (::TACO::Exception &e)
	{
		throw e;
	}
}

TACO::Motor::Motor::~Motor() throw ()
{
	if (m_motor)
		delete(m_motor);
	m_motor = NULL;
}

#if 0
void TACO::Motor::Motor::deviceReset() throw (::TACO::Exception)
{
        try
        {
		if (!m_motor->isMoving())
		{
// read all values from card
			stop();
//              	m_motor->reset();
			m_motor->doReset();
// Reset the card values to the user values
			if (m_switch != smsipcmotor::NONE)
        			updateResource<DevDouble>("lastpos", queryResource<DevDouble>("refpos"));
			m_motor->setMaxSoftPos(queryResource<DevDouble>("usermax"));
			m_motor->setMinSoftPos(queryResource<DevDouble>("usermin"));
			std::string tmpString = TACO::toLowerCase(queryResource<std::string>("direction"));
		}
        }
        catch (TACO::Exception &e)
        {
                throw __FUNCTION__ >> e;
        }
}
#endif

void TACO::Motor::Motor::stop() throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->stop();
        updateResource<DevDouble>("lastpos", this->read());
}

void TACO::Motor::Motor::tacoStop(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->stop();
	else 
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::abort() throw (::TACO::Exception)
{
	if (!m_motor)
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
        m_motor->abort();
        updateResource<DevDouble>("lastpos", this->read());
}

void TACO::Motor::Motor::tacoAbort(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception)
{
	Motor* s = dynamic_cast<Motor*>(server);
	if (s != 0)
		s->abort();
	else 
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::move( DevDouble input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->move(input);
}

void TACO::Motor::Motor::tacoMove( ::TACO::Server* server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0)
		s->move(::TACO::convert(static_cast<DevDouble*>(argin)));
	else 
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::moveTo( DevDouble input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->moveTo(input);
}

void TACO::Motor::Motor::tacoMoveTo( ::TACO::Server* server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0)
		s->moveTo(::TACO::convert(static_cast<DevDouble*>(argin)));
	else 
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::setpos(DevDouble input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->setpos(input);
        updateResource<DevDouble>("lastpos", this->read());
}

void TACO::Motor::Motor::tacoSetpos(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->setpos(::TACO::convert(static_cast<DevDouble*>(argin)));
	else 
		throw ::TACO::Exception( ::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::setSpeed(DevDouble input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	if (input <= 0)
		throw_exception(::TACO::Error::RANGE_ERROR, "requested speed is to low (<= 0)");
	if (input > m_speedMax)
		throw_exception(::TACO::Error::RANGE_ERROR, "requested speed is to high (> speedMax)");
	m_motor->setSpeed(input);
}

void TACO::Motor::Motor::tacoSetSpeed(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0)
		s->setSpeed(::TACO::convert(static_cast<DevDouble*>(argin)));
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

DevDouble TACO::Motor::Motor::speed(void ) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	return m_motor->speed();
}

void TACO::Motor::Motor::tacoSpeed(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		::TACO::assign(static_cast<DevDouble*>(argout), s->speed());
	else 
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

DevDouble TACO::Motor::Motor::read(void) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	return m_motor->read();
}

void TACO::Motor::Motor::tacoRead(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		::TACO::assign(static_cast<DevDouble*>(argout), s->read());
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::setAccSlope(const std::string &input) throw (::TACO::Exception)
{
}

void TACO::Motor::Motor::tacoSetAccelerationSlope(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->setAccSlope(::TACO::convert( static_cast<DevVarCharArray*>(argin)));
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

std::string TACO::Motor::Motor::accSlope(void) throw (::TACO::Exception)
{
	return "linear";
}

void TACO::Motor::Motor::tacoAccelerationSlope(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		::TACO::assign(static_cast<DevVarCharArray*>(argout), s->accSlope());
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::moveToReference(void) throw (::TACO::Exception)
{
}

void TACO::Motor::Motor::tacoMoveReference(::TACO::Server *server, DevArgument, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->moveToReference();
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

DevDouble TACO::Motor::Motor::acceleration(void) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	return m_motor->acceleration();
}

void TACO::Motor::Motor::tacoAcceleration(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		::TACO::assign(static_cast<DevDouble*>(argout), s->acceleration());
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::setAcceleration(DevDouble input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->setAcceleration(input);
}

void TACO::Motor::Motor::tacoSetAcceleration(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->setAcceleration(::TACO::convert(static_cast<DevDouble*>(argin)));
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::setUnit(const std::string &input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->setUnit(input);
}

void TACO::Motor::Motor::tacoSetUnit(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->setUnit(::TACO::convert( static_cast<DevVarCharArray*>(argin)));
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

DevDouble TACO::Motor::Motor::backlash(void) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	return m_motor->backlash();
}

void TACO::Motor::Motor::tacoReadBacklash(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		::TACO::assign(static_cast<DevDouble*>(argout), s->backlash());
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

void TACO::Motor::Motor::setBacklash(DevDouble input) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	m_motor->setBacklash(input);
}

void TACO::Motor::Motor::tacoSetBacklash(::TACO::Server *server, DevArgument argin, DevArgument) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		s->setBacklash(::TACO::convert(static_cast<DevDouble*>(argin)));
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

std::string TACO::Motor::Motor::unit(void ) throw (::TACO::Exception)
{
	if (!m_motor)
		throw_exception(::TACO::Error::INTERNAL_ERROR, "motor worker not initialised");
	return m_motor->unit();
}

void TACO::Motor::Motor::tacoUnit(::TACO::Server *server, DevArgument, DevArgument argout) throw (::TACO::Exception)
{
	Motor *s = dynamic_cast<Motor*>(server);
	if (s != 0) 
		::TACO::assign( static_cast<DevVarCharArray*>(argout), s->unit());
	else
		throw ::TACO::Exception(::TACO::Error::INTERNAL_ERROR, "bad dynamic cast");
}

DevShort TACO::Motor::Motor::deviceState(void) throw (::TACO::Exception)
{
	if (m_motor)
		setDeviceState(m_motor->state());
	else
		setDeviceState(DEVON_NOT_REACHED);
	return ::TACO::Server::deviceState();
}


#if 0
void TACO::Motor::Motor::deviceQueryResource() throw (TACO::Exception)
{
        TACO::IO::AnalogOutput::deviceQueryResource();
}

void TACO::Motor::Motor::deviceUpdate() throw (TACO::Exception)
{
        TACO::IO::AnalogOutput::deviceUpdate();
}
#endif



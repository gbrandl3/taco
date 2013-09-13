// collection of general purpose input and output devices
// Copyright (C) 2006-2013 Sebastian Huber

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

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TACOBasicCommands.h>

// TACODEVEL CODEGEN INCLUDES END

#include "Starter.h"
#include "StarterCmds.h"
#include "StarterCommon.h"

Starter::Starter( const std::string& name, long access, bool connect) throw (::TACO::Exception)
	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	: ::TACO::Client( name, access, connect)

	// TACODEVEL CODEGEN BASE CLASS CONSTRUCTOR CALLS END

	/* , MyFirstBaseClass(), MySecondBaseClass(), ... */
{
	// TACODEVEL CODEGEN CONSTRUCTOR CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	addDeviceType( STARTER_ID);
	if (isClientConnected()) {
		checkDeviceType( STARTER_ID);
	}

	// TACODEVEL CODEGEN CONSTRUCTOR CODE END
}

Starter::Starter() throw ()
	: ::TACO::Client()
{
	// TACODEVEL CODEGEN DEFAULT CONSTRUCTOR CODE BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	addDeviceType( STARTER_ID);

	// TACODEVEL CODEGEN DEFAULT CONSTRUCTOR CODE END
}

Starter::~Starter() throw ()
{
	// VOID
}

// TACODEVEL CODEGEN METHOD DEFINITIONS BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.
#if 0
DevULong IO::DigitalInput::read() throw (::TACO::Exception)
{
	return execute<DevULong>( ::TACO::Command::READ_U_LONG);
}
#endif

void Starter::run(const std::vector<std::string> &input) throw (::TACO::Exception)
{
	execute<void>(DevRun, input);
}

void Starter::stop(const std::vector<std::string> &input) throw (::TACO::Exception)
{
	execute<void>(DevStop, input);
}

void Starter::restore(const std::vector<std::string> &input) throw (::TACO::Exception)
{
	execute<void>(DevRestore, input);
}

void Starter::startAll(const DevUShort input) throw (::TACO::Exception)
{
	execute<void>(DevStartAll, input);
}

void Starter::stopAll(const DevUShort input) throw (::TACO::Exception)
{
	execute<void>(DevStopAll, input);
}

std::vector<std::string> Starter::getRunningServers(const bool input) throw (::TACO::Exception)
{
	return execute<std::vector<std::string> >(DevGetRunningServers, input);
}

std::vector<std::string> Starter::getStoppedServers(const bool input) throw (::TACO::Exception)
{
	return execute<std::vector<std::string> >(DevGetStoppedServers, input);
}

void Starter::start(const std::vector<std::string> &input) throw (::TACO::Exception)
{
	execute<void>(DevStart, input);
}

std::string Starter::readLog(const std::string &input) throw (::TACO::Exception)
{
	execute<std::string>(DevReadLog, input);
}

void Starter::updateServerInfo(void) throw (::TACO::Exception)
{
	execute<void>(UpdateServerInfo);
}

// TACODEVEL CODEGEN METHOD DEFINITIONS END

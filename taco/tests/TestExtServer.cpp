// Test ext server
// Copyright (C) 2006 Jens Krüger

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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <TACOConverters.h>
#include <TACOBasicCommands.h>
#include <TestExtServer.h>

Test::Ext::Server::Server( const std::string& name, long& error) throw (::TACO::Exception)
	: ::TACO::Server( name, error)
{
	setDeviceVersion( VERSION );
	setServerName("TestExtServer");
	v_Init();
}

Test::Ext::Server::~Server() throw ()
{
}

const char *Test::Ext::Server::GetClassName(void)
{
	return "Test::Ext::Server";
}

void Test::Ext::Server::deviceOff(void) throw (::TACO::Exception)
{
	Server::deviceOff();
}

void Test::Ext::Server::deviceOn(void)
	throw (::TACO::Exception)
{
	Server::deviceOn(  );
}

short Test::Ext::Server::deviceState( void ) throw (::TACO::Exception)
{
	return Server::deviceState(  );
}

void Test::Ext::Server::deviceUpdate(void) throw (::TACO::Exception)
{
}

void Test::Ext::Server::v_Init(void) throw (::TACO::Exception)
{
	std::cout << deviceName() << ": init complete." << std::endl;
}

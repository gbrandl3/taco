/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 ESRF, www.esrf.fr
 * Copyright (C) 2006-2013 Jens Krüger <jkrueger1@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:        TestExtServer.cpp
 *
 * Description: Code for TestExtServer test program
 *              Synopsis : TestExtServer <personal name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-30 13:42:46 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif


#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif // HAVE_CONFIG_H

#include <TestExtServer.h>
#include <TACOConverters.h>
#include <TACOBasicCommands.h>

Test::Ext::Server::Server( const std::string &name, DevLong &error) throw (::TACO::Exception)
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
	::TACO::Server::deviceOff();
}

void Test::Ext::Server::deviceOn(void)
	throw (::TACO::Exception)
{
	::TACO::Server::deviceOn(  );
}

short Test::Ext::Server::deviceState( void ) throw (::TACO::Exception)
{
	return ::TACO::Server::deviceState(  );
}

void Test::Ext::Server::deviceUpdate(void) throw (::TACO::Exception)
{
}

void Test::Ext::Server::v_Init(void) throw (::TACO::Exception)
{
	std::cout << deviceName() << ": init complete." << std::endl;
}

void Test::Ext::Server::deviceReset(void) throw (::TACO::Exception)
{
	try
	{
		throw ::TACO::Exception(::TACO::Error::COMMAND_NOT_IMPLEMENTED);
	}
	catch (::TACO::Exception &e)
	{
		::TACO::Exception eh = "deviceReset() : " >> e;
		throw ::TACO::Exception(::TACO::Error::RUNTIME_ERROR, eh.what());
	}
}

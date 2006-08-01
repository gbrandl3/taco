/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2004 Sebastian Huber <sebastian-huber@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>

#include <API.h>

#include <TACOException.h>

// It is important to include ApiP.h at the end
#if HAVE_APIP_H
# 	include <ApiP.h>
#else 
# 	include <private/ApiP.h>
#endif

std::string TACO::errorString( DevLong errorNumber) throw ()
{
	char* tmp = dev_error_str( errorNumber);
	std::string r;
	if (tmp != 0) {
		r = tmp;
		free( tmp);
	}
	return r;
}

std::string TACO::plainErrorString( DevLong errorNumber) throw ()
{
	char* tmp = dev_error_str( errorNumber);
	std::string r;
	if (tmp != 0) {
		r = tmp;
		free( tmp);
		r.erase( r.begin(), r.begin() + 25);
	}
	return r;
}

void TACO::pushErrorMessage( const std::string& msg) throw ()
{
	dev_error_push( const_cast<char*>( msg.c_str()));
}

TACO::Exception& operator>>( const std::string& msg, TACO::Exception& e) throw ()
{
	e.mErrorDescription = msg + e.mErrorDescription;
	return e;
}

TACO::Exception::Exception( DevLong errorNumber) throw ()
	: mErrorNumber( errorNumber), mErrorDescription( TACO::plainErrorString( errorNumber))
{
	// VOID
}

TACO::Exception::Exception( DevLong errorNumber, const std::string& errorDescription) throw ()
	: mErrorNumber( errorNumber), mErrorDescription( errorDescription)
{
	// VOID
}

TACO::Exception::Exception( const TACO::Exception& e) throw ()
	: std::exception(), mErrorNumber( e.mErrorNumber), mErrorDescription( e.mErrorDescription)
{
	// VOID
}

TACO::Exception& TACO::Exception::operator=( const TACO::Exception& e) throw ()
{
	mErrorNumber = e.mErrorNumber;
	mErrorDescription = e.mErrorDescription;
	return *this;
}

TACO::Exception& TACO::Exception::operator<<( const std::string& msg) throw ()
{
	mErrorDescription += msg;
	return *this;
}

const char* TACO::Exception::what() const throw ()
{
	return mErrorDescription.c_str();
}

/*
 * Extensions for the convenient access to TACO
 * Copyright (C) 2002-2014 Sebastian Huber <sebastian-huber@web.de>
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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>

#include <API.h>

#include "TACOException.h"

#ifdef HAVE_PTHREAD_H
#	include <pthread.h>
#endif

// It is important to include ApiP.h at the end
#if HAVE_APIP_H
# 	include <ApiP.h>
#else 
# 	include <private/ApiP.h>
#endif

namespace TACO {
	//! Mutex
	extern pthread_mutex_t mMutex;
}

std::string TACO::errorString( DevLong errorNumber) throw ()
{
	pthread_mutex_lock(&::TACO::mMutex);
	char* tmp = dev_error_str(errorNumber);
	std::string r;
	if (tmp != 0) {
		r = tmp;
		free( tmp);
	}
	pthread_mutex_unlock(&::TACO::mMutex);
	return r;
}

std::string TACO::plainErrorString( DevLong errorNumber) throw ()
{
	pthread_mutex_lock(&::TACO::mMutex);
	char* tmp = dev_error_str(errorNumber);
	std::string r;
	if (tmp != 0) {
		r = tmp;
		free( tmp);
		r.erase( r.begin(), r.begin() + 25);
	}
	pthread_mutex_unlock(&::TACO::mMutex);
	return r;
}

void TACO::pushErrorMessage(const std::string& msg) throw ()
{
	dev_error_push( const_cast<char*>( msg.c_str()));
}

TACO::Exception& operator>>( const std::string& msg, TACO::Exception& e) throw ()
{
	e = TACO::Exception(DevLong(e), msg + e.what());
	return e;
}

TACO::Exception::Exception( DevLong errorNumber) throw ()
	: mErrorNumber( errorNumber), mErrorDescription( TACO::plainErrorString( errorNumber))
{
	// std::cerr << "DevLong :" << what() << std::endl;
}

TACO::Exception::Exception( DevLong errorNumber, const std::string& errorDescription) throw ()
	: mErrorNumber( errorNumber), mErrorDescription( errorDescription)
{
	// std::cerr << "DevLong, const std::string& " << what() << std::endl;
}

TACO::Exception::Exception( const TACO::Exception& e) throw ()
	: std::exception(), mErrorNumber( e.mErrorNumber), mErrorDescription( e.mErrorDescription)
{
	// std::cerr << "const TACO::Exception& " << what() << std::endl;
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

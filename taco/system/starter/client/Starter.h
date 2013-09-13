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

#ifndef STARTER_H
#define STARTER_H

// TACODEVEL CODEGEN INCLUDES BEGIN
// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

#include <TACOClient.h>

// TACODEVEL CODEGEN INCLUDES END

#ifdef SWIGPYTHON
#define IODigitalInput IO::DigitalInput
#endif // SWIGPYTHON

class Starter;

#include "StarterCmds.h"

/**
 * @brief This class realises the access to a digital input device.
 * A digital input device is chararised by the fact that its main function is to read a physical value
 * which has only descrete (integer) values.
 */
class Starter
	// TACODEVEL CODEGEN BASE CLASSES BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

	: public ::TACO::Client

	// TACODEVEL CODEGEN BASE CLASSES END

	/* , MyFirstBaseClass, MySecondBaseClass, ... */ {
public:
        /**
         * Constructor.
         * @param name TACO device name
         * @param access access level to the device
         * @param connect auto connect to the device
         * @throw TACO::Exception in case of failure
         */
	Starter( const std::string& name, long access = 0, bool connect = true) throw (::TACO::Exception);

        /**
         * Default constructor
         */
	Starter() throw ();

        /**
         * Destructor
         */
 	~Starter() throw ();

	/**
	 * @fn virtual DevULong read() throw (::TACO::Exception)
         * This function reads the mean value of the device. The mean value is integer and may be have all
         * values between a minimum and maximum depending on the hardware device and the resolution of it.
         * @return current value
         * @throw TACO::Exception in case of failure
         */
	
	// TACODEVEL CODEGEN METHOD DECLARATIONS BEGIN
	// This is an automatically generated block.  Do not edit it.  Any modification may be lost.

        virtual void run(const std::vector<std::string> &) throw (::TACO::Exception);
        
	virtual void stop(const std::vector<std::string> &) throw (::TACO::Exception);

        virtual void restore(const std::vector<std::string> &) throw (::TACO::Exception);

	virtual void startAll(const DevUShort) throw (::TACO::Exception);

	virtual void stopAll(const DevUShort) throw (::TACO::Exception);

	virtual std::vector<std::string> getRunningServers(const bool ) throw (::TACO::Exception);
					
	virtual std::vector<std::string> getStoppedServers(const bool ) throw (::TACO::Exception);
					
	virtual void start(const std::vector<std::string> &) throw (::TACO::Exception);

	virtual std::string readLog(const std::string &) throw (::TACO::Exception);
					
	virtual void updateServerInfo() throw (::TACO::Exception);

	// TACODEVEL CODEGEN METHOD DECLARATIONS END
};

#endif // I_O_DIGITAL_INPUT_H

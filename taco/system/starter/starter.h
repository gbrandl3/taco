/*****************************************************************************
 * Copyright (C) 2003 Jens Krueger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * File:        $RCSfile: starter.h,v $
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Source code for implementing a starter server
 *
 * Author(s):   Jens Krüger
 * 		$Author: jkrueger1 $
 *
 * Original:    January 2003
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2006-09-19 08:45:02 $
 *
 *****************************************************************************/

#ifndef __STARTER_DEVICE_H__
#define __STARTER_DEVICE_H__

#include <API.h>
#include <Device.h>
#include <sys/types.h>
#include <string>

class StarterDevice : public Device
{
	friend DevLong StarterSetup(char *pszServerName, DevLong *plError);
public:
	StarterDevice(std::string name, DevLong &error);
	~StarterDevice();
protected:
//	DevLong GetResources(char *name, DevLong *error){}
private:
	/**
	 * TACO command for starting Device server.
	 */
	DevLong tacoDevRun(void *argin, void *argout, DevLong *error);

	/**
	 * TACO command for stopping Device server.
	 */
	DevLong tacoDevStop(void *argin, void *argout, DevLong *error);

	/**
	 * TACO command for restarting Device server.
	 */
	DevLong tacoDevRestart(void *argin, void *argout, DevLong *error); 

public:
	/**
	 * Implementation of TACO command DevRun
	 */
	void deviceRun(const std::string proc, const std::string pers, const std::string option, const std::string param);	
	
	/**
	 * Implementation of TACO command DevStop.
	 */
	void deviceStop(const std::string proc, const std::string pers);

	/**
	 * Implementation of TACO command DevRestart.
	 */
	void deviceReStart(const std::string proc, const std::string pers, const std::string option, const std::string param);	

private:
	pid_t getpid(const std::string proc, const std::string pers);
	
};


#endif

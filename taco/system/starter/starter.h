/*****************************************************************************
 *
 * File:        $Id: starter.h,v 1.4 2003-12-08 15:17:42 jkrueger1 Exp $
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
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2003-12-08 15:17:42 $
 *
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
 *****************************************************************************/

#ifndef __STARTER_DEVICE_H__
#define __STARTER_DEVICE_H__

#include <API.h>
#include <Device.H>
#include <sys/types.h>
#include <string>

class StarterDevice : public Device
{
	friend long StarterSetup(char *pszServerName, long *plError);
public:
	StarterDevice(std::string name, long &error);
	~StarterDevice();
protected:
//	long GetResources(char *name, long *error){}
private:
	/**
	 * TACO command for starting Device server.
	 */
	long tacoDevRun(void *argin, void *argout, long *error);

	/**
	 * TACO command for stopping Device server.
	 */
	long tacoDevStop(void *argin, void *argout, long *error);

	/**
	 * TACO command for restarting Device server.
	 */
	long tacoDevRestart(void *argin, void *argout, long *error); 

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
	pid_t StarterDevice::getpid(const std::string proc, const std::string pers);
	
};


#endif

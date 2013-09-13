/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
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
 * File:        CSHost.h
 *
 * Project:     System test
 *
 * Description: include file containing definitions and declarations
 *              for implementing the CS Host class
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jkrueger1 $
 *
 * Original:    
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2008-04-06 09:07:55 $
 *
 +**********************************************************************/

#ifndef _CSHOST_H
#define _CSHOST_H

#include <string>
#include <DeviceServer.h>

typedef struct 
{
	DeviceServer	ds;
	long		ds_embedded;
	std::string	*ds_list;
}_HostDeviceServer;


class CSHost
{
private:
	std::string host_name;
	long ds_nb;
	_HostDeviceServer *ds_array;
	long host_alive;

public:
	long get_ds_nb() const;
	long get_alive_flag() const;
	long test_ds_on_host(const int = 0,const long = 0) const;
	void get_host_name(std::string &) const;

// Constructors

	CSHost();
	CSHost(const std::string &);
	CSHost(const CSHost &);

// Destructor

	~CSHost();

// Operator overloading

	CSHost & operator=(const CSHost &);
};

// For the RTDB C library

extern "C"
{
void get_ds_host(...);
void close_db(...);
}

#endif // _CSHOST_H

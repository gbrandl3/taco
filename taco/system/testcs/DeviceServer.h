/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
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
 * File:        DeviceServer.h
 *
 * Project:     System test
 *
 * Description: include file containing definitions and declarations
 *              for implementing the DeviceServer class
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

#ifndef _DEVICESERVER_H
#define _DEVICESERVER_H

#include <string>
#include <time.h>


class DeviceServer
{
private:
	unsigned int pn;
	int pid;
	long vers_nb;
	unsigned long *vers_array;
	std::string host;
	std::string DS_name;
	std::string DS_pers_name;
	std::string DS_proc_name;
	std::string DS_check_answer;

public:
	int test_ds_udp(const long);
	int test_ds_tcp(const long);
	long get_vers_nb() const;
	long get_pid() const;
	long get_vers(const long = 0) const;
	void get_check_answer(std::string &) const;
	void get_host(std::string &) const;
	void get_ds_process_name(std::string &) const;

// Constructors

	DeviceServer();
	DeviceServer(const std::string &, const std::string &);
	DeviceServer(const DeviceServer &);

// Destructor

	~DeviceServer();

// Operator overloading

	DeviceServer & operator=(const DeviceServer &);
};

static struct timeval ds_tout = {6,0};
static struct timeval vers_tout = {1,0};
static struct timeval ds_retry_tout = {2,0};

// For the RTDB or NDBM C library

extern "C"
{
void db_get_ds(...);
void close_db(...);
}

#endif // _DEVICESERVER_H

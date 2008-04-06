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
 * File:        KernelServer.h
 *
 * Project:     System test
 *
 * Description: include file containing definitions and declarations
 *              for implementing the KernelServer class
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

#ifndef _KERNELSERVER_H
#define _KERNELSERVER_H

#include <string.h>

#ifdef sun
#include <sys/time.h>
#else
#include <time.h>
#endif

class KernelServer
{
private:
	std::string host;
	unsigned int pn;

public:
	int test_server_udp(const int) const;
	int test_server_tcp(const int) const;

// Constructors

	KernelServer(const int = 0,const int = 0,char * = 0);
	KernelServer(const KernelServer &);

// Destructor

	~KernelServer();

// Operator overloading

	KernelServer & operator=(const KernelServer &);
};

#define MANAGER		1
#define DATABASE 	2
#define DC_RD 		3
#define DC_WR 		4

#define READ 		0
#define WRITE		1

static struct timeval tout = {6,0};
static struct timeval retry_tout = {2,0};

#endif // _KERNELSERVER_H

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
 * File:        EsrfControlSystem.h
 *
 * Project:     System test
 *
 * Description: include file containing definitions and declarations
 *              for implementing the EsrfControlSystem class
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

#ifndef _ESRFCONTROLSYSTEM_H
#define _ESRFCONTROLSYSTEM_H

#include <CSHost.h>

class EsrfControlSystem
{
private:
	long host_nb;
	long ds_process;
	long host_dead;
	CSHost *host_array;

public:
	long get_ds_process_nb() const;
	long get_host_nb() const;
	long get_dead_nb() const;
	void test_cs(const long = 0) const;

// Constructors

	EsrfControlSystem();
	EsrfControlSystem(const EsrfControlSystem &);

// Destructor

	~EsrfControlSystem();

// Operator overloading

	EsrfControlSystem & operator=(const EsrfControlSystem &);
};

// Some function definition

void sort_name(char **,long);

// For the RTDB C library

extern "C"
{
void get_cs_host(...);
void close_db(...);
}

#endif // _ESRFCONTROLSYSTEM_H

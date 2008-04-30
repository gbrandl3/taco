/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 ESRF, www.esrf.fr
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
 * File:        db_connect.cpp
 *
 * Description: program for testing the connection to the database server
 *              Synopsis : db_connect 
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.6 $
 *
 * Date:        $Date: 2008-04-30 13:42:46 $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/* TACO include file */
#include <API.h>

/* Include files */
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif
#include <iostream>
#include <string>
#include <algorithm>
#include <errno.h>

/*
 ***************************************************************************
 * Code for db_connect command. 
 * Synopsis : db_connect 
 ***************************************************************************
 */
int main(int argc, char *argv[])
{
	DevLong error;
//
// Connect to database server
//
	if (db_import(&error) == -1)
	{
		std::cerr << *argv << " : Impossible to connect to database server" << std::endl;
		std::cerr << dev_error_str(errno) << std::endl;
		exit(-1);
	}
	return 0;
}

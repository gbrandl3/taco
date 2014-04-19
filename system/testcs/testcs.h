/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:        testcs.h
 *
 * Project:     System test
 *
 * Description: include file containing definitions and declarations
 *              for testcs program
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jkrueger1 $
 *
 * Original:    June 1996
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2008-04-06 09:07:56 $
 *
 +**********************************************************************/

#ifndef _TESTCS_H
#define _TESTCS_H

// Some define

#define True 1
#define False 0

// Some structure

typedef struct _dc_serv_info {
	int 	rd;
	int 	wr;
	char	host[80];
	}dc_serv_info;

// Function prototyping

void errprint(const char *);
void test_kernel(long);
int is_there_a_dc(int *,dc_serv_info **);
void test_ds(std::string &,long,long);
void test_host(std::string &,long);
void test_all(long);
void test_ud(long);

// Some C function used in this program

extern "C"
{
void open_db(...);
void close_db(...);
void get_ds_instance(...);
}

#endif

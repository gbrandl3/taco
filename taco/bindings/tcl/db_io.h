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
 * File:        db_io.h	
 *
 * Project:     TACO TCL bindings
 *
 * Description: This is the include file for the new commands
 *	  	inside a tcl interpreter named db_io_get, 
 *	  	db_io_put and db_io_debug
 *
 * Author(s): 	Denis Beauvois 	
 *		Gilbert Pepellin
 *		J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original: 	June, 1996
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-18 22:04:49 $
 */

#ifndef DB_IO_H
#define DB_IO_H	

#include <API.h>
#include <tcl.h>

/* Number of types recognized by db_io_get */
#define NUMBER_OF_TYPES_KNOWN 11

/* Number of arguments required */
#define NUMBER_ARGUMENTS 6

/* Max length of the variable arrays */
#define MAX_LENGTH_VAR 100

/* Debug mode */
#define DB_DEBUG_0 0
#define DB_DEBUG_1 1

#ifndef False
#define False 0
#endif

#ifndef True
#define True 1
#endif

typedef struct DEVICE_KNOWN {
                             DevType Type;
                             char *Name;
                            } DEVICE_KNOWN;
                            
/* Prototypes of functions */
int SetDBDebugMode ();
int GetDBDebugMode ();
DevType GetType();


#endif

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
 * File	: 	dc_io.h	
 *
 * Project:     TACO TCL bindings
 *
 * Description: 
 *
 * Author(s): 	Denis Beauvois 	
 * 		Gilbert Pepellin
 * 		J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original: 	June, 1996
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2006-09-18 22:04:49 $
 */


#ifndef DC_IO_H
#define DC_IO_H 0

#include <API.h>
#include <dc.h>

#include <tcl.h>

#include <dev_io.h>

/* ---------------- */
/* general defines  */
/* ---------------- */

/* debug mode */
#define DC_DEBUG_0 0
#define DC_DEBUG_1 1

/* Call with history mode */
#define DC_HISTORY_MODE_OFF 0
#define DC_HISTORY_MODE_ON 1

/* Number of arguments in the command line */
#define DC_ARGUMENTS_NUMBER 5


/* For keeping in memory the list of imported device */
typedef struct DC_IMPORTED_DEVICE {
                                   char *Name;
                                   datco dc;
                                   struct DC_IMPORTED_DEVICE *Next;
                                  } DC_IMPORTED_DEVICE;

#endif		

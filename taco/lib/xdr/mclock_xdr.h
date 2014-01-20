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
 * File:	mclock_xdr.h
 *
 * Description: XDR data type definition  
 *  		declaration of xdr functions , declaration of xdr length calculation 
 *  		functions , definition for data type number and definition of the
 *  		load macro.
 *
 * Author(s):
 *		$Author: jkrueger1 $
 * 
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:07:22 $
 *
 **************************************************************************/

#ifndef MCLOCK_XDR_H
#define MCLOCK_XDR_H

#include <SysNumbers.h>


/**************************************************************************/

/*  C type definition  */
struct DevTm
       {
	 DevShort tm_year;
	 DevShort tm_mon;
	 DevShort tm_mday;
	 DevShort tm_hour;
	 DevShort tm_min;
	 DevShort tm_sec;
	 DevShort tm_wday;
	 DevShort tm_yday;
	 DevShort tm_isdst;
       };
typedef struct	DevTm	DevTm;

/*  declaration of xdr functions  */
bool_t	xdr_DevTm();

/*  declaration of xdr length calculation function  */
long 	xdr_length_DevTm();



/**************************************************************************/

/*  C type definition  */
struct DevStructMasterClock
       {
	 DevString 	*convert_dh;
	 DevLong 	nb_sec;
	 DevTm 		*timeptr;
       };
typedef	struct	DevStructMasterClock	DevStructMasterClock;

/*  declaration of xdr functions  */
bool_t	xdr_DevStructMasterClock();

/*  declaration of xdr length calculation function  */
long	xdr_length_DevStructMasterClock();

/*  definition of the data type number  */
#define	D_STRUCT_MCLOCK	DevMClockBase + 1

/*  definition of the load macro  */
#define	LOAD_STRUCT_MCLOCK(A)	xdr_load_type(D_STRUCT_MCLOCK, \
					 xdr_DevStructMasterClock, \
					 sizeof(DevStructMasterClock), \
					 xdr_length_DevStructMasterClock, \
					 A)

#endif

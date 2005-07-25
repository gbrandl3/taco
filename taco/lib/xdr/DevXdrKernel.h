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
 * File:        DevXdrKernel.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Public include file for all applications
 *		using the standard device server XDR types.
 *		Only the minimal set of types defined in the 
 *		ESRF control system kernel.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	November 1993
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef DEVXDRKERNEL_H
#define DEVXDRKERNEL_H

/*
 *  standart header string to use the "what" or "ident".
 */
#ifdef _IDENT
static char DevXdrKernelh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/DevXdrKernel.h,v 1.3 2005-07-25 13:05:45 jkrueger1 Exp $";
#endif /* _IDENT */


#include <DevErrors.h>

/*
 * include XDR data types for the kernel.
 */
#include <API_xdr.h>
#include <dev_xdr.h>


/* 
 * Define the XDR data type list structure and its size.
 */

#ifdef __cplusplus
extern "C" {
typedef bool_t (*DevDataFunction)(...);
typedef long   (*DevDataLengthFunction)(...);
}
#else
typedef bool_t (*DevDataFunction)();
typedef long   (*DevDataLengthFunction)();
#endif /* __cplusplus */
typedef long   DevType;

#define MAX_NUMBER_OF_XDR_TYPES		150
#define NUMBER_OF_GENERAL_XDR_TYPES	100

typedef struct _DevDataListEntry {
											long                   type;
                                 DevDataFunction        xdr;
											long                   size;
											DevDataLengthFunction  xdr_length;
                                 } DevDataListEntry;

/*
 * Function definitions
 */

#ifndef __cplusplus
/*
 * OIC version
 */
extern long _DLLFunc xdr_load_type
		  	PT_( (long type, DevDataFunction xdr,
			      long size, DevDataLengthFunction xdr_length,
			      long *error) );
extern long _DLLFunc xdr_get_type
		     	PT_( (long type, DevDataListEntry *xdr_type,
			      long *error) );
extern long _DLLFunc xdr_load_kernel
		     	PT_( (long *error) );
#else
/*
 * C++ version
 */
extern "C" long _DLLFunc xdr_load_type
		  	PT_( (long type, DevDataFunction xdr,
			      long size, DevDataLengthFunction xdr_length,
			      long *error) );
extern "C" long _DLLFunc  xdr_get_type
		     	PT_( (long type, DevDataListEntry *xdr_type,
			      long *error) );
extern "C" long _DLLFunc  xdr_load_kernel
		     	PT_( (long *error) );

#endif /* __cplusplus */


#endif /* _DevXdrKernel_h */


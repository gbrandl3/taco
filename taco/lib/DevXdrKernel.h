
/*+*******************************************************************

 File:          DevXdrKernel.h

 Project:       Device Servers with sun-rpc

 Description:   Public include file for all applications
		using the standard device server XDR types.
		Only the minimal set of types defined in the 
		ESRF control system kernel.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	November 1993

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _DevXdrKernel_h
#define _DevXdrKernel_h

/*
 *  standart header string to use the "what" or "ident".
 */
#ifdef _IDENT
static char DevXdrKernelh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevXdrKernel.h,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";
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


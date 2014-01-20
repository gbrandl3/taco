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
 * File:        icv101_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions
 *		of the icv101 data types.
 *
 * Author(s):  	Timo Mettala
 * 		$Author: jkrueger1 $
 *
 * Original:	March 1993
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifndef ICV101_XDR_H
#define ICV101_XDR_H

/*
 *  definitions for icv101 type D_VAR_PATTERNARR
 */

struct DevPattern {
	DevShort  channel;
	DevShort  gain;
	DevShort  end;
};
typedef struct DevPattern 	DevPattern;
bool_t 	_DLLFunc xdr_DevPattern PT_((XDR *xdrs, DevPattern *objp));


struct DevVarPatternArray {
	DevULong	length;
	DevPattern  	*sequence;
};
typedef struct DevVarPatternArray DevVarPatternArray;
bool_t 	_DLLFunc xdr_DevVarPatternArray
		 PT_((XDR *xdrs, DevVarPatternArray *objp));
#define D_VAR_PATTERNARR        56
#define LOAD_VAR_PATTERNARR(A)  xdr_load_type (D_VAR_PATTERNARR, \
                                               xdr_DevVarPatternArray, \
                                               sizeof(DevVarPatternArray), \
                                               NULL, \
                                               A )


/*
 *  definitions for icv101 type D_ICV_MODE
 */

struct DevIcv101Mode {
       DevLong external_stop;
       DevLong external_start;
       DevLong continuous;
       DevLong main_frequency;
       DevLong sub_frequency;
       DevLong npfs2;
       DevLong npost;
};
typedef struct DevIcv101Mode 	DevIcv101Mode;
bool_t 	_DLLFunc xdr_DevIcv101Mode PT_((XDR *xdrs, DevIcv101Mode *objp));
#define D_ICV_MODE              57
#define LOAD_ICV_MODE(A)  	xdr_load_type (D_ICV_MODE, \
                                               xdr_DevIcv101Mode, \
                                               sizeof(DevIcv101Mode), \
                                               NULL, \
                                               A )
	  
#endif /* _icv101_xdr_h */

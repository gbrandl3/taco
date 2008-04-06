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
 * File:        atte_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions 
 *
 * Author(s):   Timo Mettala
 * 		$Author: jkrueger1 $
 *
 * Original:    November 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:20 $
 *
 *******************************************************************-*/

#ifndef ATTE_XDR_H
#define ATTE_XDR_H

struct DevAtte {
		 DevShort number;
		 DevShort filter;
	};
typedef struct DevAtte 		DevAtte;
bool_t 				_DLLFunc xdr_DevAtte PT_((XDR *xdrs, DevAtte *objp));
long                            _DLLFunc xdr_length_DevAtte PT_((DevAtte *objp));
#define D_ATTE_TYPE            	53
#define LOAD_ATTE_TYPE(A)      	xdr_load_type ( D_ATTE_TYPE, \
                                                xdr_DevAtte, \
                                                sizeof(DevAtte), \
                                                xdr_length_DevAtte, \
                                                A )

#endif /* _atte_xdr_h */

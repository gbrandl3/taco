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
 * File:        gpib_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file for XDR definitions 
 *		of the GPIB data types.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:21 $
 *
 *******************************************************************-*/

#ifndef GPIB_XDR_H
#define GPIB_XDR_H

/*
 * definitions for Gpib
 */
struct DevGpibWrite {
	DevShort  number;
	DevString msg_str;
};
typedef struct DevGpibWrite 	DevGpibWrite;
bool_t 	_DLLFunc xdr_DevGpibWrite  PT_((XDR *xdrs, DevGpibWrite *objp));
#define D_GPIB_WRITE            32
#define LOAD_GPIB_WRITE(A)      xdr_load_type ( D_GPIB_WRITE, \
                                                xdr_DevGpibWrite, \
                                                sizeof(DevGpibWrite), \
                                                NULL, \
                                                A )



struct DevGpibMulWrite {
	DevVarShortArray number_arr;
	DevString  	 msg_str;
};
typedef struct DevGpibMulWrite 	DevGpibMulWrite;
bool_t 	_DLLFunc xdr_DevGpibMulWrite
		 PT_((XDR *xdrs, DevGpibMulWrite *objp));
#define D_GPIB_MUL_WRITE        33
#define LOAD_GPIB_MUL_WRITE(A)  xdr_load_type ( D_GPIB_MUL_WRITE, \
                                                xdr_DevGpibMulWrite, \
                                                sizeof(DevGpibMulWrite), \
                                                NULL, \
                                                A )


struct DevGpibRes {
  	DevShort number;  /* Device Number */
      	DevChar  sendDC;
     	DevChar  DoPPoll;
    	DevChar  readMode;
   	DevChar  readEnd1;
 	DevChar  readEnd2;
	DevChar  writeMode;
	DevChar  writeEnd1;
       	DevChar  writeEnd2;
      	DevChar  primAddr;
     	DevChar  readStat;
    	DevChar  readMask;
   	DevChar  res1;
  	DevLong  frameCnt;
};
typedef struct DevGpibRes 	DevGpibRes;
bool_t 	_DLLFunc xdr_DevGpibRes  PT_((XDR *xdrs, DevGpibRes *objp));
#define D_GPIB_RES              37
#define LOAD_GPIB_RES(A)        xdr_load_type ( D_GPIB_RES, \
                                                xdr_DevGpibRes, \
                                                sizeof(DevGpibRes), \
                                                NULL, \
                                                A )


struct DevGpibLoc {
    	DevChar  slave;
      	DevChar  startCtrl;
     	DevChar  fullAddr;
    	DevChar  sendREN;
   	DevChar  serialPoll;
  	DevChar  paraPoll;
 	DevChar  autoRead;
	DevChar  autoSRQ;
      	DevChar  defStat;
      	DevChar  writeStat;
     	DevChar  waitREN;
    	DevChar  res0;
};
typedef struct DevGpibLoc 	DevGpibLoc;
bool_t 	_DLLFunc xdr_DevGpibLoc  PT_((XDR *xdrs, DevGpibLoc *objp));
#define D_GPIB_LOC              38
#define LOAD_GPIB_LOC(A)      	xdr_load_type ( D_GPIB_LOC, \
                                                xdr_DevGpibLoc, \
                                                sizeof(DevGpibLoc), \
                                                NULL, \
                                                A )


#endif /* _gpib_xdr_h */


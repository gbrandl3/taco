/*+*******************************************************************

 File:          gpib_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions 
		of the GPIB data types.

 Author(s):  	Jens Meyer
 		$Author: jkrueger1 $

 Original:	July 1992

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _gpib_xdr_h
#define _gpib_xdr_h


/*
 * definitions for Gpib
 */
struct DevGpibWrite {
	short number;
	char  *msg_str;
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
	char  		 *msg_str;
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
  	short number;  /* Device Number */
      	char  sendDC;
     	char  DoPPoll;
    	char  readMode;
   	char  readEnd1;
 	char  readEnd2;
	char  writeMode;
	char  writeEnd1;
       	char  writeEnd2;
      	char  primAddr;
     	char  readStat;
    	char  readMask;
   	char  res1;
  	long  frameCnt;
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
    	char  slave;
      	char  startCtrl;
     	char  fullAddr;
    	char  sendREN;
   	char  serialPoll;
  	char  paraPoll;
 	char  autoRead;
	char  autoSRQ;
      	char  defStat;
      	char  writeStat;
     	char  waitREN;
    	char  res0;
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


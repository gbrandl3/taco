
/*+*******************************************************************

 File:          atte_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions 

 Author(s):     Timo Mettala

 Original:      November 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:45 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

*******************************************************************-*/
#ifndef _atte_xdr_h
#define _atte_xdr_h


struct DevAtte {
		 short number;
		 short filter;
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

static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/dc/dc_sdev.c,v 1.1 2003-04-25 11:21:43 jkrueger1 Exp $";
/*
 
 Author:	$Author: jkrueger1 $

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-04-25 11:21:43 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                      Grenoble, France

 *  */


#include <API.h>
#include <dc.h>
#include <dcP.h>

#include <DevErrors.h>
#include <DevCmds.h>


#ifdef OSK
#include <strings.h>
#else
#include <stdlib.h>
#include <string.h>
#endif

/* Some variables defined elsewhere */

extern DevDataListEntry DevData_List[];


/****************************************************************************
*                                                                           *
*		dc_dataconvert function code                                *
*               --------------                                              *
*                                                                           *
*    Function rule : To store data belonging to super device into the data  *
*		     collector. Super devices are non physical devices. The *
*		     device name is used because of the analogy with        *
*		     real devices.					    *
*		     command for one device.				    *
*                                                                           *
*    Argins : - 							    *
*                                                                           *
*    Argout : - 							    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the err variable   *
*    pointed to by "error". Otherwise, the function returns 0               *
*                                                                           *
*****************************************************************************/


#ifdef __STDC__
int dc_dataconvert(dc_datacmd *data_cmd,unsigned int num_cmd,dc_error *error)
#else
int dc_dataconvert(data_cmd,num_cmd,error)
dc_datacmd *data_cmd;
unsigned int num_cmd;
dc_error *error;
#endif /* __STDC__ */
{
	int i;
	XDR xdrs;
	int size;
	int buf_size;
	char *buf,*buf1;
	long err;
	DevDataListEntry type_info;
	DevType type;

/* Try to verify the function parameters (non NULL pointer and two
   \ character in the device name) */

	if (data_cmd == NULL || error == NULL)
	{
		error->error_code = DcErr_BadParameters;
		error->dev_error = 0;
		return(-1);
	}
	for (i = 0;i < (int)num_cmd;i++)
	{
		if (data_cmd[i].argout == NULL)
		{
			error->error_code = DcErr_BadParameters;
			error->dev_error = 0;
			return(-1);
		}
	}
	
/* A loop on every pseudo command */

	for (i = 0;i < (int)num_cmd;i++)
	{

/* Compute argout data size */

		type = data_cmd[i].argout_type;
		if (type < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (DevData_List[type].type != type)
			{
				error->error_code = DevErr_XDRLengthCalculationFailed;
				error->dev_error = i + 1;
				return(1);
			}
			size = DevData_List[type].xdr_length(data_cmd[i].argout);
		}
		else
		{
			if (xdr_get_type(type,&type_info,&err) != DS_OK)
			{
				error->error_code = DevErr_XDRLengthCalculationFailed;
				error->dev_error = i + 1;
				return(1);
			}
			size = type_info.xdr_length(data_cmd[i].argout);
		}
		if (size == -1)
		{
			error->error_code = DevErr_XDRLengthCalculationFailed;
			error->dev_error = i + 1;
			return(1);
		}

/* Allocate memory for buffers used for data in XDR format */

		buf_size = size + SEC_MARGIN;
		if ((buf = (char *)malloc(buf_size)) == NULL)
		{
			error->error_code = DcErr_ClientMemoryAllocation;
			error->dev_error = i + 1;
			return(1);
		}
		if ((buf1 = (char *)malloc(buf_size)) == NULL)
		{
			error->error_code = DcErr_ClientMemoryAllocation;
			error->dev_error = i + 1;
			return(1);
		}

/* Create a memory stream to convert argout type to XDR format */

		xdrmem_create(&xdrs,(caddr_t)buf,buf_size,XDR_ENCODE);

/* Convert argout buffer to XDR type 
   If the argout type is one of the general types, directly access the API
   DevData_List array. If not, use the structure sent back by the previous
   call to the xdr_get_type */

		if (type < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (!xdr_pointer(&xdrs,(char **)&data_cmd[i].argout,
 			        	 DevData_List[type].size,
		 		 	(xdrproc_t)DevData_List[type].xdr))
			{
		 		free(buf);
				free(buf1);
		 		error->error_code = DcErr_CantConvertDataToXDRFormat;
				error->dev_error = i + 1;
		 		return(1);
			}
		}
		else
		{
			if (!xdr_pointer(&xdrs,(char **)&data_cmd[i].argout,
 			         	 type_info.size,(xdrproc_t)type_info.xdr))
			{
		 		free(buf);
				free(buf1);
		 		error->error_code = DcErr_CantConvertDataToXDRFormat;
				error->dev_error = i + 1;
		 		return(1);
			}
		}

/* Copy XDR data into the buffer returned to caller */

		memcpy(buf1,&(buf[4]),size);
		
/* Returned data to caller */

		data_cmd[i].length = size;
		data_cmd[i].sequence = buf1;
		
/* Free the first buffer which is not used anymore (The freed of the second
   buffer is the caller responsability) */

		free(buf);
	}

/* Leave function */

	error->error_code = 0;
	error->dev_error = 0;
	return(0);
}

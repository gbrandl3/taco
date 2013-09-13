/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:	dc_sdev.c
 *
 * Description:
 * 
 * Author(s):	E. Taurel
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2008-10-23 05:26:24 $
 *
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include "dc.h"
#include "dcP.h"

#include <DevErrors.h>
#include <DevCmds.h>


#ifdef OSK
#	include <strings.h>
#else
#	include <stdlib.h>
#	include <string.h>
#endif

/* Some variables defined elsewhere */

extern DevDataListEntry DevData_List[];

/**@ingroup dcAPI
 * Store data belonging to super device into the data collector. 
 *
 * Super devices are non physical devices. The device name is used because of the 
 * analogy with real devices.
 *
 * command for one device.				
 *
 * @param data_cmd
 * @param num_cmd
 * @param error
 * 
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 * 			to by "error". 
 * @retval DS_OK otherwise.
 */ 
int dc_dataconvert(dc_datacmd *data_cmd,unsigned int num_cmd,dc_error *error)
{
	int 			i;
	XDR 			xdrs;
	int 			size,
				buf_size;
	char 			*buf,
				*buf1;
	DevLong			err;
	DevDataListEntry 	type_info;
	DevType 		type;

/* 
 * Try to verify the function parameters (non NULL pointer and two character in the device name) 
 */
	if (data_cmd == NULL || error == NULL)
	{
		error->error_code = DcErr_BadParameters;
		error->dev_error = 0;
		return(-1);
	}
	for (i = 0;i < (int)num_cmd;i++)
		if (data_cmd[i].argout == NULL)
		{
			error->error_code = DcErr_BadParameters;
			error->dev_error = 0;
			return(DS_NOTOK);
		}

/* 
 * A loop on every pseudo command 
 */
	for (i = 0;i < (int)num_cmd;i++)
	{
/* 
 * Compute argout data size 
 */
		type = data_cmd[i].argout_type;
		if (type < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (DevData_List[type].type != type)
			{
				error->error_code = DevErr_XDRLengthCalculationFailed;
				error->dev_error = i + 1;
				return(DS_NOTOK);
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
			return(DS_NOTOK);
		}

/* 
 * Allocate memory for buffers used for data in XDR format 
 */
		buf_size = size + SEC_MARGIN;
		if ((buf = (char *)malloc(buf_size)) == NULL)
		{
			error->error_code = DcErr_ClientMemoryAllocation;
			error->dev_error = i + 1;
			return(DS_NOTOK);
		}
		if ((buf1 = (char *)malloc(buf_size)) == NULL)
		{
			free(buf);
			error->error_code = DcErr_ClientMemoryAllocation;
			error->dev_error = i + 1;
			return(DS_NOTOK);
		}

/* 
 * Create a memory stream to convert argout type to XDR format 
 */
		xdrmem_create(&xdrs,(caddr_t)buf,buf_size,XDR_ENCODE);

/* 
 * Convert argout buffer to XDR type 
 * If the argout type is one of the general types, directly access the API
 * DevData_List array. If not, use the structure sent back by the previous
 * call to the xdr_get_type 
 */
		if (type < NUMBER_OF_GENERAL_XDR_TYPES)
		{
			if (!xdr_pointer(&xdrs,(char **)&data_cmd[i].argout, DevData_List[type].size, (xdrproc_t)DevData_List[type].xdr))
			{
		 		free(buf);
				free(buf1);
		 		error->error_code = DcErr_CantConvertDataToXDRFormat;
				error->dev_error = i + 1;
		 		return(DS_NOTOK);
			}
		}
		else
		{
			if (!xdr_pointer(&xdrs,(char **)&data_cmd[i].argout, type_info.size,(xdrproc_t)type_info.xdr)) 
			{
		 		free(buf);
				free(buf1);
		 		error->error_code = DcErr_CantConvertDataToXDRFormat;
				error->dev_error = i + 1;
		 		return(DS_NOTOK);
			}
		}

/* 
 * Copy XDR data into the buffer returned to caller 
 */
		memcpy(buf1,&(buf[4]),size);
		
/* 
 * Returned data to caller 
 */
		data_cmd[i].length = size;
		data_cmd[i].sequence = buf1;
		
/* 
 * Free the first buffer which is not used anymore (The freed of the second
 * buffer is the caller responsability) 
 */
		free(buf);
	}

/* 
 * Leave function 
 */
	error->error_code = 0;
	error->dev_error = 0;
	return(DS_OK);
}

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
 * File:	dchist_cli.c
 *
 * Description:
 *
 * Author(s):	E. Taurel
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.12 $
 *
 * Date:	$Date: 2008-04-06 09:07:15 $
 *
 ****************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <dc.h>
#include <dcP.h>

#include <private/ApiP.h>
#include <DevErrors.h>
// #include <DserverTeams.h>

#include <dc_xdr.h>

#ifdef _OSK
#	ifdef _UCC
#		include <string.h>
#		include <stdlib.h>
#	else
#		include <strings.h>
#	endif /* _UCC */
#	include <inet/socket.h>
#	include <inet/netdb.h>
#else /* _OSK */
#	include <stdlib.h>
#	include <string.h>
#	if HAVE_SYS_SOCKET_H 
#		include <sys/socket.h>
#	endif
#	if HAVE_NETINET_IN_H
#		include <netinet/in.h>
#	endif
#	if HAVE_NETDB_H
#		include <netdb.h>
#	endif
#	if HAVE_UNISTD_H
#		include <unistd.h>
#	endif  
#endif /* _OSK */

/* Some global variables defined elsewhere */

extern dc_nethost_info *dc_multi_nethost;

/**@ingroup dcAPI
 * In this function the caller retrieves the result history of one command for a single
 * device. The client sends to server a structure with 4 elements which are:
 *	- The device name
 *	- The command code from which the result history must be retrieved
 *	- The command result data type
 *	- The number of records to retrieve in the history.
 * The data trenasffered from the server to the client are organised in the same way that the 
 * devget function with the command time added in the xresh and xresh_clnt structures.
 * A general error code has been added to inform the client of a global error like server ... .
 *
 * @param dc_ptr 	Device handle					
 * @param cmd_code 	The command code				
 * @param argout_type 	Command result type			
 * @param nb_rec 	Number of record to be retrieved		
 * @param hist_buff 	Pointer to buffer where command result history must be stored	
 * @param error 	Pointer to error code				
 *
 * @return In case of trouble, the function returns DS_NOTOK. If there is a problem for
 *    only some command result in the history, the function returns the number of command 
 * which failed.Otherwise, the function returns DS_OK
 */
int dc_devget_history(datco *dc_ptr,long cmd_code,dc_hist *hist_buff,DevType argout_type,long nb_rec,DevLong *error)
{
	int 		i,
			l,
			ind,
			max;
	xdevgeth 	send;
	xres_hist_clnt 	*recev;
	xresh_clnt 	*tmp_ptr;
	int 		tmp_err;
	DevLong 	err,
			error1;
	int 		ret = 0;
	long 		i_net;
	dc_nethost_info *tmp_net;

/* 
 * Try to verify function parameters 
 */
	if (dc_ptr == NULL || error == NULL || nb_rec == 0 || nb_rec > HIST)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * Miscellaneous initialisation 
 */
	i_net = dc_ptr->net_ind;
	tmp_net = &dc_multi_nethost[i_net];
	ind = dc_ptr->indice;
	max = tmp_net->host_dc.length;

/* 
 * Is it necessary to reconnect to dc server? 
 */
	for (i = 0;i < max;i++)
	{
		if (tmp_net->dchost_arr[i].cantcont_error == MAXERR)
		{
			if (rpc_reconnect_rd(i,i_net,&error1))
			{
				*error = error1;
				return(DS_NOTOK);
			}
			tmp_net->dchost_arr[i].nb_call = 0;
		}
	}

/* 
 * Allocate memory for the device name and initialise the structure sent to server. 
 */

	l = strlen(dc_ptr->device_name);
	if ((send.xdev_name = (char *)malloc(l + 1)) == NULL)
	{
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(send.xdev_name,dc_ptr->device_name);
	send.xcmd = cmd_code;
	send.xargout_type = argout_type;
	send.xnb_rec = nb_rec;

/* 
 * Allocate memory for the array of "xresh_clnt" structure used by XDR routine to deserialise data 
 */
	if ((tmp_ptr = (xresh_clnt *)calloc(nb_rec,sizeof(xresh_clnt))) == NULL)
	{
		*error = DcErr_ClientMemoryAllocation;
		return(DS_OK);
	}
	for (i = 0;i < nb_rec;i++)
	{
		tmp_ptr[i].xerr = 0;
		tmp_ptr[i].xtime = 0;
		tmp_ptr[i].xargout_type = argout_type;
		tmp_ptr[i].xargout = (DevArgument)(hist_buff[i].argout);
	}

/* 
 * Call server 
 */
	recev = dc_devgeth_clnt_1(&send,tmp_net->dchost_arr[ind].rpc_handle,tmp_ptr,&err);
	tmp_net->dchost_arr[ind].nb_call++;

/* 
 * Return memory allocated to send data to server 
 */
	free(send.xdev_name);

/* 
 * Any problem with server ? 
 */
	if (recev == NULL)
	{
		free(tmp_ptr);
		if ((err == DcErr_CantContactServer) || (err == DcErr_RPCTimedOut))
			tmp_net->dchost_arr[ind].cantcont_error++;
		else
			tmp_net->dchost_arr[ind].cantcont_error = 0;
		*error = err;
		return(DS_NOTOK);
	}
	else
		tmp_net->dchost_arr[ind].cantcont_error = 0;

/* 
 * Big problem with server ? 
 */
	tmp_err = recev->xgen_err;
	if (tmp_err != 0)
	{
		free(tmp_ptr);
		*error = tmp_err;
		return(DS_NOTOK);
	}

/* 
 * Copy results into caller structures 
 */
	for (i = 0;i < nb_rec;i++)
	{
		tmp_err = recev->xresb_clnt.xresh_clnt_val[i].xerr;
		hist_buff[i].cmd_error = tmp_err;
		if (tmp_err != 0)
		{
			ret++;
			hist_buff[i].time = (recev->xresb_clnt.xresh_clnt_val[i].xtime / 10) + TIME_OFF;
			if (((tmp_err >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError)
				hist_buff[i].argout = (DevArgument)recev->xresb_clnt.xresh_clnt_val[i].xargout;
		}
		else
		{
			hist_buff[i].time = (recev->xresb_clnt.xresh_clnt_val[i].xtime / 10) + TIME_OFF;
			hist_buff[i].argout = (DevArgument)recev->xresb_clnt.xresh_clnt_val[i].xargout;
		}
	}

/* 
 * Free the memory allocated for the XDR routines used for deserialization 
 */
	free(tmp_ptr);

/* 
 * Leave function 
 */
	*error = 0;
	return(ret);
}

 /*

 Author:	E. Taurel
		$Author: jkrueger1 $

 Version:	$Revision: 1.2 $

 Date:		$Date: 2003-05-16 13:38:52 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                      Grenoble, France

 *  */

#include <API.h>
#include <dc.h>
#include <dcP.h>

#include <private/ApiP.h>
#include <DevErrors.h>
// #include <DserverTeams.h>

#include <dc_xdr.h>

#ifdef _OSK
#ifdef _UCC
#include <string.h>
#include <stdlib.h>
#else
#include <strings.h>
#endif /* _UCC */
#include <inet/socket.h>
#include <inet/netdb.h>
#else /* _OSK */
#include <stdlib.h>
#include <string.h>
#ifndef _NT
#include <sys/socket.h>
#ifndef __hp9000s300
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <unistd.h>
#endif  /* _NT */
#endif /* _OSK */

/* Some global variables defined elsewhere */

extern dc_nethost_info *dc_multi_nethost;




/**@ingroup dcAPI
 * To retrieve from the data collector the history of a command result for a device. 
 *
 * @param dc_ptr 	Device handle					
 * @param cmd_code 	The command code				
 * @param argout_type 	Command result type			
 * @param nb_rec 	Number of record to be retrieved		
 * @param hist_buff 	Pointer to buffer where command result history must be stored	
 * @param error 	Pointer to error code				
 *
 * @return In case of trouble, the function returns DS_NOTOK. If there is a problem for
 *    only some command result in the history, the function returns the
 *    number of command which failed.Otherwise, the function returns DS_OK
 */
int dc_devget_history(datco *dc_ptr,long cmd_code,dc_hist *hist_buff,DevType argout_type,long nb_rec,long *error)
{
	int 		i,
			l,
			ind,
			max;
	xdevgeth 	send;
	xres_hist_clnt 	*recev;
	xresh_clnt 	*tmp_ptr;
	int 		tmp_err;
	long 		err,
			error1;
//	int 		ret = 0;
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
		if ((tmp_net->dchost_arr[i].nb_call == tmp_net->max_call_rd) ||
		    (tmp_net->dchost_arr[i].cantcont_error == MAXERR))
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
 * Allocate memory for the array of "xresh_clnt" structure used by XDR
 * routine to deserialise data 
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
		if (err == DcErr_CantContactServer)
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
	return(DS_OK);
}

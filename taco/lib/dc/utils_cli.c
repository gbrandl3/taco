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
 * File:	utils_cli.c
 *
 * Description:
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jensmeyer $
 *
 * Original:    1993
 *
 * Version:     $Revision: 1.10 $
 *
 * Date:        $Date: 2005-09-28 12:44:17 $
 *
 *****************************************************************************/

#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>

#include <dc.h>
#include <dcP.h>
#include <dc_xdr.h>

#ifndef _OSK
#include <stdlib.h> 
#include <sys/types.h>
#ifndef WIN32
#include <sys/socket.h>
#ifndef __hp9000s300
#include <netinet/in.h>
#endif
#include <netdb.h>
#endif  /* WIN32 */
#else /* _OSK */
#ifdef _UCC
#include <string.h>
#include <stdlib.h>
#else
#include <strings.h>
#endif /* _UCC */
#include <inet/socket.h>
#include <inet/netdb.h>
#endif


/* Some extern variables (I know that this is against the GPS !!) */

extern configuration_flags config_flags;



/**@ingroup dcAPI
 * To ask to a data collector system general information
 * This function is mainly used by the dc_info command
 *
 * @param serv_name 	The dc host name
 * @param dc_inf 	Pointer to where function result will be stored
 * @param error 	Pointer for error code
 *
 * @return  In case of trouble, the function returns DS_NOTOK and set the error variable
 *    	pointed to by "error". Otherwise, the function returns 0 
 */
int dc_info(char *serv_name,servinf *dc_inf,long *error)
{
	long 		err;
	int 		send;
	dc_infox_back 	*recev;
	struct hostent 	*host;
	unsigned char	tmp = 0;
	char 		dev_name[DEV_NAME_LENGTH],
			*tmp_ptr;
	db_devinf_imp 	*serv_net;
	CLIENT 		*cl_info;
	int 		i;
	unsigned int 	diff;
	char 		*tmp1;

/* 
 * Try to verify function parameters 
 */
	if (serv_name == NULL || dc_inf == NULL || error == NULL)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * Miscellaneous initialisation 
 */
	for (i = 0;i < MAX_DOM;i++)
	{
		dc_inf->dom_array[i].dom_nb_dev = 0;
		dc_inf->dom_array[i].dom_name[0] = 0;
	}

/* 
 * If the RPC connection to static database server is not built, build one.
 * The "config_flags" variable is defined as global by the device server
 * API library. 
 */
	if ((config_flags.database_server != True) && db_import(&err))
	{
		*error = DcErr_CantBuildStaDbConnection;
		return(DS_NOTOK);
	}

/* 
 * Get data collector server host network parameters 
 */
	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*error = DcErr_CantGetDcHostInfo;
		return(DS_NOTOK);
	}
	tmp = (unsigned char)host->h_addr[3];

/* 
 * Build the device name associated with the FIRST write server on the specified host 
 */
	snprintf(dev_name, sizeof(dev_name), "sys/dc_wr_%u/1",tmp);

/* 
 * Ask the static database for this server network parameters 
 */
	tmp_ptr = dev_name;
	if (db_dev_import(&tmp_ptr,&serv_net,1,&err))
	{
		*error = DcErr_CantGetDcServerNetInfo;
		return(DS_NOTOK);
	}

/* 
 * Remove the .esr.fr at the end of host name (if any) 
 */
#ifdef OSK
        if ((tmp1 = index(serv_net[0].host_name,'.')) != NULL)
#else
        if ((tmp1 = strchr(serv_net[0].host_name,'.')) != NULL)
#endif /* OSK */
        {
                diff = (u_int)(tmp1 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }

/* 
 * Build the RPC connection to the dc server 
 */
	cl_info = clnt_create(serv_net[0].host_name,serv_net[0].pn,serv_net[0].vn,"tcp");
	if (cl_info == NULL)
	{
		free(serv_net);
		*error = DcErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	free(serv_net);

/* 
 * Call server 
 */
	recev = dc_info_1(&send,cl_info,&err);

/* 
 * Any problem with data transfer ? 
 */
	if (recev == NULL)
	{
		clnt_destroy(cl_info);
		*error = err;
		return(DS_NOTOK);
	}

/* 
 * Any problem with data collector access ? 
 */
	if (recev->err_code != 0)
	{
		clnt_destroy(cl_info);
		*error = recev->err_code;
		return(DS_NOTOK);
	}

/* 
 * Copy result to caller area 
 */
	dc_inf->free_mem = recev->back.free_mem;
	dc_inf->mem = recev->back.mem;
	dc_inf->nb_dev = recev->back.nb_dev;
	dc_inf->dom_nb = recev->back.dom_ax.dom_ax_len;
	for (i = 0;i < dc_inf->dom_nb;i++)
	{
		dc_inf->dom_array[i].dom_nb_dev = recev->back.dom_ax.dom_ax_val[i].dom_nb_dev;
		strncpy(dc_inf->dom_array[i].dom_name,recev->back.dom_ax.dom_ax_val[i].dom_name, sizeof(dc_inf->dom_array[i].dom_name));
	}

/* 
 * Free the memory allocated by XDR and destroy the RPC connection 
 */
	clnt_freeres(cl_info,(xdrproc_t)xdr_dc_infox_back,(char *)recev);
	clnt_destroy(cl_info);

/* 
 * No error 
 */
	*error = 0;
	return(DS_OK);
}



/**@ingroup dcAPI
 * To ask to a data collector system the name of all its registered devices
 *
 * @param serv_name 	The dc host name
 * @param devnametab 	Pointer where the device list will be stored
 * @param dev_n		Pointer where the number of devices in the list will be stored
 * @param error 	Pointer for error code
 *
 * @return    In case of trouble, the function returns DS_NOTOK and set the err variable
 *    pointed to by "perr". Otherwise, the function returns DS_OK
 */
int dc_devall(char *serv_name,char ***devnametab,int *dev_n,long *error)
{
	long 		err;
	int 		send,
			nb_dev;
	int 		i,
			j;
	dc_devallx_back *recev;
	struct hostent 	*host;
	unsigned char 	tmp = 0;
	char 		dev_name[DEV_NAME_LENGTH];
	char 		*tmp_ptr;
	db_devinf_imp 	*serv_net;
	CLIENT 		*cl_info;
	unsigned int 	diff;
	char 		*tmp1;

/* 
 * Try to verify function parameters 
 */
	if (serv_name == NULL || devnametab == NULL || error == NULL)
	{
		*dev_n = 0;
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * If the RPC connection to static database server is not built, build one.
 * The "config_flags" variable is defined as global by the device server
 * API library. 
 */
	if ((config_flags.database_server != True) && db_import(&err))
	{
		*dev_n = 0;
		*error = DcErr_CantBuildStaDbConnection;
		return(DS_NOTOK);
	}

/* 
 * Get data collector server host network parameters 
 */
	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*dev_n = 0;
		*error = DcErr_CantGetDcHostInfo;
		return(DS_NOTOK);
	}
	tmp = (unsigned char)host->h_addr[3];

/* 
 * Build the device name associated with the FIRST write server on the
 * specified host 
 */
	snprintf(dev_name, sizeof(dev_name), "sys/dc_wr_%u/1",tmp);

/* 
 * Ask the static database for this server network parameters 
 */
	tmp_ptr = dev_name;
	if (db_dev_import(&tmp_ptr,&serv_net,1,&err))
	{
		*dev_n = 0;
		*error = DcErr_CantGetDcServerNetInfo;
		return(DS_NOTOK);
	}

/* 
 * Remove the .esr.fr at the end of host name (if any) 
 */
#ifdef OSK
        if ((tmp1 = index(serv_net[0].host_name,'.')) != NULL)
#else
        if ((tmp1 = strchr(serv_net[0].host_name,'.')) != NULL)
#endif /* OSK */
        {
                diff = (u_int)(tmp1 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }

/* 
 * Build the RPC connection to the dc server 
 */
	cl_info = clnt_create(serv_net[0].host_name,serv_net[0].pn,serv_net[0].vn,"tcp");
	if (cl_info == NULL)
	{
		free(serv_net);
		*dev_n = 0;
		*error = DcErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	free(serv_net);

/* 
 * Call server 
 */
	recev = dc_devall_1(&send,cl_info,&err);

/* 
 * Any problem with data transfer ? 
 */
	if (recev == NULL)
	{
		clnt_destroy(cl_info);
		*dev_n = 0;
		*error = err;
		return(DS_NOTOK);
	}

/* 
 * Any problem with data collector access ? 
 */
	if (recev->err_code != 0)
	{
		clnt_destroy(cl_info);
		*dev_n = 0;
		*error = recev->err_code;
		return(DS_NOTOK);
	}

/* 
 * Allocate memory for the caller result 
 */
	nb_dev = recev->dev_name.name_arr_len;
	if ((*devnametab = (char **)calloc(nb_dev,sizeof(char *))) == NULL)
	{
		clnt_freeres(cl_info,(xdrproc_t)xdr_dc_infox_back,(char *)recev);
		clnt_destroy(cl_info);
		*dev_n = 0;
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (i = 0;i < nb_dev;i++)
	{
		tmp_ptr = recev->dev_name.name_arr_val[i];
		if (((*devnametab)[i] = (char *)malloc(strlen(tmp_ptr) + 1)) == NULL)
		{
			for (j = 0;j < i;j++)
				free((*devnametab)[j]);
			free(*devnametab);
			clnt_freeres(cl_info,(xdrproc_t)xdr_dc_infox_back,(char *)recev);
			clnt_destroy(cl_info);
			*dev_n = 0;
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
	}

/* 
 * Copy result to caller area 
 */
	for (i = 0;i < nb_dev;i++)
		strcpy((*devnametab)[i],recev->dev_name.name_arr_val[i]);
	*dev_n = nb_dev;

/* 
 * Free the memory allocated by XDR and destroy the RPC connection 
 */
	clnt_freeres(cl_info,(xdrproc_t)xdr_dc_devallx_back,(char *)recev);
	clnt_destroy(cl_info);

/* 
 * No error 
 */
	*error = 0;
	return(DS_OK);
}



/**@ingroup dcAPI
 * To ask to a data collector system information about a specific device. 
 * These information are :
 *  - The command used for polling and their argument types
 *  - The time needed to execute the command
 *  - The polling interval
 *  - Address of pointers in the shared memory segments
 *  - Time between the last five polling
 *
 * @param dev_name 	The device name
 * @param dc_dev_info 	Pointer to a structure where all the info will be stored	
 * @param error 	Pointer to error code (in case of)
 *
 * @return   In case of trouble, the function returns DS_NOTOK and set the err variable 
 *    pointed to by "perr". Otherwise, the function returns DS_OK
 */
int dc_dinfo(char *dev_name,dc_devinf *dc_dev_info,long *error)
{
	long 			err;
	int 			i,
				ret,
				l;
	DevVarStringArray 	host_dc;
	db_resource 		res_tab;
	long 			nethost_defined = False;
	char 			dv_name[DEV_NAME_LENGTH];
	char 			nethost[HOST_NAME_LENGTH];

/* 
 * Try to verify function parameters 
 */
	if (dev_name == NULL || dc_dev_info == NULL || error == NULL)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * Check device name (correct number of /) with or without nethost defined 
 */
	l = 0;
	if (dev_name[0] == '/')
	{
		NB_CHAR(l,dev_name,'/');
		if ((dev_name[1] != '/') || (l != 5)) 
		{
			*error = DcErr_BadParameters;
			return(DS_NOTOK);
		}
		else
		{
			l = strlen(dev_name);
			for (i = 2;i < l;i++)
			{
				if (dev_name[i] == '/')
					break;
				nethost[i - 2] = dev_name[i];
			}
			nethost[i - 2] = '\0';
			snprintf(dv_name, sizeof(dv_name), "//%s/class/dc/1",nethost);
		}
	}
	else
	{
		NB_CHAR(l,dev_name,'/');
		if (l != 2)
		{	
			*error = DcErr_BadParameters;
			return(DS_NOTOK);
		}
		nethost[0] ='\0';
		strncpy(dv_name,"class/dc/1", sizeof(dv_name));
	}

        host_dc.length = 0;
	host_dc.sequence = NULL;
        res_tab.resource_name = "host";
        res_tab.resource_type = D_VAR_STRINGARR;
        res_tab.resource_adr = &host_dc;

        if (db_getresource(dv_name,&res_tab,1,&err))
	{
		*error = DcErr_CantGetDcResources;
                return(DS_NOTOK);
	}


/* 
 * For each host where a single dc runs, ask if it knows the device 
 */
        for (i = 0;i < (int)host_dc.length;i++)
	{
                ret = dc_devinfo(host_dc.sequence[i],dev_name,dc_dev_info,&err);
                if (ret == DS_NOTOK)
		{
                        if (err != DcErr_DeviceNotDefined)
			{
				*error = err;
                                return(DS_NOTOK);
			}
		}
                if (ret == 0)
                        break;
	}

	if (i == (int)host_dc.length)
	{
		*error = DcErr_DeviceNotDefined;
                return(DS_NOTOK);
	}

/* 
 * Free memory allocated by db_getresource 
 */
	for (i = 0;i < (int)host_dc.length;i++)
		free(host_dc.sequence[i]);
	free(host_dc.sequence);
	
/* 
 * Leave function 
 */
	return(DS_OK);

}


/**@ingroup dcAPI
 * To ask to a single data collector system information
 * about a specific device. These information are :
 *
 * - The command used for polling and their argument types
 * - The time needed to execute the command
 * - The polling interval
 * - Address of pointers in the shared memory segments
 * - Time between the last five polling
 *
 * @param serv_name 	The dc host name
 * @param dev_name 	The device name
 * @param dc_dev_info 	Pointer to structure where command result will be stored
 * @param error 	Pointer for error code
 *
 * @return   In case of trouble, the function returns DS_NOTOK and set the err variable
 *    pointed to by "perr". Otherwise, the function returns DS_OK
 */
int dc_devinfo(char *serv_name,char *dev_name,dc_devinf *dc_dev_info,long *error)
{
	long 		err;
	int 		i,
			l;
	static char 	*send;
	dc_devinfx_back *recev;
	struct hostent 	*host;
	unsigned char 	tmp = 0;
	char 		dev_name1[DEV_NAME_LENGTH];
	char 		*tmp_ptr;
	db_devinf_imp 	*serv_net;
	CLIENT 		*cl_info;
	cmd_infox 	*tmp1;
	char 		*tmp_name;
	char 		nethost[HOST_NAME_LENGTH];
	unsigned int 	diff;
	char 		*tmp2;

/* 
 * Get data collector server host network parameters 
 */
	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*error = DcErr_CantGetDcHostInfo;
		return(DS_NOTOK);
	}
	tmp = (unsigned char)host->h_addr[3];

/* 
 * Extract nethost name if any 
 */
	if (dev_name[0] == '/')
	{
		l = strlen(dev_name);
		for (i = 2;i < l;i++)
		{
			if (dev_name[i] == '/')
				break;
			nethost[i - 2] = dev_name[i];
		}
		nethost[i - 2] = '\0';
		snprintf(dev_name1, sizeof(dev_name1), "//%s/sys/dc_wr_%u/1",nethost, tmp);
	}
	else
		snprintf(dev_name1, sizeof(dev_name1), "sys/dc_wr_%u/1", tmp);

/* 
 * Build the device name associated with the FIRST write server on the
 * specified host 
 */

/* 
 * Ask the static database for this server network parameters 
 */
	tmp_ptr = dev_name1;
	if (db_dev_import(&tmp_ptr,&serv_net,1,&err))
	{
		*error = DcErr_CantGetDcServerNetInfo;
		return(DS_NOTOK);
	}

/* 
 * Remove the .esr.fr at the end of host name (if any) 
 */

#ifdef OSK
        if ((tmp2 = index(serv_net[0].host_name,'.')) != NULL)
#else
        if ((tmp2 = strchr(serv_net[0].host_name,'.')) != NULL)
#endif /* OSK */
        {
                diff = (u_int)(tmp2 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }

/* 
 * Build the RPC connection to the dc server 
 */
	cl_info = clnt_create(serv_net[0].host_name,serv_net[0].pn,serv_net[0].vn,"tcp");
	if (cl_info == NULL)
	{
		free(serv_net);
		*error = DcErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	free(serv_net);

/* 
 * Extract real device name if nethost is specified 
 */
	if (dev_name[0] == '/')
	{
		tmp_name = dev_name + 2;
		for (i = 2;dev_name[i] != '/';i++)
			tmp_name++;
		tmp_name++;
	}
	else
		tmp_name = dev_name;

/* 
 * Device name in lower case letters 
 */
	l = strlen(tmp_name);
	if ((send = (char *)malloc(l + 1)) == NULL)
	{
		clnt_destroy(cl_info);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strncpy(send, tmp_name, l);
	for (i = 0;i < l;i++)
		send[i] = tolower(send[i]);

/* 
 * Call server 
 */
	recev = dc_devinfo_1(&send,cl_info,&err);

/* 
 * Free memory 
 */
	free(send);

/* 
 * Any problem with data transfer ? 
 */
	if (recev == NULL)
	{
		clnt_destroy(cl_info);
		dc_dev_info->devinf_nbcmd = 0;
		*error = err;
		return(DS_NOTOK);
	}

/* 
 * Any problem with data collector access ? 
 */
	if (recev->err_code != 0)
	{
		clnt_destroy(cl_info);
		dc_dev_info->devinf_nbcmd = 0;
		*error = recev->err_code;
		return(DS_NOTOK);
	}

/* 
 * Copy result to caller area 
 */
	for (i = 0;i < (int)recev->device.cmd_dev.cmd_dev_len;i++)
	{
		tmp1 = &(recev->device.cmd_dev.cmd_dev_val[i]);
		dc_dev_info->devcmd[i] = *(dc_devinf_cmd *)(tmp1);
	}
	dc_dev_info->devinf_nbcmd = recev->device.cmd_dev.cmd_dev_len;

	dc_dev_info->devinf_diff_time = recev->device.diff_time;
	dc_dev_info->devinf_poll = recev->device.pollx;
	dc_dev_info->devinf_ptr_off = recev->device.ptr_offset;
	dc_dev_info->devinf_data_off = recev->device.data_offset;
	dc_dev_info->devinf_data_base = recev->device.data_base;

	for (i = 0;i < 5;i++)
		dc_dev_info->devinf_delta[i] = recev->device.deltax[i];

/* 
 * Free the memory allocated by XDR and destroy the RPC connection 
 */
	clnt_freeres(cl_info,(xdrproc_t)xdr_dc_devinfx_back,(char *)recev);
	clnt_destroy(cl_info);

/* 
 * No error 
 */
	*error = 0;
	return(DS_OK);
}

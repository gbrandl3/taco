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
 * File       :   tools_cli.c
 * 
 * Project    :   Static database
 * 
 * Description:   Application Programmers Interface
 *                Interface to access static database for all its utilities
 *                calls
 * 
 * Author(s)  :   Emmanuel Taurel
 *                $Author: jkrueger1 $
 * 
 * Original   :   April 1997
 * 
 * Version    :   $Revision: 1.15 $
 * 
 * Date       :   $Date: 2013-05-17 08:55:47 $
 ********************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#define PORTMAP

#include <macros.h>
#include "db_setup.h"
#include "db_xdr.h"

#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>

#include "taco_utils.h"

#if defined(WIN32)
#	include <rpc.h>
#else
#	ifdef _OSK
#		include <inet/socket.h>
#		include <inet/netdb.h>
#		include <strings.h>
#	else
#		include <string.h>
#		if HAVE_SYS_SOCKET_H
#			include <sys/socket.h>
#		else
#			include <socket.h>
#		endif
#		if HAVE_NETDB_H
#			include <netdb.h>
#		else
#			include <hostLib.h>
#			include <taskLib.h>
#		endif
#		include <unistd.h>
#	endif /* _OSK */
#endif	/* WIN32 */

#ifndef OSK
#include <stdlib.h>
#endif

#include <math.h>

#ifdef ALONE
extern CLIENT 			*cl;
extern int 			first;
#endif /* ALONE */

static CLIENT 			*cl_tcp;
static int 			first_tcp = 0;
static struct sockaddr_in 	serv_adr;
#ifndef vxworks
static struct hostent 		*ht;
#else
static int 			host_addr;
#endif

/* Static and Global variables */

static struct timeval timeout_browse = {60, 0};

/*
 * global variable defined in gen_api.c which keeps track of
 * multiple nethosts
 */
extern nethost_info 		*multi_nethost;


/**@ingroup dbaseAPIdevice
 * This function returns to the caller a structure with many device information.
 * These information are
 * @li the name of the server in charge of the device
 * @li the host where it is running
 * @li the device server program number
 * @li the device class,
 * @li ...
 *									
 * @param dev_name	Device name
 * @param p_info 	Pointer for device information			
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *		to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_deviceinfo(const char *dev_name,db_devinfo_call *p_info, DevLong *p_error)
{
	db_devinfo_svc 	*recev;
	int 		i,k;
	char 		*name_sent,
			*devname,
			*ptr;
	char 		*nethost;
	DevLong 	error;
	long 		exit_code = DS_OK;
        long		i_nethost;
	struct timeval 	old_tout;
	CLIENT 		*local_cl;
#ifndef _OSK
	struct timeval 	tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if ((p_info == NULL) || (dev_name == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
			
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/*
 * find out which nethost has been requested for this device
 */
	if ((i_nethost = get_i_nethost_by_device_name(dev_name,p_error)) < 0)
	{
/* 
 * The nethost is not imported, extract nethost name and import it 
 */
		nethost = extract_nethost(dev_name, p_error);
		if (*p_error != DS_OK)
			return DS_NOTOK;
/* 
 * The specified nethost is not in the list of imorted nethosts, therefore 
 * call setup_config_multi() to add it 
 */
		if (setup_config_multi(nethost, p_error) != DS_OK)
		{
			return(DS_NOTOK);
		}
/* 
 * Find where the nethost is in the multi-nethost array 
 */
		i_nethost = get_i_nethost_by_name(nethost, p_error);
		if (i_nethost < 0)
		{
			return DS_NOTOK;
		}
	}
/* 
 * If the RPC connection to the database server is not built, build one.
 * The "config_flags" variable is defined as global by the device server
 * API library. If the db_import failed, clear the configuration flag
 * in order to recall the manager for database server RPC parameter at the
 * next db_import (for reconnection) 
 */
	if ((multi_nethost[i_nethost].config_flags.database_server != True)
			&& db_import_multi(multi_nethost[i_nethost].nethost,&error))
	{
		multi_nethost[i_nethost].config_flags.configuration = False;
		*p_error = DbErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	local_cl = multi_nethost[i_nethost].db_info->clnt;
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call 
 */
   	clnt_control(local_cl,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
	devname = extract_device_name(dev_name,p_error);

#endif /* ALONE */
/*
 * Initialize data sent to server
 */
	k = strlen(devname);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	ptr = strcpy_tolower(name_sent,devname);
/*
 * if the device name contains the hostname strip the host name ???
 */
	if (strncmp(name_sent, "//", 2) == 0)
		ptr = strchr(name_sent + 2, '/') + 1;
/*
 * Call server 
 */
	recev = db_deviceinfo_1(&ptr,local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&name_sent,(void **)&recev,&local_cl,
					(int)DB_DEVINFO,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call 
 */
	if (recev->db_err == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{
#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_deviceinfo_1(&name_sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((recev->db_err == DS_OK) || 
			    (recev->db_err != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if(recev->db_err != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_devinfo_svc,(char *)recev);
		free(name_sent);
		return(DS_NOTOK);
	}
/*	
 * Free memory used to send data to server
 */
	free(name_sent);
/*	
 * Copy data sent back to client into caller structure
 */
	p_info->device_type = recev->device_type;
	p_info->device_exported = recev->device_exported;
	strcpy(p_info->device_class,recev->device_class);
	strcpy(p_info->server_name,recev->server_name);
	strcpy(p_info->personal_name,recev->personal_name);
	strcpy(p_info->process_name,recev->process_name);
	p_info->server_version = recev->server_version;
	strcpy(p_info->host_name,recev->host_name);
	p_info->pid = recev->pid;
	p_info->program_num = recev->program_num;
/*	
 * Free memory allocated by XDR stuff
 */
	clnt_freeres(local_cl,(xdrproc_t)xdr_db_devinfo_svc,(char *)recev);
/*
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}


/**@ingroup dbaseAPIdevice
 * This function returns to the caller the list of all resources for a list of devices. The resources are
 * returned as string(s) with the following syntax: "device name/resource name : resource value".
 *
 * @param dev_nb 	The number of device name(s)				
 * @param dev_name_list The device name list			
 * @param p_res_nb	The number of resource(s) 
 * @param ppp_list	Resource(s) list (allocated by this function)
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_deviceres(long dev_nb, char **dev_name_list, long *p_res_nb, char ***ppp_list, DevLong *p_error)
{
	db_res *recev;
	int i,j,k;
	DevLong error;
	long exit_code = DS_OK;
	long nb_res;
	db_res sent;
	struct timeval old_tout;
	CLIENT *local_cl;
	int tcp_so;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if ((ppp_list == NULL) || (p_res_nb == NULL) ||
	    (dev_name_list == NULL) || (dev_nb == 0))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_res_nb = 0;
	*p_error = DS_OK;
		
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first_tcp)
	{
		cl_tcp = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"tcp");
		if (cl_tcp == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first_tcp++;
	}
	old_tout = timeout_browse;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
	if (!first_tcp)
	{
#ifndef vxworks
		ht = gethostbyname(db_info.conf->server_host);
		if (ht == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}		
#else /* !vxworks */
		host_addr = hostGetByName(db_info.conf->server_host);
		if (host_addr == 0)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
#endif /* !vxworks */


		serv_adr.sin_family = AF_INET;
#ifndef vxworks
		memcpy((void *)(&serv_adr.sin_addr),ht->h_addr,(size_t)ht->h_length);
#else  /* !vxworks */
		memcpy((void *)(&serv_adr.sin_addr),(char*)&host_addr, 4);
#endif /* !vxworks */


		serv_adr.sin_port = 0;
		tcp_so = RPC_ANYSOCK;

		cl_tcp = clnttcp_create(&serv_adr,db_info.conf->prog_number,
					DB_VERS_3,&tcp_so,0,0);

		if (cl_tcp == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		
		clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);

		first_tcp = 1;
	}			
#endif /* ALONE */

	local_cl = cl_tcp;
/*
 * Initialize data sent to server 
 */
	sent.db_err = DS_OK;
	sent.res_val.arr1_len = dev_nb;
	
	if ((sent.res_val.arr1_val = (char **)calloc(dev_nb,sizeof(char *))) == NULL)
	{
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (i = 0;i < dev_nb;i++)
	{	
		k = strlen(dev_name_list[i]);
		if ((sent.res_val.arr1_val[i] = (char *)malloc(k + 1)) == NULL)
		{
			for (j = 0;j < i;j++)
				free(sent.res_val.arr1_val[j]);
			free(sent.res_val.arr1_val);	
			*p_error = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		strcpy_tolower(sent.res_val.arr1_val[i],dev_name_list[i]);
	}

/* 
 * Call server 
 */
	recev = db_deviceres_1(&sent,local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_DEVRES,(long)0,DB_TCP,&error);
			cl_tcp = local_cl;
		}
		if (error != DS_OK)
		{
			for (i = 0;i < dev_nb;i++)
				free(sent.res_val.arr1_val[i]);
			free(sent.res_val.arr1_val);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call 
 */
	if (recev->db_err == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_deviceres_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				for (i = 0;i < dev_nb;i++)
					free(sent.res_val.arr1_val[i]);
				free(sent.res_val.arr1_val);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((recev->db_err == DS_OK) || 
			    (recev->db_err != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ? 
 */
	if(recev->db_err != DS_OK)
	{
		for (i = 0;i < dev_nb;i++)
			free(sent.res_val.arr1_val[i]);
		free(sent.res_val.arr1_val);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);
		return(DS_NOTOK);
	}
/*
 * Free memory used to send data to server 
 */
	for (i = 0;i < dev_nb;i++)
		free(sent.res_val.arr1_val[i]);
	free(sent.res_val.arr1_val);
/*		
 * Initialize resource number
 */
	nb_res = recev->res_val.arr1_len;
	*p_res_nb = nb_res;
/*
 * Allocate memory for resource names array and copy them
 */
	if (nb_res != 0)
	{
		if ((*ppp_list = (char **)calloc(nb_res,sizeof(char *))) == NULL)
		{
			*p_error = DbErr_ClientMemoryAllocation;
			exit_code = DS_NOTOK;
		}
		else 
		{	
			for (i = 0;i < nb_res;i++)
			{
				if (((*ppp_list)[i] = (char *)malloc(strlen(recev->res_val.arr1_val[i]) + 1)) == NULL)
				{
					for (j = 0;j < i;j++)
						free((*ppp_list)[j]);
					free(*ppp_list);
					*p_error = DbErr_ClientMemoryAllocation;
					exit_code = DS_NOTOK;
					break;
				}
				else
					strcpy((*ppp_list)[i],recev->res_val.arr1_val[i]);
			}
		}
	}
/*	
 * Free memory allocated by XDR stuff 
 */
	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);
/*
 * Leave function
 */
	return(exit_code);
}




/**@ingroup dbaseAPIdevice
 * This function deletes a (pseudo) device from the list of devices registered in the database.
 *									
 * @param dev_name 	Device name
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_devicedelete(const char *dev_name,DevLong *p_error)
{
	long *recev;
	int i,k;
	char *name_sent;
	DevLong error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if (dev_name == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
			
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Initialize data sent to server
 */
	k = strlen(dev_name);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(name_sent,dev_name);
/*
 * Call server 
 */
	recev = db_devicedelete_1(&name_sent,local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server. 
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&name_sent,(void **)&recev,&local_cl,
					(int)DB_DEVDEL,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (*recev == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_devicedelete_1(&name_sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((*recev == DS_OK) || 
			    (*recev != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if (*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(name_sent);
		*p_error = *recev;
		return(DS_NOTOK);
	}
/*
 * Free memory used to send data to server
 */
	free(name_sent);
/*	
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}



/**@ingroup dbaseAPIdevice
 * This function deletes all the resources belonging to a list of devices from the database.	
 *									
 * @param dev_nb  	The device name number				
 * @param dev_name_list The device name list
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_devicedeleteres(long dev_nb,char **dev_name_list,db_error *p_error)
{
	db_psdev_error *recev;
	int i,j,k;
	db_res sent;
	DevLong error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if ((dev_name_list == NULL) || (dev_nb == 0))
	{
		p_error->error_code = DbErr_BadParameters;
		p_error->psdev_err = DS_OK;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	p_error->error_code = DS_OK;
	p_error->psdev_err = DS_OK;
			
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			p_error->error_code = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/* 
 * Initialize data sent to server
 */
	sent.db_err = DS_OK;
	sent.res_val.arr1_len = dev_nb;
	
	if ((sent.res_val.arr1_val = (char **)calloc(dev_nb,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		p_error->error_code = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	for (i = 0;i < dev_nb;i++)
	{	
		k = strlen(dev_name_list[i]);
		if ((sent.res_val.arr1_val[i] = (char *)malloc(k + 1)) == NULL)
		{
			for (j = 0;j < i;j++)
				free(sent.res_val.arr1_val[j]);
			free(sent.res_val.arr1_val);	
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			p_error->error_code = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		strcpy_tolower(sent.res_val.arr1_val[i],dev_name_list[i]);
	}
/*
 * Sort this device name list
 */
	kern_sort(sent.res_val.arr1_val,dev_nb);
/*	
 * Call server
 */
	recev = db_devicedeleteres_1(&sent,local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_DEVDELALLRES,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			for (i = 0;i < dev_nb;i++)
				free(sent.res_val.arr1_val[i]);
			free(sent.res_val.arr1_val);
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			p_error->error_code = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (recev->error_code == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_devicedeleteres_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				for (i = 0;i < dev_nb;i++)
					free(sent.res_val.arr1_val[i]);
				free(sent.res_val.arr1_val);
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				p_error->error_code = error;
				return(DS_NOTOK);
			}
			if ((recev->error_code == DS_OK) || 
			    (recev->error_code != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if (recev->error_code != DS_OK)
	{
		for (i = 0;i < dev_nb;i++)
			free(sent.res_val.arr1_val[i]);
		free(sent.res_val.arr1_val);
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		p_error->error_code = recev->error_code;
		p_error->psdev_err = recev->psdev_err;
		return(DS_NOTOK);
	}
/*
 * Free memory used to send data to server
 */
	for (i = 0;i < dev_nb;i++)
		free(sent.res_val.arr1_val[i]);
	free(sent.res_val.arr1_val);
/*	
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}


/**@ingroup dbaseAPImisc
 * This function returns the database global information as
 * @li the number of exported devices defined in the database
 * @li the number of resources defined for each device domain
 * @li ...
 *
 * @param p_info 	Pointer to the structure where all the info will be
 *		        stored. These information are the number of devices defined in the 
 *			database, the number of resources defined in the database ...
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_stat(db_stat_call *p_info,DevLong *p_error)
{
	db_info_svc *recev;
	int i;
	DevLong error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if (p_info == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
			
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Call server
 */
	recev = db_stat_1(local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_INFO,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (recev->db_err == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_stat_1(local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((recev->db_err == DS_OK) || 
			    (recev->db_err != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if(recev->db_err != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_info_svc,(char *)recev);
		return(DS_NOTOK);
	}
/*
 * Allocate memory  to store domain name and element number
 */
	if ((p_info->dev_domain = (db_info_dom *)calloc(recev->dev.dom_len,sizeof(db_info_dom))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_info_svc,(char *)recev);
		return(DS_NOTOK);
	}
	  if ((p_info->res_domain = (db_info_dom *)calloc(recev->res.dom_len,sizeof(db_info_dom))) == NULL)
	{
		free(p_info->dev_domain);
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_info_svc,(char *)recev);
		return(DS_NOTOK);
	}
/*		
 * Copy result to client structure
 */
	p_info->dev_defined = recev->dev_defined;
	p_info->dev_exported = recev->dev_exported;
	p_info->psdev_defined = recev->psdev_defined;
	p_info->res_number = recev->res_number;
	p_info->dev_domain_nb = recev->dev.dom_len;
	for (i = 0;i < recev->dev.dom_len;i++)
	{
		strcpy(p_info->dev_domain[i].dom_name,recev->dev.dom_val[i].dom_name);
		p_info->dev_domain[i].dom_elt = recev->dev.dom_val[i].dom_elt;
	}
	p_info->res_domain_nb = recev->res.dom_len;
	for (i = 0;i < recev->res.dom_len;i++)
	{
		strcpy(p_info->res_domain[i].dom_name,recev->res.dom_val[i].dom_name);
		p_info->res_domain[i].dom_elt = recev->res.dom_val[i].dom_elt;
	}
/*
 * Free memory allocated by XDR stuff
 */
	clnt_freeres(local_cl,(xdrproc_t)xdr_db_info_svc,(char *)recev);
/*
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}



/**@ingroup dbaseAPIserver
 * This function marks all devices driven by the device server with a full name ds_full_name
 * as not exported devices.
 *
 * @param ds_name  	The device server name			
 * @param pers_name 	The device server personal name		
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_servunreg(const char *ds_name,const char *pers_name,DevLong *p_error)
{
	long *recev;
	int i,j,k;
	DevLong error;
	long exit_code = DS_OK;
	db_res sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters 
 */
	if ((ds_name == NULL) || (pers_name == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
		
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Initialize data sent to server
 */
	sent.db_err = DS_OK;
	sent.res_val.arr1_len = 2;
	
	if ((sent.res_val.arr1_val = (char **)calloc(sizeof(char *),2)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(ds_name);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(sent.res_val.arr1_val[0],ds_name);

	k = strlen(pers_name);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(sent.res_val.arr1_val[1],pers_name);
/*
 * Call server
 */
	recev = db_servunreg_1(&sent,local_cl,&error);	
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_SVCUNREG,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			free(sent.res_val.arr1_val[0]);
			free(sent.res_val.arr1_val[1]);
			free(sent.res_val.arr1_val);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (*recev == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_servunreg_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				free(sent.res_val.arr1_val[0]);
				free(sent.res_val.arr1_val[1]);
				free(sent.res_val.arr1_val);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((*recev == 0) || 
			    (*recev != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if(*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(sent.res_val.arr1_val[0]);
		free(sent.res_val.arr1_val[1]);
		free(sent.res_val.arr1_val);
		*p_error = *recev;
		return(DS_NOTOK);
	}
/*
 * Free memory used to send data to server
 */
	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);	
/*
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIserver
 * This function returns miscellaneous information for a device server started with
 * a personal name. These information are
 * @li the number and name of device served by the server
 * @li the device server process name
 * @li the process id (PID) of the server process
 * @li the host name where the server process is running
 *
 * @param ds_name 	The device server name			
 * @param pers_name 	The device server personal name		
 * @param p_inf 	Pointer for the structure with DS info		
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 * 			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_servinfo(const char *ds_name,const char *pers_name, db_svcinfo_call *p_inf, DevLong *p_error)
{
	svcinfo_svc *recev;
	int i,j,k;
	DevLong error;
	long tmp,tmp_dev;
	long exit_code = DS_OK;
	db_res sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if ((ds_name == NULL) || (pers_name == NULL) || (p_inf == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
	
	p_inf->embedded_server_nb = 0;
	p_inf->server = NULL;
	p_inf->process_name[0] = '\0';
	p_inf->pid  = 0;
	p_inf->host_name[0] = '\0';
	p_inf->program_num = 0;
		
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Initialize data sent to server
 */
	sent.db_err = DS_OK;
	sent.res_val.arr1_len = 2;
	
	if ((sent.res_val.arr1_val = (char **)calloc(sizeof(char *),2)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(ds_name);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(sent.res_val.arr1_val[0],ds_name);

	k = strlen(pers_name);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(sent.res_val.arr1_val[1],pers_name);
/*
 * Call server
 */
	recev = db_servinfo_1(&sent,local_cl,&error);	
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_SVCINFO,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			free(sent.res_val.arr1_val[0]);
			free(sent.res_val.arr1_val[1]);
			free(sent.res_val.arr1_val);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (recev->db_err == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_servinfo_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				free(sent.res_val.arr1_val[0]);
				free(sent.res_val.arr1_val[1]);
				free(sent.res_val.arr1_val);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((recev->db_err == DS_OK) || 
			    (recev->db_err != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if(recev->db_err != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(sent.res_val.arr1_val[0]);
		free(sent.res_val.arr1_val[1]);
		free(sent.res_val.arr1_val);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_svcinfo_svc,(char *)recev);
		return(DS_NOTOK);
	}
/*
 * Copy received data into caller structure
 */
	strcpy(p_inf->process_name,recev->process_name);
	p_inf->pid = recev->pid;
	strcpy(p_inf->host_name,recev->host_name);
	p_inf->program_num = recev->program_num;
	
	tmp = recev->embedded_len;
	p_inf->embedded_server_nb = tmp;
	
	if ((p_inf->server = (db_svcinfo_server *)calloc(tmp,sizeof(db_svcinfo_server))) == NULL)
	{
		clnt_freeres(local_cl,(xdrproc_t)xdr_svcinfo_svc,(char *)recev);
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(sent.res_val.arr1_val[0]);
		free(sent.res_val.arr1_val[1]);
		free(sent.res_val.arr1_val);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (i = 0;i < tmp;i++)
	{
		strcpy(p_inf->server[i].server_name,recev->embedded_val[i].server_name);
		tmp_dev = recev->embedded_val[i].dev_len;
		p_inf->server[i].device_nb = tmp_dev;
		if ((p_inf->server[i].device = (db_svcinfo_dev *)calloc(tmp_dev,sizeof(db_svcinfo_dev))) == NULL)
		{
			for (j = 0;j < i;j++)
				free(p_inf->server[j].device);
			free(p_inf->server);
			free(sent.res_val.arr1_val[0]);
			free(sent.res_val.arr1_val[1]);
			free(sent.res_val.arr1_val);
			clnt_freeres(local_cl,(xdrproc_t)xdr_svcinfo_svc,(char *)recev);
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		for (j = 0;j < tmp_dev;j++)
		{
			p_inf->server[i].device[j].exported_flag = recev->embedded_val[i].dev_val[j].exported_flag;
			strcpy(p_inf->server[i].device[j].dev_name,recev->embedded_val[i].dev_val[j].name);
		}
	}
/*
 * Free memory used to send data to server
 */
	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);
/*		
 * Free memory allocated by XDR stuff
 */
	clnt_freeres(local_cl,(xdrproc_t)xdr_svcinfo_svc,(char *)recev);
/*
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}


/**@ingroup dbaseAPIserver
 * This function deletes a device server from the database and if needed, all the server
 * device resources.
 *
 * @param ds_name  	The device server name			
 * @param pers_name 	The device server personal name		
 * @param delres_flag 	A flag set to True if device resources should also be deleted				
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_servdelete(const char *ds_name,const char *pers_name, long delres_flag, DevLong *p_error)
{
	long *recev;
	int i,j,k;
	DevLong error;
	long exit_code = DS_OK;
	db_res sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if ((ds_name == NULL) || (pers_name == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
	if ((delres_flag != True) && (delres_flag != False))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
		
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Initialize data sent to server. The db_err long is used to transfer the
 * delete resource flag over the network !!
 */
	sent.db_err = delres_flag;
	sent.res_val.arr1_len = 2;
	
	if ((sent.res_val.arr1_val = (char **)calloc(sizeof(char *),2)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(ds_name);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(sent.res_val.arr1_val[0],ds_name);

	k = strlen(pers_name);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(sent.res_val.arr1_val[1],pers_name);
/*		
 * Call server
 */
	recev = db_servdelete_1(&sent,local_cl,&error);	
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_SVCDELETE,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (*recev == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_servdelete_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((*recev == DS_OK) || 
			    (*recev != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if(*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = *recev;
		return(DS_NOTOK);
	}
/*
 * Free memory used to send data to server
 */
	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);	

/* 
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}



/**@ingroup dbaseAPIdevice
 * This function returns to the caller information about the device poller in charge
 * of a device. A poller is a process in charge of "polling" the device in order to
 * store device command result into the TACO data collector. The poller information are
 * @li the poller name
 * @li the host where it is running
 * @li ...
 *									
 * @param dev_name  	Device name
 * @param poll 		Pointer to the structure where poller info will be
 *		       	stored. these info are : ds_name, ds pers. name,
 *		       	host name, process name and PID			
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_getpoller(const char *dev_name,db_poller *poll,DevLong *p_error)
{
	db_poller_svc *recev;
	int i,k;
	char *name_sent;
	DevLong error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters 
 */
	if ((dev_name == NULL) || (poll == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
			
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call 
 */
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Initialize data sent to server
 */
	k = strlen(dev_name);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(name_sent,dev_name);
/*
 * Call server
 */
	recev = db_getpoll_1(&name_sent,local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&name_sent,(void **)&recev,&local_cl,
					(int)DB_GETPOLLER,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			free(name_sent);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (recev->db_err == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_getpoll_1(&name_sent,local_cl,&error);
			if(recev == NULL)
			{
				free(name_sent);
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((recev->db_err == DS_OK) || 
			    (recev->db_err != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*	
 * Any problems during database access ?
 */
	if(recev->db_err != DS_OK)
	{
		free(name_sent);
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_poller_svc,(char *)recev);
		return(DS_NOTOK);
	}
/*
 * Init caller structure
 */
	strcpy(poll->server_name,recev->server_name);
	strcpy(poll->personal_name,recev->personal_name);
	strcpy(poll->host_name,recev->host_name);
	strcpy(poll->process_name,recev->process_name);
	poll->pid = recev->pid;
/*
 * Free memory used to send data to server
 */
	free(name_sent);
/*	
 * Free memory allocated by XDR stuff
 */
	clnt_freeres(local_cl,(xdrproc_t)xdr_db_poller_svc,(char *)recev);
/*
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}


/**@ingroup dbaseAPImisc
 * This function initialises a resource cache for the specified domain.
 *									
 * @param domain 	Domain name
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "p_error". 
 * @retval DS_OK otherwise.
 */
long db_initcache(const char *domain, DevLong *p_error)
{
	long *recev;
	int i,k;
	char *name_sent;
	DevLong error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
/*
 * Try to verify function parameters
 */
	if (domain == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}
/*
 * Miscellaneous init.
 */
	*p_error = DS_OK;
			
#ifdef ALONE
/* 
 * Create RPC connection if it's the first call
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*p_error = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_browse);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
		first++;
	}
	old_tout = timeout_browse;
	local_cl = cl;
#else
/* 
 * Get client time-out value and change it to a larger value more suitable
 * for browsing call
 */  
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/*
 * Initialize data sent to server
 */
	k = strlen(domain);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(name_sent,domain);
/*
 * Call server
 */
	recev = db_initcache_1(&name_sent,local_cl,&error);
/*
 * Any problem with server ? If yes and if it is a time-out, try to reconnect
 * to a new database server.
 */
	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&name_sent,(void **)&recev,&local_cl,
					(int)DB_INITCACHE,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = error;
			return(DS_NOTOK);
		}
	}
/*
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call
 */
	if (*recev == DbErr_DatabaseNotConnected)
	{
		for (i = 0;i < RETRY;i++)
		{

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
			recev = db_initcache_1(&name_sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*p_error = error;
				return(DS_NOTOK);
			}
			if ((*recev == DS_OK) || 
			    (*recev != DbErr_DatabaseNotConnected))
				break;
		}
	}
/*
 * Any problems during database access ?
 */
	if (*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(name_sent);
		*p_error = *recev;
		return(DS_NOTOK);
	}
/*
 * Free memory used to send data to server
 */
	free(name_sent);
/*
 * Leave function
 */
	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);
}

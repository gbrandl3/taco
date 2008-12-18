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
 * File       : setacc_cli.c
 *
 * Project    : Static database
 *
 * Description: Application Programmers Interface
 *              Interface to access static database
 *
 * Author(s)  : Emmanuel Taurel
 *		$Author: jkrueger1 $
 *
 * Original   : January 1991
 *
 * Version    :	$Revision: 1.24 $
 *
 * Date       :	$Date: 2008-12-18 07:59:44 $
 * 
 *-*******************************************************************/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif /* WIN32 */
#define PORTMAP

#include "macros.h"
#include "db_setup.h"
#include "db_xdr.h"
#include "API.h"
#include "private/ApiP.h"
#include "DevErrors.h"

#include "taco_utils.h"

#if defined(WIN32)
#	include <rpc.h>
#	if 0
#		include <nm_rpc.h>
#	endif
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

#ifdef __STDCPP__
#include <vector>
#include <string>
#endif

#ifdef ALONE
CLIENT *cl;
int first = 0;
#endif /* ALONE */

#define SIZE_A	20
#define SIZE_B	40
#define SIZE_C	80

int test_star PT_((const char *filter));

/* Static and Global variables */

int func_ptr = 0;
devexp_res *tab_clstu;
static struct timeval timeout_resource={15,0};

static int first_devexp = 0;
static int first_tcp_call = 0;
static struct sockaddr_in serv_adr;
#ifndef vxworks
static struct hostent *ht;
#else
static int host_addr;
#endif


/*
 * global variable defined in gen_api.c which keeps track of
 * multiple nethosts
 */

extern nethost_info *multi_nethost;

/**@ingroup dbaseAPIresource
 * Retrieves resource from the database, converts them to the desired type and 
 * stores them at the right place. A resource value is stored as a atring in a database on a 
 * remote computer.
 *
 * @param devname	A pointer to a string which defines the device name
 * @param res		A pointer to an array of db_resource structure defining the
 *               	resource to be retrieved
 * @param res_num	The number of resource to be retrieved
 * @param perr 		The error code if any
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_getresource(const char *devname, Db_resource res, u_int res_num, DevLong *perr)
{
	db_res		*recev;
	arr1 		send;
	int 		i,
			j,
			k,
			l,
			ctr,
			tcp_used = 0;
	short 		tmp_short,
			*short_ptr;
	char 		numb[SIZE_A],
			*char_ptr;
	DevLong 	*long_ptr;
	float 		*float_ptr;
	double 		*double_ptr;
	char 		**str_ptr,
			*ptrc,
			*ptra;
	u_int 		diff;
	register char 	*temp,
			*tmp;
	CLIENT 		*tcp_cl;
	CLIENT 		*local_cl;
	int 		tcp_so;
	DevLong		error;
	long 		i_nethost;
	char 		*nethost = NULL;
	long 		try_reconnect = False;
#ifndef _OSK
	struct timeval 	tout;
#endif
#ifdef ALONE
	char 		*serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */
	char		*dev_name = (char *)devname;

/* 
 * Try to verify the function parameters (non NULL pointer and two 
 *  characters in device name) 
 */
	if (config_flags && config_flags->no_database)
	{
		*perr = DbErr_NoDatabase;
		return(DS_NOTOK);
	}

	if (dev_name == NULL || res == NULL || res_num == 0)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	for (i=0;i<(int)res_num;i++)
	{
		if (res[i].resource_name == NULL || res[i].resource_adr == NULL)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}

/* 
 * Check if device name follows naming conventions. If the nethost is specified,
 * checks syntax. If it is not specified, only count the / number 
 */
	l = 0;
	NB_CHAR(l, dev_name, '/');
	if (dev_name[0] == '/')
	{
		if ((l != 5) || (dev_name[1] != '/'))
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}
	else if (l != 2)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

#ifdef ALONE
/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout_resource);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&timeout_resource);
		first++;
	}
	local_cl = cl;
#else
/*
 * find out which nethost has been requested for this device
 */
	if ((i_nethost = get_i_nethost_by_device_name(dev_name,perr)) < 0)
	{
/* 
 * The nethost is not imported, extract nethost name and import it 
 */
		nethost = extract_nethost(dev_name, perr);
		if (*perr != DS_OK)
			return DS_NOTOK;
/* 
 * The specified nethost is not in the list of imported nethosts, therefore 
 * call setup_config_multi() to add it 
 */
		if (setup_config_multi(nethost,perr) != DS_OK)
			return(DS_NOTOK);
/* 
 * Find where the nethost is in the multi-nethost array 
 */
		i_nethost = get_i_nethost_by_name(nethost,perr);
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
		*perr = DbErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	local_cl = multi_nethost[i_nethost].db_info->clnt;
	dev_name = extract_device_name(dev_name, perr);
#endif /* ALONE */
/* 
 * Allocate memory for the array of pointeur to char 
 */
	if((send.arr1_val = (DevString *)calloc(res_num,sizeof(DevString))) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
/* 
 * Build the full resource name (in lowercase letters) and initialize the array
 * of pointer to resource name 
 */
	k = strlen(dev_name);
	for ( i = 0; i < (int)res_num; i++)
	{
		l = strlen(res[i].resource_name);
		if ((send.arr1_val[i] = (DevString)malloc(k + l + 2)) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			for (j=0;j<i;j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			return(DS_NOTOK);
		}

		strcpy_tolower(send.arr1_val[i],dev_name);
		strcat(send.arr1_val[i], "/");
		strcat_tolower(send.arr1_val[i],res[i].resource_name);
	}

/* 
 * Initialize the structure sended to server 
 */
	send.arr1_len = res_num;

/* 
 * Call server 
 */
	recev = db_getres_1(&send,local_cl,&error);
/* 
 * Any problem with server ? 
 */
	if(recev == NULL)
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
			to_reconnection((void *)&send,(void **)&recev,&local_cl,(int)DB_GETRES,0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			for (j = 0; j < (int)res_num; j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			*perr = error;
			return(DS_NOTOK);
		}
	}
/* 
 * Any problems during database access ? 
 */
	if(recev->db_err != DS_OK)
	{
		if (recev->db_err == DbErr_DatabaseNotConnected)
		{
/* 
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call 
 */
			for (i = 0;i < RETRY;i++)
			{
#ifdef _OSK
				tsleep(SLEEP_TIME);
#else
				tout.tv_sec = 0;
				tout.tv_usec = 20000;
				select(0,0,0,0,&tout);
#endif /* _OSK */
				recev = db_getres_1(&send,local_cl,&error);
				if(recev == NULL)
				{
					for (j = 0; j < (int)res_num; j++)
						free(send.arr1_val[j]);
					free(send.arr1_val);
					*perr = error;
					return(DS_NOTOK);
				}
				if (recev->db_err == DS_OK || recev->db_err != DbErr_DatabaseNotConnected)
					break;
			}
		}
		if (recev->db_err != DbErr_TooManyInfoForUDP)
		{
			for (j = 0; j < (int)res_num; j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			*perr = recev->db_err;
			return(DS_NOTOK);
		}
/* 
 * If the server answers that there is too many info to send for the UDP
 * protocol, create a TCP connection to the server and redo the call.
 * To be able to correctly close the TCP connection, we must know the 
 * socket number to close (the RPC function does not do this). So, instead
 * of the clnt_create function, we used the clnttcp_create function. 
 */
		else
		{
			if (!first_tcp_call)
			{
#ifdef ALONE
				ht = gethostbyname(serv_name);
#else
				if (i_nethost == 0)
				{
#ifndef vxworks
					ht = gethostbyname(db_info.conf->server_host);
#else /* !vxworks */
					host_addr = hostGetByName(db_info.conf->server_host);
#endif /* !vxworks */
				}
				else
				{
#ifndef vxworks
					ht = gethostbyname(multi_nethost[i_nethost].db_info->server_host);
#else /* !vxworks */
					host_addr = hostGetByName(multi_nethost[i_nethost].db_info->server_host);
#endif /* !vxworks */
				}
#endif /* ALONE */
#ifndef vxworks
				if (ht  == NULL)
#else  /* !vxworks */
				if (host_addr == 0)
#endif /* !vxworks */
				{
					for (j = 0; j < (int)res_num; j++)
						free(send.arr1_val[j]);
					free(send.arr1_val);
					*perr = DbErr_CannotCreateClientHandle;
					return(DS_NOTOK);
				}
				serv_adr.sin_family = AF_INET;
#ifndef vxworks
				memcpy((void *)(&serv_adr.sin_addr),ht->h_addr,(size_t)ht->h_length);
#else  /* !vxworks */
				memcpy((void *)(&serv_adr.sin_addr),(char*)&host_addr, 4);
#endif /* !vxworks */
				first_tcp_call = 1;
			}

			serv_adr.sin_port = 0;
			tcp_so = RPC_ANYSOCK;
#ifdef ALONE
			tcp_cl = clnttcp_create(&serv_adr,DB_SETUPPROG, DB_VERS_3, &tcp_so,0,0);
#else
			tcp_cl = clnttcp_create(&serv_adr, multi_nethost[i_nethost].db_info->prog_number, DB_VERS_3,&tcp_so,0,0);
#endif /* ALONE */
			if (tcp_cl == NULL)
			{
				for (j = 0; j < (int)res_num; j++)
					free(send.arr1_val[j]);
				free(send.arr1_val);
				*perr = DbErr_CannotCreateClientHandle;
				return(DS_NOTOK);
			}

			tcp_used = 1;
			recev = db_getres_1(&send,tcp_cl,&error);
/* 
 * Any problem with server ? 
 */
			if(recev == NULL)
			{
#ifndef WIN32
				close(tcp_so);
#else
				closesocket(tcp_so);
#endif
				clnt_destroy(tcp_cl);
				for (j = 0; j < (int)res_num; j++)
					free(send.arr1_val[j]);
				free(send.arr1_val);
				*perr = error;
				return(DS_NOTOK);
			}

			if (recev->db_err == DbErr_DatabaseNotConnected)
			{
/* 
 * If the server is not connected to the database (because a database update
 * is just going on), sleep a while (20 mS) and redo the call 
 */
				for (i = 0;i < RETRY;i++)
				{
#ifdef _OSK
					tsleep(SLEEP_TIME);
#else
					tout.tv_sec = 0;
					tout.tv_usec = 20000;
					select(0,0,0,0,&tout);
#endif /* _OSK */
					recev = db_getres_1(&send,tcp_cl,&error);
					if(recev == NULL)
					{
#ifndef WIN32
						close(tcp_so);
#else
						closesocket(tcp_so);
#endif
						clnt_destroy(tcp_cl);
						for (j=0;j<(int)res_num;j++)
							free(send.arr1_val[j]);
						free(send.arr1_val);
						*perr = error;
						return(DS_NOTOK);
					}
					if (recev->db_err == DS_OK || recev->db_err != DbErr_DatabaseNotConnected)
						break;
				}
/* 
 * Any problems during database access ? 
 */
				if(recev->db_err != DS_OK)
				{
#ifndef WIN32
					close(tcp_so);
#else
					closesocket(tcp_so);
#endif
					clnt_destroy(tcp_cl);
					for (j = 0; j < (int)res_num; j++)
						free(send.arr1_val[j]);
					free(send.arr1_val);
					*perr = recev->db_err;
					return(DS_NOTOK);
				}

			}
		}
	}
/* 
 * Return memory 
 */
	for (j=0;j<(int)res_num;j++)
		free(send.arr1_val[j]);
	free(send.arr1_val);

/* 
 * Resources type conversion 
 */
	for (i=0;i<(int)res_num;i++) 
	{
		ptrc = recev->res_val.arr1_val[i];
		if (strcmp(ptrc,"N_DEF") == 0)
			continue;
		switch(res[i].resource_type)
		{
			case D_SHORT_TYPE :
				if (ptrc[0] == '0' && ptrc[1] == 'x')
				{
					if (sscanf(&(ptrc[2]),"%hx",(short *)res[i].resource_adr) == -1)
					{
						*perr = DbErr_BadResourceType;
						return(DS_NOTOK);
					}
				}
				else
					*(short *)res[i].resource_adr = (short)atoi(ptrc);
				break;

			case D_USHORT_TYPE :
				if (ptrc[0] == '0' && ptrc[1] == 'x')
				{
					if (sscanf(&(ptrc[2]),"%hx",(short *)res[i].resource_adr) == -1)
					{
						*perr = DbErr_BadResourceType;
						return(DS_NOTOK);
					}
				}
				else
					*(unsigned short *)res[i].resource_adr = (unsigned short)atoi(ptrc);
				break;

			case D_LONG_TYPE :
				if (ptrc[0] == '0' && ptrc[1] == 'x')
				{
					if (sscanf(&(ptrc[2]),"%x",(long *)res[i].resource_adr) == -1)
					{
						*perr = DbErr_BadResourceType;
						return(DS_NOTOK);
					}
				}
				else
					*(long *)res[i].resource_adr = atol(ptrc);
				break;

			case D_ULONG_TYPE :
				if (ptrc[0] == '0' && ptrc[1] == 'x')
				{
					if (sscanf(&(ptrc[2]),"%x",(long *)res[i].resource_adr) == -1)
					{
						*perr = DbErr_BadResourceType;
						return(DS_NOTOK);
					}
				}
				else
					*(unsigned long *)res[i].resource_adr = atol(ptrc);
				break;

			case D_DOUBLE_TYPE :
				*(double *)res[i].resource_adr = atof(ptrc);
				break;

			case D_FLOAT_TYPE :
				*(float *)res[i].resource_adr = (float)atof(ptrc);
				break;

			case D_STRING_TYPE :
				if((ptra = (char *)malloc(strlen(ptrc) + 1)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					return(DS_NOTOK);
				}

				strcpy(ptra,ptrc);
				*(char **)res[i].resource_adr = ptra;
				break;

			case D_BOOLEAN_TYPE :
				str_tolower(ptrc);

				if (!strcmp("off",ptrc) || !strcmp("false",ptrc) || !strcmp("0",ptrc))
					*(char *)res[i].resource_adr = FALSE;
				else if (!strcmp("on",ptrc) || !strcmp("true",ptrc) || !strcmp("1",ptrc))
					*(char *)res[i].resource_adr = TRUE;
				else
				{
					*perr = DbErr_BooleanResource;
					return(DS_NOTOK);
				}	
				break;

			case D_VAR_CHARARR :
/* 
 * If the array has only one element, treat it has a single resource 
 */
				if (ptrc[0] != INIT_ARRAY)
				{
#ifndef OSK
					if ((char_ptr = (char *)malloc((size_t)sizeof(char))) == NULL)
#else
					if ((char_ptr = (char *)malloc(sizeof(char))) == NULL)
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}
					if (ptrc[0] == '0' && ptrc[1] == 'x')
					{
						if (sscanf(&(ptrc[2]),"%hx",&tmp_short) == -1)
						{
							*perr = DbErr_BadResourceType;
							return(DS_NOTOK);
						}
						*char_ptr = (char)tmp_short;
					}
					else
						*char_ptr = (char)atoi(ptrc);
					((DevVarCharArray *)res[i].resource_adr)->sequence = char_ptr;
					((DevVarCharArray *)res[i].resource_adr)->length = 1;
				}
				else
				{
/* 
 * Retrieve the array element number 
 */
#ifndef OSK
					tmp = (char *)strchr(ptrc,SEP_ELT);
#else
					tmp = index(ptrc,SEP_ELT);
#endif /* OSK */
					diff = (u_int)(tmp++ - ptrc) - 1;
					strncpy(numb,&ptrc[1],diff);
					numb[diff] = 0;
					ctr = (u_int)atoi(numb) - 1;

#ifndef OSK
					if ((char_ptr = (char *)calloc((size_t)(ctr + 1),(size_t)sizeof(char))) == NULL)
#else
					if ((char_ptr = (char *)calloc((ctr + 1),sizeof(char))) == NULL)
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}
/* 
 * Convert each array element 
 */
					for (l=0;l<ctr;l++)
					{
#ifndef OSK
						temp = (char *)strchr(tmp,SEP_ELT);
#else
						temp = index(tmp,SEP_ELT);
#endif /* OSK */
						diff = (u_int)(temp++ - tmp);
						strncpy(numb,tmp,diff);
						numb[diff] = 0;
						if (numb[0] == '0' && numb[1] == 'x')
						{
							if (sscanf(&(numb[2]),"%hx",&tmp_short) == -1)
							{
								*perr = DbErr_BadResourceType;
								return(DS_NOTOK);
							}
							char_ptr[l] = (char)tmp_short;
						}
						else
							char_ptr[l] = (char)atoi(numb);
						tmp = temp;
					}
					if (tmp[0] == '0' && tmp[1] == 'x')
					{
						if (sscanf(&(tmp[2]),"%hx",&tmp_short) == -1)
						{
							*perr = DbErr_BadResourceType;
							return(DS_NOTOK);
						}
						char_ptr[l] = (char)tmp_short;
					}
					else 
						char_ptr[l] = (char)atoi(tmp);

					((DevVarCharArray *)res[i].resource_adr)->sequence = char_ptr;
					((DevVarCharArray *)res[i].resource_adr)->length = ctr + 1;
				}
				break;

			case D_VAR_SHORTARR :
/* 
 * If the array has only one element, treat it as a single resource 
 */
				if (ptrc[0] != INIT_ARRAY) 
				{
#ifndef OSK
					if ((short_ptr = (short *)malloc((size_t)sizeof(short))) == NULL) 
#else
					if ((short_ptr = (short *)malloc(sizeof(short))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}
					if (ptrc[0] == '0' && ptrc[1] == 'x') 
					{
						if (sscanf(&(ptrc[2]),"%hx",short_ptr) == -1) 
						{
							*perr = DbErr_BadResourceType;
							return(DS_NOTOK);
						}
					}
					else 
						*short_ptr = (short)atoi(ptrc);

					((DevVarShortArray *)res[i].resource_adr)->sequence = short_ptr;
					((DevVarShortArray *)res[i].resource_adr)->length = 1;
				}
				else 
				{
/* 
 * Retrieve the array element number 
 */
#ifndef OSK
					tmp = (char *)strchr(ptrc,SEP_ELT);
#else
					tmp = index(ptrc,SEP_ELT);
#endif /* OSK */
					diff = (u_int)(tmp++ - ptrc) - 1;
					strncpy(numb,&ptrc[1],diff);
					numb[diff] = 0;
					ctr = (u_int)atoi(numb) - 1;

#ifndef OSK
					if ((short_ptr = (short *)calloc((size_t)(ctr + 1),(size_t)sizeof(short))) == NULL) 
#else
					if ((short_ptr = (short *)calloc((ctr + 1),sizeof(short))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}
/* 
 * Convert each array element 
 */
					for (l = 0; l < ctr; l++) 
					{
#ifndef OSK
						temp = (char *)strchr(tmp,SEP_ELT);
#else
						temp = index(tmp,SEP_ELT);
#endif /* OSK */
						diff = (u_int)(temp++ - tmp);
						strncpy(numb,tmp,diff);
						numb[diff] = 0;
						if (numb[0] == '0' && numb[1] == 'x') 
						{
							if (sscanf(&(numb[2]),"%hx",&(short_ptr[l])) == -1) 
							{
								*perr = DbErr_BadResourceType;
								return(DS_NOTOK);
						  	}
						}
						else
							short_ptr[l] = (short)atoi(numb);
						tmp = temp;
					}
					if (tmp[0] == '0' && tmp[1] == 'x') 
					{
						if (sscanf(&(tmp[2]),"%hx",&(short_ptr[l])) == -1) 
						{
							*perr = DbErr_BadResourceType;
							return(DS_NOTOK);
						}
					}
					else 
						short_ptr[l] = (short)atoi(tmp);

					((DevVarShortArray *)res[i].resource_adr)->sequence = short_ptr;
					((DevVarShortArray *)res[i].resource_adr)->length = ctr + 1;
				}
				break;

			case D_VAR_LONGARR :
/* 
 * If the array has only one element, treat it as a single resource 
 */
				if (ptrc[0] != INIT_ARRAY) 
				{
#ifndef OSK
					if ((long_ptr = (DevLong *)malloc((size_t)sizeof(DevLong))) == NULL) 
#else
					if ((long_ptr = (DevLong *)malloc(sizeof(DevLong))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

					if (ptrc[0] == '0' && ptrc[1] == 'x') 
					{
						if (sscanf(&(ptrc[2]),"%x",long_ptr) == -1) 
						{
							*perr = DbErr_BadResourceType;
							return(DS_NOTOK);
						}
					}
			    		else 
						*long_ptr = atol(ptrc);

					((DevVarLongArray *)res[i].resource_adr)->sequence = long_ptr;
					((DevVarLongArray *)res[i].resource_adr)->length = 1;
				}
				else 
				{
/* 
 * Retrieve the array element number 
 */
#ifndef OSK
					tmp = (char *)strchr(ptrc,SEP_ELT);
#else
					tmp = index(ptrc,SEP_ELT);
#endif /* OSK */
					diff = (u_int)(tmp++ - ptrc) - 1;
					strncpy(numb,&ptrc[1],diff);
					numb[diff] = 0;
					ctr = (u_int)atoi(numb) - 1;

#ifndef OSK
					if ((long_ptr = (DevLong *)calloc((size_t)(ctr + 1),(size_t)sizeof(DevLong))) == NULL) 
#else
					if ((long_ptr = (DevLong *)calloc((ctr + 1),sizeof(DevLong))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}
/* 
 * Convert each array element 
 */
					for (l = 0; l < ctr; l++) 
					{
#ifndef OSK
						temp = (char *)strchr(tmp,SEP_ELT);
#else
						temp = index(tmp,SEP_ELT);
#endif /* OSK */
						diff = (u_int)(temp++ - tmp);
						strncpy(numb,tmp,diff);
						numb[diff] = 0;
						if (numb[0] == '0' && numb[1] == 'x') 
						{
							if (sscanf(&(numb[2]),"%lx",&(long_ptr[l])) == -1) 
							{
								*perr = DbErr_BadResourceType;
								return(DS_NOTOK);
							}
						}
						else
							long_ptr[l] = atol(numb);
						tmp = temp;
					}
					if (tmp[0] == '0' && tmp[1] == 'x') 
					{
						if (sscanf(&(tmp[2]),"%lx",&(long_ptr[l])) == -1) 
						{
							*perr = DbErr_BadResourceType;
							return(DS_NOTOK);
						}
					}
					else 
						long_ptr[l] = atol(tmp);

					((DevVarLongArray *)res[i].resource_adr)->sequence = long_ptr;
					((DevVarLongArray *)res[i].resource_adr)->length = ctr + 1;
				}
				break;

			case D_VAR_DOUBLEARR :
/* 
 * If the array has only one element, treat it as a single resource 
 */
				if (ptrc[0] != INIT_ARRAY) 
				{
#ifndef OSK
					if ((double_ptr = (double *)malloc((size_t)sizeof(double))) == NULL) 
#else
					if ((double_ptr = (double *)malloc(sizeof(double))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

					*double_ptr = atof(ptrc);

					((DevVarDoubleArray *)res[i].resource_adr)->sequence = double_ptr;
					((DevVarDoubleArray *)res[i].resource_adr)->length = 1;
				}
				else 
				{

/* Retrieve the array element number */

#ifndef OSK
					tmp = (char *)strchr(ptrc,SEP_ELT);
#else
					tmp = index(ptrc,SEP_ELT);
#endif /* OSK */
					diff = (u_int)(tmp++ - ptrc) - 1;
					strncpy(numb,&ptrc[1],diff);
					numb[diff] = 0;
					ctr = (u_int)atoi(numb) - 1;

#ifndef OSK
					if ((double_ptr = (double *)calloc((size_t)(ctr + 1),(size_t)sizeof(double))) == NULL) 
#else
					if ((double_ptr = (double *)calloc((ctr + 1),sizeof(double))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

/* Convert each array element */
					for (l=0;l<ctr;l++) 
					{
#ifndef OSK
						temp = (char *)strchr(tmp,SEP_ELT);
#else
						temp = index(tmp,SEP_ELT);
#endif /* OSK */
						diff = (u_int)(temp++ - tmp);
						strncpy(numb,tmp,diff);
						numb[diff] = 0;
						double_ptr[l] = atof(numb);
						tmp = temp;
					}
					double_ptr[l] = atof(tmp);

					((DevVarDoubleArray *)res[i].resource_adr)->sequence = double_ptr;
					((DevVarDoubleArray *)res[i].resource_adr)->length = ctr + 1;
				}
				break;

			case D_VAR_FLOATARR :
/* 
 * If the array has only one element, treat it as a single resource 
 */
				if (ptrc[0] != INIT_ARRAY) 
				{
#ifndef OSK
					if ((float_ptr = (float *)malloc((size_t)sizeof(float))) == NULL) 
#else
					if ((float_ptr = (float *)malloc(sizeof(float))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

					*float_ptr = (float)atof(ptrc);

					((DevVarFloatArray *)res[i].resource_adr)->sequence = float_ptr;
					((DevVarFloatArray *)res[i].resource_adr)->length = 1;
				}
				else 
				{

/* Retrieve the array element number */

#ifndef OSK
					tmp = (char *)strchr(ptrc,SEP_ELT);
#else
					tmp = index(ptrc,SEP_ELT);
#endif /* OSK */
					diff = (u_int)(tmp++ - ptrc) - 1;
					strncpy(numb,&ptrc[1],diff);
					numb[diff] = 0;
					ctr = (u_int)atoi(numb) - 1;

#ifndef OSK
					if ((float_ptr = (float *)calloc((size_t)(ctr + 1),(size_t)sizeof(float))) == NULL) 
#else
					if ((float_ptr = (float *)calloc((ctr + 1),sizeof(float))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

/* Convert each array element */

					for (l=0;l<ctr;l++) 
					{
#ifndef OSK
						temp = (char *)strchr(tmp,SEP_ELT);
#else
						temp = index(tmp,SEP_ELT);
#endif /* OSK */
						diff = (u_int)(temp++ - tmp);
						strncpy(numb,tmp,diff);
						numb[diff] = 0;
						float_ptr[l] = (float)atof(numb);
						tmp = temp;
					}
					float_ptr[l] = (float)atof(tmp);

					((DevVarFloatArray *)res[i].resource_adr)->sequence = float_ptr;
					((DevVarFloatArray *)res[i].resource_adr)->length = ctr + 1;
				}
				break;
			case D_VAR_STRINGARR :
				if (ptrc[0] != INIT_ARRAY) 
				{
#ifndef OSK
					if ((str_ptr = (char **)malloc((size_t)sizeof(char *))) == NULL) 
#else
					if ((str_ptr = (char **)malloc(sizeof(char *))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

					if((str_ptr[0] = (char *)malloc(strlen(ptrc) + 1)) == NULL)  
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

					strcpy(str_ptr[0],ptrc);

					((DevVarStringArray *)res[i].resource_adr)->sequence = str_ptr;
					((DevVarStringArray *)res[i].resource_adr)->length = 1;
				}
				else 
				{
/* 
 * Retrieve the array element number 
 */
#ifndef OSK
					tmp = (char *)strchr(ptrc,SEP_ELT);
#else
					tmp = index(ptrc,SEP_ELT);
#endif /* OSK */
					diff = (u_int)(tmp++ - ptrc) - 1;
					strncpy(numb,&ptrc[1],diff);
					numb[diff] = 0;
					ctr = (u_int)atoi(numb) - 1;

/* 
 * Allocate memory for the array of pointer to strings 
 */
#ifndef OSK
					if ((str_ptr = (char **)calloc((size_t)(ctr + 1),(size_t)sizeof(char *))) == NULL) 
#else
					if ((str_ptr = (char **)calloc((ctr + 1),sizeof(char *))) == NULL) 
#endif /* OSK */
					{
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}

/* 
 * Convert each array element 
 */
					for (l = 0; l < ctr; l++) 
					{
#ifndef OSK
						temp = (char *)strchr(tmp,SEP_ELT);
#else
						temp = index(tmp,SEP_ELT);
#endif /* OSK */
						diff = (u_int)(temp++ - tmp);
						if ((str_ptr[l] = (char *)malloc(diff + 1)) == NULL) 
						{
							for(k = 0; k < l; k++)
								free(str_ptr[k]);
							free(str_ptr);
							*perr = DbErr_ClientMemoryAllocation;
							return(DS_NOTOK);
						}
						strncpy(str_ptr[l],tmp,diff);
						str_ptr[l][diff] = 0;
						tmp = temp;
					}
					if ((str_ptr[l] = (char *)malloc(strlen(tmp) + 1)) == NULL) 
					{
						for(k=0;k<l;k++)
							free(str_ptr[k]);
						free(str_ptr);
						*perr = DbErr_ClientMemoryAllocation;
						return(DS_NOTOK);
					}
					strcpy(str_ptr[l],tmp);

					((DevVarStringArray *)res[i].resource_adr)->sequence = str_ptr;
					((DevVarStringArray *)res[i].resource_adr)->length = ctr + 1;
				}
				break;
		}
	}
/* 
 * Return memory allocated by XDR routines. If the TCP connection has been
 * used, close the associated socket and destroy the RPC connection. 
 */
	if (tcp_used == 1)
	{
		if (!clnt_freeres(tcp_cl,(xdrproc_t)xdr_db_res,(char *)recev))
		{
			*perr = DbErr_MemoryFree;
			return(DS_NOTOK);
		}
#ifndef WIN32
		close(tcp_so);
#else
		closesocket(tcp_so);
#endif
		clnt_destroy(tcp_cl);
	}
	else
	{
		if (!clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev)) 
		{
			*perr = DbErr_MemoryFree;
			return(DS_NOTOK);
		}
	}

/* 
 * No error 
 */
	*perr = DS_OK;
	return(DS_OK);
}
 


/**@ingroup dbaseAPIresource
 * Updates already defined resource(s) or adds new resources(s) if it (they) 
 * do(es) not exist. Resource files are not updated by this function. It is not possible
 * to update/insert resource(s) belonging to the SEC domain.
 * 
 * @param dev_name 	a string which defines the device name
 * @param res      	A pointer to an array of db_resource structure defining the 
 *                 	resources to be updated or inserted
 * @param res_num  	The number of resource to be retrieved 
 * @param perr		The error code if any
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_putresource(const char *dev_name, Db_resource res, u_int res_num, DevLong *perr)
{
	int i,j,k,k1,l;
	int k2 = 0;
	tab_putres send;
	char tmp_rname[SIZE_C];
	char d_name[SIZE_B];
	register char *temp;
	u_int diff;
	int big_res;
	putres *tmp;
	char *tmp_arr;
	char	*devname;
	DevVarCharArray *tmp_char;
	DevVarStringArray *tmp_string;
	DevVarShortArray *tmp_short;
	DevVarLongArray *tmp_long;
	DevVarFloatArray *tmp_float;
	DevVarDoubleArray *tmp_double;
	u_int len = 0;
	int *recev;
	int big_packet = 0;
	CLIENT *tcp_cl;
	CLIENT 		*local_cl;
	int tcp_so;
	DevLong error;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Verify function parameters (non NULL pointers and two \ characters in 
   device name). */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

	if (dev_name == NULL || res == NULL || res_num == 0)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	l = 0;
	NB_CHAR(l,dev_name,'/');
	if (l != 2)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	for (i=0;i<(int)res_num;i++)
	{
		if (res[i].resource_adr == NULL || res[i].resource_name == NULL)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
		if (res[i].resource_type == D_VAR_CHARARR && 
 		    ((DevVarCharArray *)(res[i].resource_adr))->length == 0)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
		else if (res[i].resource_type == D_VAR_SHORTARR &&
			((DevVarShortArray *)(res[i].resource_adr))->length == 0)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
		else if (res[i].resource_type == D_VAR_LONGARR &&
			((DevVarLongArray *)(res[i].resource_adr))->length == 0)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
		else if (res[i].resource_type == D_VAR_DOUBLEARR &&
			((DevVarDoubleArray *)(res[i].resource_adr))->length == 0)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
		else if (res[i].resource_type == D_VAR_FLOATARR &&
			((DevVarFloatArray *)(res[i].resource_adr))->length == 0)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
		else if (res[i].resource_type == D_VAR_STRINGARR &&
			((DevVarStringArray *)(res[i].resource_adr))->length == 0)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}

/* If the users try to update resource of the "sec" domain, refuse to do
   the work */

#ifdef OSK
	tmp_arr = index(dev_name,'/');
#else
	tmp_arr = (char *)strchr(dev_name,'/');
#endif /* OSK */
	diff = (u_int)(tmp_arr - dev_name);
	strncpy_tolower(tmp_rname,dev_name,diff);
	tmp_rname[diff] = 0;
	if (strcmp(tmp_rname,"sec") == 0)
	{
		*perr = DbErr_DomainDefinition;
		return(DS_NOTOK);
	}

#ifdef ALONE
/* Create RPC connection if it is the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			clnt_pcreateerror("why : ");
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
	devname = dev_name;
	local_cl;
#else
/*
 * find out which nethost has been requested for this device
 */
	if ((i_nethost = get_i_nethost_by_device_name(dev_name,perr)) < 0)
	{
/* 
 * The nethost is not imported, extract nethost name and import it 
 */
		char *nethost = extract_nethost(dev_name, perr);
		if (*perr != DS_OK)
			return DS_NOTOK;
/* 
 * The specified nethost is not in the list of imorted nethosts, therefore 
 * call setup_config_multi() to add it 
 */
		if (setup_config_multi(nethost,perr) != DS_OK)
			return(DS_NOTOK);
/* 
 * Find where the nethost is in the multi-nethost array 
 */
		i_nethost = get_i_nethost_by_name(nethost,perr);
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
		*perr = DbErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	devname = extract_device_name(dev_name,perr);
	local_cl = multi_nethost[i_nethost].db_info->clnt;
#endif /* ALONE */

/* Allocate memory for the array putres structure */

#ifndef OSK
	if ((send.tab_putres_val = (putres *)calloc((size_t)res_num,(size_t)sizeof(putres))) == NULL)
	{
#else
	if ((send.tab_putres_val = (putres *)calloc(res_num,sizeof(putres))) == NULL)
	{
#endif /* OSK */
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	k1 = strlen(devname);
	for (i = 0;i < (int)res_num;i++)
	{

/* Build and allocate memory for the full resource name.
   Change the resource name to lowercase letters. */

		tmp = &send.tab_putres_val[i];
		l = strlen(res[i].resource_name);
		if ((tmp->res_name = (char *)malloc(k1 + l + 2)) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			for (j=0;j<i;j++)
			{
				free(send.tab_putres_val[j].res_name);
				free(send.tab_putres_val[j].res_val);
			}
			free(send.tab_putres_val);
			return(DS_NOTOK);
		}

		strcpy_tolower(tmp->res_name,devname);
		strcat(tmp->res_name, "/");
		strcat_tolower(tmp->res_name,res[i].resource_name);

/* Build a string with the resource value */

		switch(res[i].resource_type)
		{
			case D_SHORT_TYPE :
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(10)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert resource value to string */
				snprintf(tmp->res_val, 10, "%hd",*(short *)res[i].resource_adr);
				break;

			case D_USHORT_TYPE :
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(10)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert resource value to string */
				snprintf(tmp->res_val, 10, "%hu",*(unsigned short *)res[i].resource_adr);
				break;

			case D_LONG_TYPE :
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(16)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert resource value to string */
				snprintf(tmp->res_val, 16, "%ld",*(long *)res[i].resource_adr);
				break;
			case D_ULONG_TYPE :
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(16)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert resource value to string */
				snprintf(tmp->res_val, 16, "%ul",*(unsigned long *)res[i].resource_adr);
				break;
			case D_FLOAT_TYPE :
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(SIZE_A)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert resource value to string */
				snprintf(tmp->res_val, SIZE_A, "%.8e",*(float *)res[i].resource_adr);
				break;
			case D_DOUBLE_TYPE :
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(SIZE_A)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert resource value to string */
				snprintf(tmp->res_val, SIZE_A, "%.8e",*(double *)res[i].resource_adr);
				break;
			case D_STRING_TYPE :
/* Verify that the string is not too long */
				l = strlen(*(char **)res[i].resource_adr);
				if (l > MAX_RES)
				{
					*perr = DbErr_StringTooLong;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
	
/* Allocate memory for the string */
				if ((tmp->res_val = (char *)malloc(l + 1)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Copy the string */
				strcpy(tmp->res_val,*(char **)res[i].resource_adr);
				break;
			case D_BOOLEAN_TYPE :
/* Allocate memory for the string "Off" */
				if ((tmp->res_val = (char *)malloc(4)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					return(DS_NOTOK);
				}
/* Convert the boolean value to string */
				if (*(char *)res[i].resource_adr == TRUE)
					strcpy(tmp->res_val,"On");
				else
					strcpy(tmp->res_val,"Off");
				break;
			case D_VAR_CHARARR :
				tmp_char = (DevVarCharArray *)res[i].resource_adr;
/* Allocate memory for the temporary string */
				if (k2 == 0) 
				{
					if ((tmp_arr = (char *)malloc(SIZE)) == NULL) 
					{
						*perr = DbErr_ClientMemoryAllocation;
						for (j=0;j<i;j++) 
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						return(DS_NOTOK);
					}
					tmp_arr[0] = 0;
					k2 = 1;
				}   

/* Build the temporary string and realloc memory for the temporary string if it is needed. */

				for (j=0;j<(int)tmp_char->length;j++) 
				{

					k = strlen(tmp_arr);
					tmp_arr[k++] = SEP_ELT;
					snprintf(&tmp_arr[k], LIM, "%d",tmp_char->sequence[j]);
					if (k > ((k2 * SIZE) - LIM)) 
					{
						if ((tmp_arr = (char *)realloc((void *)tmp_arr,(k2 + 1) * SIZE)) == NULL) 
						{
							*perr = DbErr_ClientMemoryAllocation;
							for (j=0;j<i;j++) 
							{
								free(send.tab_putres_val[j].res_name);
								free(send.tab_putres_val[j].res_val);
							}
							free(tmp->res_name);
							free(send.tab_putres_val);
							free(tmp_arr);
							return(DS_NOTOK);
						}
						k2++;
					}
				}
/* Allocate memory for the full string (header and array elements) */
				if ((tmp->res_val = (char *)malloc(strlen(tmp_arr) + 10)) == NULL) 
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++) 
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}
/* Copy the temporary string */
				tmp->res_val[0] = INIT_ARRAY;
				snprintf(&tmp->res_val[1], 10, "%d",tmp_char->length);
				strcat(tmp->res_val,tmp_arr);
/* Clear the temporary buffer */
				tmp_arr[0] = 0;
				break;
			case D_VAR_SHORTARR :
				tmp_short = (DevVarShortArray *)res[i].resource_adr;
/* Allocate memory for the temporary string */
				if (k2 == 0) 
				{
					if ((tmp_arr = (char *)malloc(SIZE)) == NULL) 
					{
						*perr = DbErr_ClientMemoryAllocation;
						for (j=0;j<i;j++) 
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						return(DS_NOTOK);
					}
					tmp_arr[0] = 0;
					k2 = 1;
				}   
/* Build the temporary string and realloc memory for the temporary string if it is needed. */
				for (j=0;j<(int)tmp_short->length;j++) 
				{
					k = strlen(tmp_arr);
					tmp_arr[k++] = SEP_ELT;
					snprintf(&tmp_arr[k], LIM, "%d",tmp_short->sequence[j]);
					if (k > ((k2 * SIZE) - LIM)) 
					{
						if ((tmp_arr = (char *)realloc((void *)tmp_arr,(k2 + 1) * SIZE)) == NULL) 
						{
							*perr = DbErr_ClientMemoryAllocation;
							for (j=0;j<i;j++) 
							{
								free(send.tab_putres_val[j].res_name);
								free(send.tab_putres_val[j].res_val);
							}
							free(tmp->res_name);
							free(send.tab_putres_val);
							free(tmp_arr);
							return(DS_NOTOK);
						}
						k2++;
					}
				}
/* Allocate memory for the full string (header and array) */
				if ((tmp->res_val = (char *)malloc(strlen(tmp_arr) + 10)) == NULL) 
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++) 
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}
/* Copy the temporary string */
				tmp->res_val[0] = INIT_ARRAY;
				snprintf(&tmp->res_val[1], 10, "%d",tmp_short->length);
				strcat(tmp->res_val,tmp_arr);
/* Clear the temporary buffer */
				tmp_arr[0] = 0;
				break;
			case D_VAR_LONGARR :
				tmp_long = (DevVarLongArray *)res[i].resource_adr;
/* Allocate memory for the temporary string */
				if (k2 == 0) 
				{
					if ((tmp_arr = (char *)malloc(SIZE)) == NULL) 
					{
						*perr = DbErr_ClientMemoryAllocation;
						for (j=0;j<i;j++) 
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						return(DS_NOTOK);
					}
					tmp_arr[0] = 0;
					k2 = 1;
				}   
/* Build the temporary string and realloc memory for the temporary string if it is needed. */
				for (j=0;j<(int)tmp_long->length;j++) 
				{
					k = strlen(tmp_arr);
					tmp_arr[k++] = SEP_ELT;
					snprintf(&tmp_arr[k], LIM, "%d",tmp_long->sequence[j]);
					if (k > ((k2 * SIZE) - LIM)) 
					{
						if ((tmp_arr = (char *)realloc((void *)tmp_arr,(k2 + 1) * SIZE)) == NULL) 
						{
							*perr = DbErr_ClientMemoryAllocation;
							for (j=0;j<i;j++) 
							{
								free(send.tab_putres_val[j].res_name);
								free(send.tab_putres_val[j].res_val);
							}
							free(tmp->res_name);
							free(send.tab_putres_val);
							free(tmp_arr);
							return(DS_NOTOK);
						}
						k2++;
					}
				}
/* Allocate memory for the full string (header and array) */
				if ((tmp->res_val = (char *)malloc(strlen(tmp_arr) + 10)) == NULL) 
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++) 
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}
/* Copy the temporary string */
				tmp->res_val[0] = INIT_ARRAY;
				snprintf(&tmp->res_val[1], 10, "%d",tmp_long->length);
				strcat(tmp->res_val,tmp_arr);
/* Clear the temporary buffer */
				tmp_arr[0] = 0;
				break;
			case D_VAR_FLOATARR :
				tmp_float = (DevVarFloatArray *)res[i].resource_adr;
/* Allocate memory for the temporary string */
				if (k2 == 0) 
				{
					if ((tmp_arr = (char *)malloc(SIZE)) == NULL) 
					{
						*perr = DbErr_ClientMemoryAllocation;
						for (j=0;j<i;j++) 
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						return(DS_NOTOK);
					}
					tmp_arr[0] = 0;
					k2 = 1;
				}   
/* Build a string with only the array elements */
				for (j=0;j<(int)tmp_float->length;j++) 
				{
					k = strlen(tmp_arr);
					tmp_arr[k++] = SEP_ELT;
					snprintf(&tmp_arr[k], LIM, "%.8e",tmp_float->sequence[j]);

					if (k > ((k2 * SIZE) - LIM)) 
					{
						if ((tmp_arr = (char *)realloc((void *)tmp_arr,(k2 + 1) * SIZE)) == NULL) 
						{
							*perr = DbErr_ClientMemoryAllocation;
							for (j=0;j<i;j++) 
							{
								free(send.tab_putres_val[j].res_name);
								free(send.tab_putres_val[j].res_val);
							}
							free(tmp->res_name);
							free(send.tab_putres_val);
							free(tmp_arr);
							return(DS_NOTOK);
						}
						k2++;
					}
				}
/* Allocate memory for the full string (header and array) */
				if ((tmp->res_val = (char *)malloc(strlen(tmp_arr) + 10)) == NULL) 
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++) 
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}
/* Copy the temporary string */
				tmp->res_val[0] = INIT_ARRAY;
				snprintf(&tmp->res_val[1], 10, "%d",tmp_float->length);
				strcat(tmp->res_val,tmp_arr);
/* Clear the temporary buffer */
				tmp_arr[0] = 0;
				break;
			case D_VAR_DOUBLEARR :
				tmp_double = (DevVarDoubleArray *)res[i].resource_adr;
/* Allocate memory for the temporary string */
				if (k2 == 0) 
				{
					if ((tmp_arr = (char *)malloc(SIZE)) == NULL) 
					{
						*perr = DbErr_ClientMemoryAllocation;
						for (j=0;j<i;j++) 
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						return(DS_NOTOK);
					}
					tmp_arr[0] = 0;
					k2 = 1;
				}   
/* Build a string with only the array elements */
				for (j=0;j<(int)tmp_double->length;j++) 
				{
					k = strlen(tmp_arr);
					tmp_arr[k++] = SEP_ELT;
					snprintf(&tmp_arr[k], LIM, "%.8e",tmp_double->sequence[j]);
					if (k > ((k2 * SIZE) - LIM)) 
					{
						if ((tmp_arr = (char *)realloc((void *)tmp_arr,(k2 + 1) * SIZE)) == NULL) 
						{
							*perr = DbErr_ClientMemoryAllocation;
							for (j=0;j<i;j++) 
							{
								free(send.tab_putres_val[j].res_name);
								free(send.tab_putres_val[j].res_val);
							}
							free(tmp->res_name);
							free(send.tab_putres_val);
							free(tmp_arr);
							return(DS_NOTOK);
						}
						k2++;
					}
				}
/* Allocate memory for the full string (header and array) */
				if ((tmp->res_val = (char *)malloc(strlen(tmp_arr) + 10)) == NULL) 
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++) 
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}
/* Copy the temporary string */
				tmp->res_val[0] = INIT_ARRAY;
				snprintf(&tmp->res_val[1], 10, "%d",tmp_double->length);
				strcat(tmp->res_val,tmp_arr);
/* Clear the temporary buffer */
				tmp_arr[0] = 0;
				break;
			case D_VAR_STRINGARR :
				tmp_string = (DevVarStringArray *)res[i].resource_adr;
/* Allocate memory for the temporary string */
				if (k2 == 0)
				{
					if ((tmp_arr = (char *)malloc(SIZE)) == NULL)
					{
						*perr = DbErr_ClientMemoryAllocation;
						for (j=0;j<i;j++)
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						return(DS_NOTOK);
					}
					tmp_arr[0] = 0;
					k2 = 1;
				}   
/* Build a string with only the array elements */
				for (j=0;j<(int)tmp_string->length;j++)
				{
					if (strlen(tmp_string->sequence[j]) > (size_t)MAX_RES)
					{
						*perr = DbErr_StringTooLong;
						for (j=0;j<i;j++)
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						free(tmp_arr);
						return(DS_NOTOK);
					}
/*
 *  STRANGE - why check the string length against STRING_SIZE when you have
 *            just checked it against MAX_RES ... ?! Skip this check - andy 25mar05
 *
					if (strlen(tmp_string->sequence[j]) > (size_t)STRING_SIZE) 
					{
						*perr = DbErr_StringTooLong;
						for (j=0;j<i;j++) 
						{
							free(send.tab_putres_val[j].res_name);
							free(send.tab_putres_val[j].res_val);
						}
						free(tmp->res_name);
						free(send.tab_putres_val);
						free(tmp_arr);
						return(DS_NOTOK);
					}
 */
					k = strlen(tmp_arr);
					tmp_arr[k++] = SEP_ELT;
					tmp_arr[k] = 0;
					strcat(tmp_arr,tmp_string->sequence[j]);

					if (k > ((k2 * SIZE) - LIM))
					{
						if ((tmp_arr = (char *)realloc((void *)tmp_arr,(k2 + 1) * SIZE)) == NULL)
						{
							*perr = DbErr_ClientMemoryAllocation;
							for (j=0;j<i;j++)
							{
								free(send.tab_putres_val[j].res_name);
								free(send.tab_putres_val[j].res_val);
							}
							free(tmp->res_name);
							free(send.tab_putres_val);
							free(tmp_arr);
							return(DS_NOTOK);
						}
						k2++;
					}
				}
/* Allocate memory for the full string (header and array) */
				if ((tmp->res_val = (char *)malloc(strlen(tmp_arr) + 10)) == NULL)
				{
					*perr = DbErr_ClientMemoryAllocation;
					for (j=0;j<i;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(tmp->res_name);
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}
/* Copy the temporary string */
				tmp->res_val[0] = INIT_ARRAY;
				snprintf(&tmp->res_val[1], 10, "%d",tmp_string->length);
				strcat(tmp->res_val,tmp_arr);
/* Clear the temporary buffer */
				tmp_arr[0] = 0;
				break;
		}

/* Compute an estimation of the network packet size. If it exceeds 8K, set a flag. */

		if ((len = strlen(send.tab_putres_val[i].res_val) + strlen(send.tab_putres_val[i].res_name) + len) > SIZE)
		{
			big_packet = 1;
		}
	}

/* Initialize the structure sended to server */

	send.tab_putres_len = res_num;

/* If it is necessary, create a TCP connection to be able to send more than
   8K of data. */

	if (big_packet == 1)
	{

/* To be able to correctly close the TCP connection, we must know the 
   socket number to close it (the RPC function does not do this). So, instead
   of the clnt_create function, we used the clnttcp_create function. */

		if (!first_tcp_call)
		{
#ifdef ALONE
			if ((ht = gethostbyname(serv_name)) == NULL)
			{
#else
#ifndef vxworks
				if ((ht = gethostbyname(db_info.conf->server_host)) == NULL)
				{
#else  /* !vxworks */
				if ((host_addr = hostGetByName(db_info.conf->server_host)) == 0)
				{
#endif /* !vxworks */
#endif /* ALONE */
					*perr = DbErr_CannotCreateClientHandle;
					for (j = 0;j < (int)res_num;j++)
					{
						free(send.tab_putres_val[j].res_name);
						free(send.tab_putres_val[j].res_val);
					}
					free(send.tab_putres_val);
					free(tmp_arr);
					return(DS_NOTOK);
				}

				serv_adr.sin_family = AF_INET;
#ifndef vxworks
				memcpy((void *)(&serv_adr.sin_addr),ht->h_addr,(size_t)ht->h_length);
#else  /* !vxworks */
				memcpy((void *)(&serv_adr.sin_addr),(char*)&host_addr,4);
#endif /* !vxworks */
				first_tcp_call = 1;
			}

			serv_adr.sin_port = 0;
			tcp_so = RPC_ANYSOCK;
#ifdef ALONE
			tcp_cl = clnttcp_create(&serv_adr,DB_SETUPPROG,
					DB_VERS_3, &tcp_so,0,0);
#else
			tcp_cl = clnttcp_create(&serv_adr,
					multi_nethost[i_nethost].db_info->prog_number,
				        DB_VERS_3,&tcp_so,0,0);
#endif /* ALONE */
			if (tcp_cl == NULL)
			{
				*perr = DbErr_CannotCreateClientHandle;
				for (j = 0;j < (int)res_num;j++)
				{
					free(send.tab_putres_val[j].res_name);
					free(send.tab_putres_val[j].res_val);
				}
				free(send.tab_putres_val);
				free(tmp_arr);
				return(DS_NOTOK);
			}
		}

/* Call server */

#ifdef ALONE
	if (big_packet == 1)
		recev = db_putres_1(&send,tcp_cl,&error);
	else
		recev = db_putres_1(&send,cl,&error);
#else
	if (big_packet == 1)
		recev = db_putres_1(&send,tcp_cl,&error);
	else
					
		recev = db_putres_1(&send,multi_nethost[i_nethost].db_info->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if(recev != NULL) {
	if (*recev == DbErr_DatabaseNotConnected) {
		for (i = 0;i < RETRY;i++) {

#ifdef _OSK
			tsleep(SLEEP_TIME);
#else
			tout.tv_sec = 0;
			tout.tv_usec = 20000;
			select(0,0,0,0,&tout);
#endif /* _OSK */
#ifdef ALONE
			if (big_packet == 1)
				recev = db_putres_1(&send,tcp_cl,&error);
			else
				recev = db_putres_1(&send,cl,&error);
#else
			if (big_packet == 1)
				recev = db_putres_1(&send,tcp_cl,&error);
			else
				recev = db_putres_1(&send,multi_nethost[i_nethost].db_info->clnt,&error);
#endif /* ALONE */
			if(recev == NULL)
				break;
			if ((*recev == DS_OK) || (*recev != DbErr_DatabaseNotConnected))
				break;
						}
							}
			}

/* Any problem with server ?. If yes, and if it is a time-out, try to
   reconnect. Don't forget to free memory. */

	if (recev == NULL) {
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&send,(void **)&recev,&cl,(int)DB_PUTRES,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&send,(void **)&recev,
					&multi_nethost[i_nethost].db_info->clnt,(int)DB_PUTRES,
					i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}

		if (error != DS_OK) {
			for (j = 0;j < (int)res_num;j++) {
				free(send.tab_putres_val[j].res_name);
				free(send.tab_putres_val[j].res_val);
						}
			free(send.tab_putres_val);
			if (k2 != 0)
				free(tmp_arr);
			if (big_packet == 1) {
#ifndef WIN32
				close(tcp_so);
#else
				closesocket(tcp_so);
#endif
				clnt_destroy(tcp_cl);
						}
			*perr = error;
			return(DS_NOTOK);
				}
			}

/* Return memory */

	for (j = 0;j < (int)res_num;j++) {
		free(send.tab_putres_val[j].res_name);
		free(send.tab_putres_val[j].res_val);
				}
	free(send.tab_putres_val);
	if (k2 != 0)
		free(tmp_arr);

/* Any problem during database access ? */

	if (*recev != 0) {
		if (big_packet == 1) {
#ifndef WIN32
			close(tcp_so);
#else
			closesocket(tcp_so);
#endif
			clnt_destroy(tcp_cl);
					}
		*perr = *recev;
		return(DS_NOTOK);
			}

/* If the TCP connection was necessary, close it and destroy the associated
   socket */

	if (big_packet == 1)
	{
#ifndef WIN32
		close(tcp_so);
#else
		closesocket(tcp_so);
#endif
		clnt_destroy(tcp_cl);
	}

/* Leave function */

	return(DS_OK);

}



/**@ingroup dbaseAPIresource
 * Allows an user to remove resources from the database. The resource file
 * where the resource was initially defined is not updated. It is not possible to delete
 * resource(s) from the SEC domain with this function.
 *
 * @param dev_name	a string which defines the device name
 * @param res_name      an array of string defining the resource to be deleted
 * @param res_num       The number of resource to be deleted
 * @param perr 		The error code if any
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 *
 * @see db_delreslist
 */
int _DLLFunc db_delresource(const char *dev_name, char **res_name, u_int res_num,  DevLong *perr)
{
	int i,j,k,l;
	arr1 send;
	DevLong error;
	int *recev;
	char t_name[SIZE_B];
	register char *tmp;
	u_int diff;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters (non NULL pointer and two \
   characters in device name) */

	if (dev_name == NULL || res_name == NULL || res_num == 0)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	l=0;
	NB_CHAR(l,dev_name,'/');
	if (l != 2)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	for (i=0;i<(int)res_num;i++)
	{
		if (res_name[i] == NULL)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory for the array of pointeur to char */

	if((send.arr1_val = (DevString *)calloc(res_num,sizeof(DevString))) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

/* Build the full resource name (in lowercase letters) and initialize the array
  of pointer to resource name */

	k = strlen(dev_name);
	for (i=0;i<(int)res_num;i++)
	{
		l = strlen(res_name[i]);
		if ((send.arr1_val[i] = (DevString)malloc(k + l + 2)) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			for (j=0;j<i;j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			return(DS_NOTOK);
		}

		strcpy_tolower(send.arr1_val[i],dev_name);
		strcat(send.arr1_val[i], "/");
		strcat_tolower(send.arr1_val[i],res_name[i]);
	}

/* If the device domain name is "sec", refuse to do the work */

	t_name[0] = 0;
#ifdef OSK
	tmp = index(send.arr1_val[0],'/');
#else
	tmp = (char *)strchr(send.arr1_val[0],'/');
#endif /* OSK */
	diff = (u_int)(tmp - send.arr1_val[0]);
	strncpy(t_name,send.arr1_val[0],diff);
	t_name[diff] = 0;
	if (strcmp(t_name,"sec") == 0)
	{
		for (j = 0;j < (int)res_num;j++)
			free(send.arr1_val[j]);
		free(send.arr1_val);
		*perr = DbErr_DomainDefinition;
		return(DS_NOTOK);
	}
		

/* Initialize the structure sended to server */

	send.arr1_len = res_num;

/* Call server */

#ifdef ALONE
	recev = db_delres_1(&send,cl,&error);
#else
	recev = db_delres_1(&send,db_info.conf->clnt,&error);
#endif /* ALONE */


/* Any problems during database access ? */

	if (recev != NULL)
	{
		if (*recev == DbErr_DatabaseNotConnected)
		{

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

			for (i = 0;i < RETRY;i++)
			{

#ifdef _OSK
				tsleep(SLEEP_TIME);
#else
				tout.tv_sec = 0;
				tout.tv_usec = 20000;
				select(0,0,0,0,&tout);
#endif /* _OSK */
#ifdef ALONE
				recev = db_delres_1(&send,cl,&error);
#else
				recev = db_delres_1(&send,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(recev == NULL)
					break;
				if ((*recev == DS_OK) || (*recev != DbErr_DatabaseNotConnected))
						break;
			}

		}
	}

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new server. Don't forget to free memory */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&send,(void **)&recev,&cl,(int)DB_DELRES,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&send,(void **)&recev,
				     &db_info.conf->clnt,(int)DB_DELRES,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			for (j = 0;j < (int)res_num;j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* Return memory */

	for (j = 0;j < (int)res_num;j++)
		free(send.arr1_val[j]);
	free(send.arr1_val);

/* Any problem with database access ? */

	if (*recev != DS_OK)
	{
		*perr = *recev;
		return(DS_NOTOK);
	}

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPIresource
 * Delete a list of resources. The resource values are stored as a string in a 
 * database on a remote computer
 *
 * It is not the same call than the db_delresource because the resource name
 * are given here with the following syntax :
 *				
 *		domain/family/member/r_name				
 *
 * @param res_list 	an array of string defining the resource to be deleted
 * @param res_num  	The number of resource to be deleted
 * @param perr		The error code if any
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 *
 * @see db_delresource
 */
long _DLLFunc db_delreslist(char **res_list, long res_num, DevLong *perr)
{
	int i,j,k,l;
	arr1 send;
	DevLong error;
	int *recev;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters (non NULL pointer and two \
   characters in device name) */

	if ((res_list == NULL) || (res_num == 0))
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	for (i = 0;i < (int)res_num;i++)
	{
		l = 0;
		NB_CHAR(l,res_list[i],'/');
		if (l != 3)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory for the array of pointeur to char */

	if((send.arr1_val = (DevString *)calloc(res_num,sizeof(DevString))) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

/* Copy resource name (in lowercase letters) and initialize the array
  of pointer to resource name */

	for (i = 0;i< (int) res_num;i++)
	{
		k = strlen(res_list[i]);
		if ((send.arr1_val[i] = (DevString)malloc(k + 1)) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			for (j = 0;j < i;j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			return(DS_NOTOK);
		}

		strcpy_tolower(send.arr1_val[i],res_list[i]);
	}

/* Initialize the structure sended to server */

	send.arr1_len = res_num;

/* Call server */

#ifdef ALONE
	recev = db_delres_1(&send,cl,&error);
#else
	recev = db_delres_1(&send,db_info.conf->clnt,&error);
#endif /* ALONE */


/* Any problems during database access ? */

	if (recev != NULL)
	{
		if (*recev == DbErr_DatabaseNotConnected)
		{

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

			for (i = 0;i < RETRY;i++)
			{

#ifdef _OSK
				tsleep(SLEEP_TIME);
#else
				tout.tv_sec = 0;
				tout.tv_usec = 20000;
				select(0,0,0,0,&tout);
#endif /* _OSK */
#ifdef ALONE
				recev = db_delres_1(&send,cl,&error);
#else
				recev = db_delres_1(&send,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(recev == NULL)
					break;
				if ((*recev == DS_OK) || (*recev != DbErr_DatabaseNotConnected))
						break;
			}

		}
	}

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new server. Don't forget to free memory */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&send,(void **)&recev,&cl,(int)DB_DELRES,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&send,(void **)&recev,
				     &db_info.conf->clnt,(int)DB_DELRES,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			for (j = 0;j < (int)res_num;j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* Return memory */

	for (j = 0;j < (int)res_num;j++)
		free(send.arr1_val[j]);
	free(send.arr1_val);

/* Any problem with database access ? */

	if (*recev != DS_OK)
	{
		*perr = *recev;
		return(DS_NOTOK);
	}

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPIdevice
 * Return to the caller the device list for the device server with the
 * full device server name ds_full_name.
 *
 * @param name		The name of the device server
 * @param tab		the string's array, where the found devices are stored in
 * @param num_dev	The number of deices for this device server
 * @param perr		The error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_getdevlist(const char *name, char ***tab, u_int *num_dev, DevLong *perr)
{
	char *name1;
	db_res *recev;
	int i,j,l,k;
	DevLong error;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/*
 * if this is a device server without database then simply return
 * the list of devices passed on the command line
 */
 	if (config_flags && config_flags->no_database == True)
	{
		*num_dev = config_flags->device_no;
		*tab = (char **)config_flags->device_list;
		return(DS_OK);
	}

/* Try to verify the function parameters (non NULL pointers and one
   \ character in device server name) */

	if (name == NULL || tab == NULL)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}
	k = 0;
	NB_CHAR(k,name,'/');
	if (k != 1)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Make a copy of the device server name and all  the string's character 
   in lowercase */

	k = strlen(name);
	if ((name1 = (char *)malloc(k + 1)) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(name1,name);

/* Call server */

#ifdef ALONE
	recev = db_getdev_1(&name1,cl,&error);
#else
	recev = db_getdev_1(&name1,db_info.conf->clnt,&error);	
#endif /* ALONE */
	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&name1,(void **)&recev,&cl,(int)DB_GETDEV,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&name1,(void **)&recev,
				     &db_info.conf->clnt,(int)DB_GETDEV,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			free(name1);
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

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
#ifdef ALONE
			recev = db_getdev_1(&name1,cl,&error);
#else
			recev = db_getdev_1(&name1,db_info.conf->clnt,&error);
#endif /* ALONE */
			if(recev == NULL)
			{
				free(name1);
				*perr = error;
				return(DS_NOTOK);
			}
			if (recev->db_err == DS_OK || recev->db_err != DbErr_DatabaseNotConnected)
				break;
		}
	}

/* Return memory */

	free(name1);

/* Any problems during database access ? */

	if(recev->db_err != DS_OK)
	{
		*perr = recev->db_err;
		return(DS_NOTOK);
	}

/* Allocate memory to copy device name. I know that it i smore time consuming 
   than directly give the meory allocated by XDR to caller but how the caller
   will correctly free memory allocated by XDR */
   
   	if ((*tab = (char **)calloc(recev->res_val.arr1_len,sizeof(char *))) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (i = 0;i < recev->res_val.arr1_len;i++)
	{
		l = strlen(recev->res_val.arr1_val[i]);
		if (((*tab)[i] = (char *)malloc(l + 1)) == NULL)
		{
			for (j = 0;j < i;j++)
				free((*tab)[j]);
			free(*tab);
			*perr = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		strcpy((*tab)[i],recev->res_val.arr1_val[i]);
	}
	
/* Initialize devices name number */

	*num_dev = recev->res_val.arr1_len;

/* Free memory allocated by XDR */

#ifdef ALONE
	clnt_freeres(cl,(xdrproc_t)xdr_db_res,(char *)recev);
#else
	clnt_freeres(db_info.conf->clnt,(xdrproc_t)xdr_db_res,(char *)recev);
#endif /* ALONE */
	
/* No error */

	*perr = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPIdevice
 * Store into the database the network parameters for a device or a
 * group of devices. The network parameters are all the information needed by the
 * RPC to build a connection between a client and the device server in charge of a
 * device.
 * This means that after this function has been executed by a device server, a client 
 * is able to ask for 
 * @li host name
 * @li program number 
 * @li version number of the device server in charge of a device.

 * These information are stored in a database
 *
 * This function is now supported in version 1,2 and 3
 *
 * The version 2 also send the process ID to the database server
 * The version 3 also send the device server process name to the database. This is 
 * useful for multi-classes device server.
 *
 * @param devexp	an array of db_devinf structures defining the
 *               	devices to be exported
 * @param dev_num	The number of resource to be exported
 * @param perr		The error code if any
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 *
 * @see db_dev_import
 */
int _DLLFunc db_dev_export(Db_devinf devexp, u_int dev_num, DevLong *perr)
{
	int i,j,k,l;
	tab_dbdev_3 send3;
	tab_dbdev_2 send2;
	int *pdb_err;
	register db_devinfo_3 *devexp3;
	register db_devinfo_2 *devexp2;
	DevLong error;
	int pid;
	long vers;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

#ifndef ALONE
	vers = db_info.conf->vers_number;
#else
	vers = DB_VERS_3;
#endif 

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the functions parameters (Non NULL pointer and two
   \ character in the device name) */

	if (devexp == NULL || dev_num == 0)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

	for (i=0;i<(int)dev_num;i++)
	{
		if (vers == DB_VERS_2)
		{
			if ((devexp[i].host_name == NULL) || 
		    	    (devexp[i].device_name == NULL) || 
		            (devexp[i].device_type == NULL) || 
		            (devexp[i].device_class == NULL))
			{
				*perr = DbErr_BadParameters;
				return(DS_NOTOK);
			}
		}
		else
		{
			if ((devexp[i].host_name == NULL) || 
		    	    (devexp[i].device_name == NULL) || 
		            (devexp[i].device_type == NULL) || 
		            (devexp[i].device_class == NULL) ||
		            (devexp[i].proc_name == NULL))
			{
				*perr = DbErr_BadParameters;
				return(DS_NOTOK);
			}
		}
		l = 0;
		NB_CHAR(l,devexp[i].device_name,'/');
		if (l != 2)
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory to copy the db_devimp structures array */

	if (vers == DB_VERS_2)
	{
		if ((devexp2 = (db_devinfo_2 *)calloc(dev_num,sizeof(db_devinfo_2))) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
	}
	else
	{
		if ((devexp3 = (db_devinfo_3 *)calloc(dev_num,sizeof(db_devinfo_3))) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
	}

/* Host name and device name in lowercase letters. To do that, we are obliged
   to make a copy of host_name and device_name to be abble to change them
   (ssm in OS-9 system). Copy also the device type and device class strings. */

	if (vers == DB_VERS_2)
	{
		for (i=0;i<(int)dev_num;i++)
		{
			k = strlen(devexp[i].host_name);
			if ((devexp2[i].host_name = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp2[i].host_name);
					free(devexp2[i].dev_name);
					free(devexp2[i].dev_type);
					free(devexp2[i].dev_class);
				}
				free(devexp2);
				return(DS_NOTOK);
			}
			strcpy_tolower(devexp2[i].host_name,devexp[i].host_name);

			k = strlen(devexp[i].device_name);
			if ((devexp2[i].dev_name = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp2[i].host_name);
					free(devexp2[i].dev_name);
					free(devexp2[i].dev_type);
					free(devexp2[i].dev_class);
				}
				free(devexp2[i].host_name);
				free(devexp2);
				return(DS_NOTOK);
			}
			strcpy_tolower(devexp2[i].dev_name,devexp[i].device_name);

			k = strlen(devexp[i].device_type);
			if ((devexp2[i].dev_type = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp2[i].host_name);
					free(devexp2[i].dev_name);
					free(devexp2[i].dev_type);
					free(devexp2[i].dev_class);
				}
				free(devexp2[i].host_name);
				free(devexp2[i].dev_name);
				free(devexp2);
				return(DS_NOTOK);
			}
			strcpy(devexp2[i].dev_type,devexp[i].device_type);

			k = strlen(devexp[i].device_class);
			if ((devexp2[i].dev_class = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp2[i].host_name);
					free(devexp2[i].dev_name);
					free(devexp2[i].dev_type);
					free(devexp2[i].dev_class);
				}
				free(devexp2[i].host_name);
				free(devexp2[i].dev_name);
				free(devexp2[i].dev_type);
				free(devexp2);
				return(DS_NOTOK);
			}
			strcpy(devexp2[i].dev_class,devexp[i].device_class);

			devexp2[i].p_num = devexp[i].pn;
			devexp2[i].v_num = devexp[i].vn;

/* Get process ID (specific for version 2) */

			if (i == 0)
#if defined(WIN32)
				pid = _getpid();
#else
#if !defined (vxworks)
				pid = getpid();
#else  /* !vxworks */
				pid = taskIdSelf();
#endif /* !vxworks */
#endif
			devexp2[i].pid = pid;
		}
	}
	else
	{
		for (i=0;i<(int)dev_num;i++)
		{
			k = strlen(devexp[i].host_name);
			k = k >= HOST_NAME_LENGTH ? HOST_NAME_LENGTH - 1 : k;
			if ((devexp3[i].host_name = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp3[i].host_name);
					free(devexp3[i].dev_name);
					free(devexp3[i].dev_type);
					free(devexp3[i].dev_class);
					free(devexp3[i].proc_name);
				}
				free(devexp3);
				return(DS_NOTOK);
			}
			strncpy_tolower(devexp3[i].host_name, devexp[i].host_name, k);
			devexp3[i].host_name[k] = '\0';

			k = strlen(devexp[i].device_name);
			k = k >= DEV_NAME_LENGTH ? DEV_NAME_LENGTH - 1 : k;
			if ((devexp3[i].dev_name = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp3[i].host_name);
					free(devexp3[i].dev_name);
					free(devexp3[i].dev_type);
					free(devexp3[i].dev_class);
					free(devexp3[i].proc_name);
				}
				free(devexp3[i].host_name);
				free(devexp3);
				return(DS_NOTOK);
			}
			strncpy_tolower(devexp3[i].dev_name, devexp[i].device_name, k);
			devexp3[i].dev_name[k] = '\0';

			k = strlen(devexp[i].device_type);
			k = k >= DEV_TYPE_LENGTH ? DEV_TYPE_LENGTH - 1 : k;
			if ((devexp3[i].dev_type = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp3[i].host_name);
					free(devexp3[i].dev_name);
					free(devexp3[i].dev_type);
					free(devexp3[i].dev_class);
					free(devexp3[i].proc_name);
				}
				free(devexp3[i].host_name);
				free(devexp3[i].dev_name);
				free(devexp3);
				return(DS_NOTOK);
			}
			strncpy(devexp3[i].dev_type,devexp[i].device_type, k);
			devexp3[i].dev_type[k] = '\0';

			k = strlen(devexp[i].device_class);
			k = k >= DEV_CLASS_LENGTH ? DEV_CLASS_LENGTH - 1 : k;
			if ((devexp3[i].dev_class = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp3[i].host_name);
					free(devexp3[i].dev_name);
					free(devexp3[i].dev_type);
					free(devexp3[i].dev_class);
					free(devexp3[i].proc_name);
				}
				free(devexp3[i].host_name);
				free(devexp3[i].dev_name);
				free(devexp3[i].dev_type);
				free(devexp3);
				return(DS_NOTOK);
			}
			strncpy(devexp3[i].dev_class,devexp[i].device_class, k);
			devexp3[i].dev_class[k] = '\0';

			devexp3[i].p_num = devexp[i].pn;
			devexp3[i].v_num = devexp[i].vn;

/* Get process ID (specific for version 2) */

			if (i == 0)
#if defined(WIN32)
				pid = _getpid();
#else  /* WIN32 */
#if !defined (vxworks)
				pid = getpid();
#else  /* !vxworks */
				pid = taskIdSelf();
#endif /* !vxworks */
#endif /* WIN32 */
			devexp3[i].pid = pid;

/* Copy process name (specific for version 3) */

			k = strlen(devexp[i].proc_name);
			k = k >= PROC_NAME_LENGTH ? PROC_NAME_LENGTH - 1 : k;
			if ((devexp3[i].proc_name = (char *)malloc(k + 1)) == NULL)
			{
				*perr = DbErr_ClientMemoryAllocation;
				for (l=0;l<i;l++)
				{
					free(devexp3[i].host_name);
					free(devexp3[i].dev_name);
					free(devexp3[i].dev_type);
					free(devexp3[i].dev_class);
					free(devexp3[i].proc_name);
				}
				free(devexp3[i].host_name);
				free(devexp3[i].dev_name);
				free(devexp3[i].dev_type);
				free(devexp3[i].dev_class);
				free(devexp3);
				return(DS_NOTOK);
			}
			strncpy_tolower(devexp3[i].proc_name, devexp[i].proc_name, k);
			devexp3[i].proc_name[k] = '\0';
		}

	}

/* Initialize the structure sended to the server */

	if (vers == DB_VERS_2)
	{
		send2.tab_dbdev_len = dev_num;
		send2.tab_dbdev_val = devexp2;
	}
	else
	{
		send3.tab_dbdev_len = dev_num;
		send3.tab_dbdev_val = devexp3;
	}

/* Call server */

#ifdef ALONE
	pdb_err = db_devexp_3(&send3,cl,&error);
#else
	if (vers == DB_VERS_2)
		pdb_err = db_devexp_2(&send2,db_info.conf->clnt,&error);
	else
		pdb_err = db_devexp_3(&send3,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (pdb_err != NULL)
	{
		if (*pdb_err == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				pdb_err = db_devexp_3(&send3,cl,&error);
#else
				if (vers == DB_VERS_2)
					pdb_err = db_devexp_2(&send2,db_info.conf->clnt,&error);
				else
					pdb_err = db_devexp_3(&send3,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(pdb_err == NULL)
					break;
				if ((*pdb_err == DS_OK) || (*pdb_err != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. Don't forget to free memory in case of error. */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&send3,(void **)&pdb_err,&cl,(int)DB_DEVEXP_3,i_nethost,DB_UDP,&error);
#else
			if (vers == DB_VERS_2)
				to_reconnection((void *)&send2,
						(void **)&pdb_err,
					  	&db_info.conf->clnt,
						(int)DB_DEVEXP_2,
						i_nethost,DB_UDP,&error);
			else
				to_reconnection((void *)&send3,
						(void **)&pdb_err,
					  	&db_info.conf->clnt,
						(int)DB_DEVEXP_3,
						i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			if (vers == DB_VERS_2)
			{
				for (l=0;l<(int)dev_num;l++)
				{
					free(devexp2[l].host_name);
					free(devexp2[l].dev_name);
					free(devexp2[l].dev_type);
					free(devexp2[l].dev_class);
				}
				free(devexp2);
			}
			else
			{
				for (l=0;l<(int)dev_num;l++)
				{
					free(devexp3[l].host_name);
					free(devexp3[l].dev_name);
					free(devexp3[l].dev_type);
					free(devexp3[l].dev_class);
					free(devexp3[l].proc_name);
				}
				free(devexp3);
			}
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* Return memory */

	if (vers == DB_VERS_2)
	{
		for (l=0;l<(int)dev_num;l++)
		{
			free(devexp2[l].host_name);
			free(devexp2[l].dev_name);
			free(devexp2[l].dev_type);
			free(devexp2[l].dev_class);
		}
		free(devexp2);
	}
	else
	{
		for (l=0;l<(int)dev_num;l++)
		{
			free(devexp3[l].host_name);
			free(devexp3[l].dev_name);
			free(devexp3[l].dev_type);
			free(devexp3[l].dev_class);
			free(devexp3[l].proc_name);
		}
		free(devexp3);
	}

/* Any trouble during database access ? */

	if (*pdb_err != DS_OK)
	{
		*perr = *pdb_err;
		return(DS_NOTOK);
	}

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}
 


/**@ingroup dbaseAPIdevice
 * Return all the necessary parameters to buld RPC connection beween a client 
 * and the device server in charge of a device. 
 * This means to ask for the host name, the program number and the version number of the device
 * server which drives a device.
 * 
 * It allows to retrieve these RPC's information for several devices at the same time.
 *
 * @param name	 	an array of pointers to strings. Each string
 *               	contains the name of a device to be imported.
 * @param num_dev	The number of devices to be imported
 * @param tab		an array of db_devinf_imp structures. Each
 *               	structure defines a device
 * @param perr		The error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 *
 * @see db_dev_export
 */
int _DLLFunc db_dev_import(char **name,Db_devinf_imp *tab, u_int num_dev, DevLong *perr)
{
	int i,j,k,l;
	arr1 send;
	register Db_devinf_imp tmp_ptr;
	db_devinfo *tmp_ptr1;
	db_resimp *recev;
	DevLong error;
	long i_nethost;
	char **name_copy;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters (non NULL pointer and two
   \ character in the device name to be imported) */

	if (name == NULL || tab == NULL || num_dev == 0) 
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/*
 * find out which nethost has been requested for the first device
 * It is not possible to db_import several devices from different
 * nethosts in the same call. The nethost chosen is the nethost associated
 * with the first device in the device name array
 */
	if ((i_nethost = get_i_nethost_by_device_name(name[0],perr)) < 0)
		return(DS_NOTOK);

	name_copy = (char**)malloc(sizeof(char*)*num_dev);

	for (i=0;i<(int)num_dev;i++) 
	{
/*
 * if nethost is specified in the device name "remove" it
 *
 *
 * BUG - this code removes the nethost and overwrites the device name 
 *       thereby losing the nethost information for good. after this 
 *	 everything can go wrong from segmentation violation
 *	 to reimport not working to who knows what ... !
 *
 *
 *		name[i] = extract_device_name(name[i],perr);
 *
 * BUG FIX - make copy of device name for local use
 *
 * ag + wdk (7jul99)
 */
		name_copy[i] = extract_device_name(name[i],perr);

		l = 0;
		NB_CHAR(l,name_copy[i],'/');
		if (l != 2) 
		{
			*perr = DbErr_BadParameters;
			return(DS_NOTOK);
		}
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first) 
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL) 
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory to build the array of pointers to strings and all device
   names in lowercase letters */

	if ((send.arr1_val = (DevString *)calloc(num_dev,sizeof(DevString))) == NULL) 
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (k=0;k<(int)num_dev;k++) 
	{
		l = strlen(name_copy[k]);
		if ((send.arr1_val[k] = (DevString)malloc(l + 1)) == NULL) 
		{
			*perr = DbErr_ClientMemoryAllocation;
			for(i=0;i<k;i++)
				free(send.arr1_val[i]);
			free(send.arr1_val);
			free(name_copy);
			return(DS_NOTOK);
		}
		strcpy_tolower(send.arr1_val[k],name_copy[k]);
	}

/* Allocate memory for the array of structures sent back to the client */

	if ((*tab = (Db_devinf_imp)calloc(num_dev,sizeof(db_devinf_imp))) == NULL)
	{
		for (i=0;i<(int)num_dev;i++)
			free(send.arr1_val[i]);
		free(send.arr1_val);
		free(name_copy);
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

/* Structure initialization (structure sent to server) */

	send.arr1_len = num_dev;

/* Call server */

#ifdef ALONE
	recev = db_devimp_1(&send,cl,&error);
#else
	recev = db_devimp_1(&send, multi_nethost[i_nethost].db_info->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (recev != NULL)
	{
		if (recev->db_imperr == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				recev = db_devimp_1(&send,cl,&error);
#else
				recev = db_devimp_1(&send,multi_nethost[i_nethost].db_info->clnt, &error);
#endif /* ALONE */
				if(recev == NULL)
					break;
				if ((recev->db_imperr == DS_OK) || (recev->db_imperr != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes, don't forget to free memory */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&send,(void **)&recev,&cl,(int)DB_DEVIMP,0,DB_UDP,&error);
#else
			to_reconnection((void *)&send,(void **)&recev,
					&multi_nethost[i_nethost].db_info->clnt,
				        (int)DB_DEVIMP,i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			*perr = error;
			free(*tab);
			*tab = NULL;
			for (i=0;i<(int)num_dev;i++)
				free(send.arr1_val[i]);
			free(send.arr1_val);
			free(name_copy);
			return(DS_NOTOK);
		}
	}

/* Return memory */

	for (i=0;i<(int)num_dev;i++)
		free(send.arr1_val[i]);
	free(send.arr1_val);
	free(name_copy);

/* Any problem with database access ? */	

	if (recev->db_imperr != DS_OK)
	{
		*perr = recev->db_imperr;
		free(*tab);
		*tab = NULL;
		return(DS_NOTOK);
	}

/* No error,so copy the result into the caller structures */

	for (i=0;i<(int)num_dev;i++)
	{
		tmp_ptr = &((*tab)[i]);	
		tmp_ptr1 = &(recev->imp_dev.tab_dbdev_val[i]);
		snprintf(tmp_ptr->device_name, sizeof(tmp_ptr->device_name), "%s", tmp_ptr1->dev_name);
		snprintf(tmp_ptr->host_name, sizeof(tmp_ptr->host_name), "%s", tmp_ptr1->host_name);
		snprintf(tmp_ptr->device_type, sizeof(tmp_ptr->device_type), "%s", tmp_ptr1->dev_type);
		snprintf(tmp_ptr->device_class, sizeof(tmp_ptr->device_class), "%s", tmp_ptr1->dev_class);
		tmp_ptr->pn = tmp_ptr1->p_num;
		tmp_ptr->vn = tmp_ptr1->v_num;
	}
		
/* Free the memory allocated by XDR */

#ifdef ALONE
	clnt_freeres(cl,xdr_db_resimp,(char *)recev);
#else
	clnt_freeres(multi_nethost[i_nethost].db_info->clnt, (xdrproc_t)xdr_db_resimp,(char *)recev);
#endif /* ALONE */

/* Leave function */

	*perr = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPIserver
 * This unregisters (mark device(s) as not exported) for all the device(s) served by the 
 * device server ds_name started with the personal name pers_name.
 * (to deinitialize the host name, program number and version number for
 * all the devices driven by a device server)
 *
 * @param ds_netnam 	a string which contains the device server network name.
 * @param perr 		The error caode in case of trouble
 *
 * @retval DS_NOTOK  In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_svc_unreg(const char *ds_netnam,  DevLong *perr)
{
	int i,j,k;
	int *pdb_err;
	char *ds_netnam1;
	DevLong error;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters  (non NULL pointer and one
   \ character in device server name) */

	if (ds_netnam == NULL)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}
	j = 0;
	NB_CHAR(j,ds_netnam,'/');
	if (j != 1)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

# ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Make a copy of device server network name and Device server network name 
   in lowercase letters */

	k = strlen(ds_netnam);
	if ((ds_netnam1 = (char *)malloc(k + 1)) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(ds_netnam1,ds_netnam);

/* Call server */

#ifdef ALONE
	pdb_err = db_svcunr_1(&ds_netnam1,cl,&error);
#else
	pdb_err = db_svcunr_1(&ds_netnam1,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (pdb_err != NULL)
	{
		if (*pdb_err == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				pdb_err = db_svcunr_1(&ds_netnam1,cl,&error);
#else
				pdb_err = db_svcunr_1(&ds_netnam1,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(pdb_err == NULL)
					break;
				if ((*pdb_err == DS_OK) || (*pdb_err != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it a time-out, try to reconnect to
   a new database server. Don't fotget to free memory in case of error. */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&ds_netnam1,(void **)&pdb_err,&cl,(int)DB_SVCUNR,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&ds_netnam1,(void **)&pdb_err,
				     &db_info.conf->clnt,(int)DB_SVCUNR,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			*perr = error;
			return(DS_NOTOK);
		}
	}	

/* Return memory */

	free(ds_netnam1);

/* Any problem during database access ? */

	if (*pdb_err != DS_OK)
	{
		*perr = *pdb_err;
		return(DS_NOTOK);
	}

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPIserver
 * Return host name, program number and version number of the first 
 * device found in the database for the device server with the full name ds_full_name.
 *
 * @param ds_netname	a string which contains the device server
 *               	network name.
 * @param pp_num	The program number
 * @param pv_num	The version number
 * @param pho_name	The host_name
 * @param perr		The error code in case of trouble
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 *
 */
int _DLLFunc db_svc_check(const char *ds_netname, char **pho_name, u_int *pp_num, u_int *pv_num, DevLong *perr)
{
	int i,j,k;
	svc_inf *back;
	char *ds_netname1;
	DevLong error;
	long i_nethost = 0;
#ifndef OSK
	struct timeval tout;
#endif /* OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters (non NULL pointers and one
   / character in device server name) */

	if (ds_netname == NULL || pho_name == NULL || pp_num == NULL || pv_num == NULL)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}
	k = 0;
	NB_CHAR(k,ds_netname,'/');
	if (k != 1)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory for a copy of the device server network name and device 
   server network name in lowercase letters */

	k = strlen(ds_netname);
	if ((ds_netname1 = (char *)malloc(k + 1)) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(ds_netname1,ds_netname);
/* Call server */

#ifdef ALONE
	back = db_svcchk_1(&ds_netname1,cl,&error);
#else
	back = db_svcchk_1(&ds_netname1,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (back != NULL)
	{
		if (back->db_err == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				back = db_svcchk_1(&ds_netname1,cl,&error);
#else
				back = db_svcchk_1(&ds_netname1,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(back == NULL)
					break;
				if ((back->db_err == DS_OK) || (back->db_err != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. Don't forget to free memory in case of error */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&ds_netname1,(void **)&back,&cl,(int)DB_SVCCHK,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&ds_netname1,(void **)&back,
				     &db_info.conf->clnt,(int)DB_SVCCHK,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			free(ds_netname1);
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* Return memory */

	free(ds_netname1);

/* Any problem during database access ? */

	if (back->db_err != DS_OK)
	{
#ifdef ALONE
		clnt_freeres(cl,(xdrproc_t)xdr_svc_inf,(char *)back);
#else	
		clnt_freeres(db_info.conf->clnt,(xdrproc_t)xdr_svc_inf,(char *)back);
#endif
		*perr = back->db_err;
		return(DS_NOTOK);
	}

/* Allocate memory to store host name and copy it */

	if ((*pho_name = (char *)malloc(strlen(back->ho_name) + 1)) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(*pho_name,back->ho_name);
	
/* No error */

	*pp_num = back->p_num;
	*pv_num = back->v_num;
#ifdef ALONE
	clnt_freeres(cl,(xdrproc_t)xdr_svc_inf,(char *)back);
#else	
	clnt_freeres(db_info.conf->clnt,(xdrproc_t)xdr_svc_inf,(char *)back);
#endif
	*perr = DS_OK;
	return(DS_OK);

}
 


#ifndef _OSK
/**@ingroup dbaseAPIexport
 * Allows an user to get the name of exported (and then ready to accept
 * command) devices. With the filter parameter, it is possible to limit the devices
 * name return by the function. This function is not available for OS-9 client.
 *
 * @param filter	A filter to select the exported device names.
 * @param tab		the string's array containing the found exported devices
 * @param num_dev	The number of exported devices
 * @param perr		The error code in case of problems
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 * 			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_getdevexp(const char *filter, char ***tab, u_int *num_dev, DevLong *perr)
{
	char *filter1;
	register db_res *recev;
	int i,j,k,tp;
	CLIENT *tcp_cl;
	int tcp_so;
	int tcp_used = 0;
	DevLong error;
	struct timeval tout;
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters (non NULL pointers) and no more 
   than 2 / characters in filter name */

	if (num_dev == NULL || tab == NULL)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}
	
/* If the filter string is NULL, replace it with * chracters */

	if (filter == NULL)
		filter = (char *)"*/*/*";

	i = 0;
	NB_CHAR(i,filter,'/');
	if (i > 2)
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}


/* Verify that only one wild-card is used for every filter fields */

	if (test_star(filter))
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Verify that the first and the last characters are not /. */

	k = strlen(filter);
	if (filter[0] == '/' || filter[k - 1] == '/')
	{
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Allocate memory for the db_res structures array if it's the first call
   to this function */

	if (!first_devexp)
	{
		if ((tab_clstu = (devexp_res *)calloc(ST_ALLOC,sizeof(devexp_res))) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		first_devexp++;
	}

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#else
/*
 * find out which nethost has been requested for this device
 */
	if (!config_flags)
	{
		if (db_import(perr) != DS_OK)
			return(DS_NOTOK);
	}
#endif /* ALONE */


/* Make a copy of the filter name and all  the string's character 
   in lowercase */

	if ((filter1 = (char *)malloc(k + 1)) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy_tolower(filter1,filter);

/* Realloc memory if the array tab_clstu is full */

	if (func_ptr != 0 && (func_ptr & 0x7) == 0)
	{
		tp = ((func_ptr >> 3) + 1) << 3;
		if ((tab_clstu = (devexp_res *)realloc(tab_clstu,sizeof(devexp_res) * tp)) == NULL)
		{
			*perr = DbErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		memset((void *)(&tab_clstu[func_ptr]),0,sizeof(devexp_res) << 3);
	}

/* Call server */

#ifdef ALONE
	recev = db_getdevexp_1(&filter1,cl,&error);
#else
	recev = db_getdevexp_1(&filter1,db_info.conf->clnt,&error);	
#endif /* ALONE */
	
/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (recev != NULL)
	{
		if (recev->db_err == DbErr_DatabaseNotConnected)
		{
			for (i = 0;i < RETRY;i++)
			{
				tout.tv_sec = 0;
				tout.tv_usec = 20000;
				select(0,0,0,0,&tout);
#ifdef ALONE
				recev = db_getdevexp_1(&filter1,cl,&error);
#else
				recev = db_getdevexp_1(&filter1,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(recev == NULL)
					break;
				if ((recev->db_err == DS_OK) || (recev->db_err != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? */

	if(recev == NULL)
	{
		*perr = error;
		return(DS_NOTOK);
	}

/* Any problems during database access ? */

	if(recev->db_err != DS_OK)
	{
		if (recev->db_err != DbErr_MaxDeviceForUDP)
		{
			*perr = recev->db_err;
			return(DS_NOTOK);
		}

/* If the server answers that there is too many info to send for the UDP
   protocol, create a TCP connection to the server and redo the call.
   To be able to correctly close the TCP connection, we must know the 
   socket number to close it (the RPC function does not do this). So, instead
   of the clnt_create function, we used the clnttcp_create function. */

		else
		{

			if (!first_tcp_call)
			{
#ifdef ALONE
				if ((ht = gethostbyname(serv_name)) == NULL)
				{
#else
#ifndef vxworks
				if ((ht = gethostbyname(db_info.conf->server_host)) == NULL)
				{
#else  /* !vxworks */
				if ((host_addr = hostGetByName(db_info.conf->server_host)) == 0) {
#endif /* !vxworks */
#endif /* ALONE */
					*perr = DbErr_CannotCreateClientHandle;
					return(DS_NOTOK);
				}

				serv_adr.sin_family = AF_INET;
#ifndef vxworks
				memcpy((void *)(&serv_adr.sin_addr),ht->h_addr,(size_t)ht->h_length);
#else  /* !vxworks */
				memcpy((void *)(&serv_adr.sin_addr),(char*)&host_addr,4);
#endif /* !vxworks */
				first_tcp_call = 1;
			}

			serv_adr.sin_port = 0;
			tcp_so = RPC_ANYSOCK;
#ifdef ALONE
			tcp_cl = clnttcp_create(&serv_adr,DB_SETUPPROG,
						DB_VERS_3, &tcp_so,0,0);
#else
			tcp_cl = clnttcp_create(&serv_adr,
					        db_info.conf->prog_number,
					        DB_VERS_3,&tcp_so,0,0);
#endif /* ALONE */
			if (tcp_cl == NULL)
			{
				*perr = DbErr_CannotCreateClientHandle;
				return(DS_NOTOK);
			}

/* Redo the call */

			recev = db_getdevexp_1(&filter1,tcp_cl,&error);

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

			if (recev != NULL)
			{
				if (recev->db_err == DbErr_DatabaseNotConnected)
				{
					for (i = 0;i < RETRY;i++)
					{
						tout.tv_sec = 0;
						tout.tv_usec = 20000;
						select(0,0,0,0,&tout);

						recev = db_getdevexp_1(&filter1,tcp_cl,&error);

						if(recev == NULL)
							break;
						if ((recev->db_err == DS_OK) || (recev->db_err != DbErr_DatabaseNotConnected))
							break;
					}
				}
			}

/* Any problem with server ? */

			if(recev == NULL)
			{
				*perr = error;
#ifndef WIN32
				close(tcp_so);
#else
				closesocket(tcp_so);
#endif
				clnt_destroy(tcp_cl);
				return(DS_NOTOK);
			}

/* Any problems during database access ? */

			if(recev->db_err != DS_OK)
			{
				*perr = recev->db_err;
#ifndef WIN32
				close(tcp_so);
#else
				closesocket(tcp_so);
#endif
				clnt_destroy(tcp_cl);
				return(DS_NOTOK);
			}

			tcp_used = 1;

		}
	}
	
/* Try to get TANGO exported device(s) list */

#ifdef __STDCPP__
	long ret_val;
	vector<string> tg_dev_name;
	long full_dev_nb;
	 
	ret_val = get_tango_exp_devices(filter1,tg_dev_name,perr);

/* Copy device name in classical C char arrays */
		
	switch (ret_val)
	{
	case 1 :
		full_dev_nb = recev->res_val.arr1_len; 
		break;
		
	case 0 :
		full_dev_nb = tg_dev_name.size() + recev->res_val.arr1_len;
		break;
	}
	
/* Merge the two arrays (taco devices and tango devices) */
	
	if ((*tab = new char *[full_dev_nb]) == NULL)
	{
		*perr = DbErr_ClientMemoryAllocation;
		return DS_NOTOK;
	}
	
	for (i = 0;i < full_dev_nb;i++)
	{
		if (i < recev->res_val.arr1_len)
		{
			if (((*tab)[i] = new char[strlen(recev->res_val.arr1_val[i]) + 1]) == NULL)
			{
				for (j = 0;j < i;j++)
					delete (*tab)[j];
				delete *tab;
				*perr = DbErr_ClientMemoryAllocation;
				return DS_NOTOK;
			}
			strcpy((*tab)[i],recev->res_val.arr1_val[i]);
		}
		else
		{
			j = i - recev->res_val.arr1_len;
			if (((*tab)[i] = new char[tg_dev_name[j].size() + 1]) == NULL)
			{
				for (k = 0;k < i;k++)
					delete (*tab)[k];
				delete *tab;
				*perr = DbErr_ClientMemoryAllocation;
				return DS_NOTOK;
			}
			strcpy((*tab)[i],tg_dev_name[j].c_str());
		}
	}

/* Return memory */

	free(filter1);
	if (tcp_used == 1)
		clnt_freeres(tcp_cl,(xdrproc_t)xdr_db_res,(char *)&(recev->res_val));
	else
		clnt_freeres(db_info.conf->clnt,(xdrproc_t)xdr_db_res,(char *)&(recev->res_val));
	
				
/* Sort the returned string array */

	kern_sort(*tab,full_dev_nb);

/* Initialize devices name number */

	*num_dev = full_dev_nb;
	
/* Copy the information needed to free memory in the db_freedevexp function
   in the tab_clstu array */

	if (tcp_used == 1)
	{
		tab_clstu[func_ptr].tcp = 1;
		tab_clstu[func_ptr].tcp_so = tcp_so;
		tab_clstu[func_ptr].cl = tcp_cl;
		tab_clstu[func_ptr].res.res_val.arr1_len = full_dev_nb;
		tab_clstu[func_ptr].res.res_val.arr1_val = *tab;
	}
	else
	{
#ifdef ALONE
		tab_clstu[func_ptr].cl = cl;
#else
		tab_clstu[func_ptr].cl = db_info.conf->clnt;
#endif /* ALONE */
		tab_clstu[func_ptr].tcp = 0;
		tab_clstu[func_ptr].res.res_val.arr1_len = full_dev_nb;
		tab_clstu[func_ptr].res.res_val.arr1_val = *tab;
	}
	func_ptr++;

/* No error */

	*perr = DS_OK;
	return(DS_OK);
	
#else /* __STDCPP__ */

/* Return memory */

	free(filter1);
	
/* Sort the returned string array */

	kern_sort(recev->res_val.arr1_val,recev->res_val.arr1_len);

/* Initialize devices name number */

	*num_dev = recev->res_val.arr1_len;

/* Initialize the pointer to the array of string (memory alloc. by XDR) */

	*tab = recev->res_val.arr1_val;
	
/* Copy the information needed to free memory in the db_freedevexp function
   in the tab_clstu array */

	if (tcp_used == 1)
	{
		tab_clstu[func_ptr].tcp = 1;
		tab_clstu[func_ptr].tcp_so = tcp_so;
		tab_clstu[func_ptr].cl = tcp_cl;
		tab_clstu[func_ptr].res.res_val.arr1_len = recev->res_val.arr1_len;
		tab_clstu[func_ptr].res.res_val.arr1_val = *tab;	
	}
	else
	{
#ifdef ALONE
		tab_clstu[func_ptr].cl = cl;
#else
		tab_clstu[func_ptr].cl = db_info.conf->clnt;
#endif /* ALONE */
		tab_clstu[func_ptr].tcp = 0;
		tab_clstu[func_ptr].res.res_val.arr1_len = recev->res_val.arr1_len;
		tab_clstu[func_ptr].res.res_val.arr1_val = *tab;
	}
	func_ptr++;

/* No error */

	*perr = DS_OK;
	return(DS_OK);
	
#endif /* __STDCPP__ */

}




/**@ingroup dbaseAPIintern
 * To sort an array of strings. The sort is done with the
 * strcmp function. The algorithm come from the famous
 * Kernighan and Ritchie book (chapter 5)
 *
 * @param tab 	the array of strings pointers
 * @param n	The number of elements in this array
 */
void kern_sort(char **tab,int n)
{
	int 	gap,
		i,
		j;
	char 	*temp;

	for (gap = n/2;gap > 0;gap /= 2)
		for (i = gap;i < n;i++)
			for (j = i - gap;j >=0; j -= gap)
				if (strcmp(tab[j],tab[j + gap]) <= 0) 
					break;
				else
				{
					temp = tab[j];
					tab[j] = tab[j + gap];
					tab[j + gap] = temp;
				}
}
			
/**@ingroup dbaseAPIintern
 * Test the number of wild card ('*') in every filter
 * fields. The maximun number of '*' for each field is 1
 *
 * @param filter the filter string				
 *
 * @retval DS_OK if the '*' is correctly used. 
 * @retval DS_NOTOK Otherwise
 */
int test_star(const char *filter)
{
	int i,j;
	const char *tmp;
	char *temp;
	u_int diff;
	char filter_domain[SIZE_A];
	char filter_family[SIZE_A];
	char filter_member[SIZE_A];

/* 
 * Extract from filter string each part of the filter (domain, family and 
 * member). If two / characters can be retrieved in the filter string, this
 * means that the domain, family and member part of the filter are initialized.
 * If only one / can be retrieved, only the domain and family part are
 * initialized and if there is no / in the filter string, just the domain
 * is initialized. 
 */

	i = 0;
	tmp = filter;
	NB_CHAR(i,tmp,'/');

	switch(i)
	{
		case 2 : 
			tmp = strchr(filter,'/');
			diff = (u_int)(tmp++ - filter);
			strncpy(filter_domain,filter,diff);
			filter_domain[diff] = 0;

			temp = strchr(tmp,'/');
			diff = (u_int)(temp++ - tmp);
			strncpy(filter_family,tmp,diff);
			filter_family[diff] = 0;

			strcpy(filter_member,temp);

/* 
 * For each fields, count the number of wild card used 
 */
			j = 0;
			NB_CHAR(j,filter_domain,'*');
			if (j > 1)
				return(DS_NOTOK);

			j = 0;
			NB_CHAR(j,filter_family,'*');
			if (j > 1)
				return(DS_NOTOK);

			j = 0;
			 NB_CHAR(j,filter_member,'*');
			if (j > 1)
				return(DS_NOTOK);
			break;
		case 1 : 
			tmp = strchr(filter,'/');
			diff = (u_int)(tmp++ - filter);
			strncpy(filter_domain,filter,diff);
			filter_domain[diff] = 0;

			strcpy(filter_family,tmp);
/* 
 * For each fields, count the number of wild card used 
 */
			j = 0;
			NB_CHAR(j,filter_domain,'*')
			if (j > 1)
				return(DS_NOTOK);

			j = 0;
			NB_CHAR(j,filter_family,'*');
			if (j > 1)
				return(DS_NOTOK);
			break;
		case 0 : 
			strcpy(filter_domain,filter);
/* 
 * For each fields, count the number of wild card used 
 */
			j = 0;
			NB_CHAR(j,filter_domain,'*')
			if (j > 1)
				return(DS_NOTOK);
			break;
	}
	return(DS_OK);
}



/**@ingroup dbaseAPIexport
 * The @ref db_getdevexp function can return a lot of device names and allocates memory
 * to store them. This call is a local call and frees all the memory allocated by the
 * db_getdevexp function.
 *
 * @param ptr The pointer to the array of exported device name strings.
 *   This pointer must have been initialized by a db_getdevexp function.
 *
 * @retval DS_OK if no errors occurs 
 * @retval DS_NOTOK if a problem occurs
 * 
 * @see db_getdevexp
 */
int _DLLFunc db_freedevexp(char **ptr)
{
	register int i;
	int l;

        if (config_flags && config_flags->no_database)
        {
                return(DS_NOTOK);
        } 

/* If the buffer's pointer is null (may be no device names have been returned
   by the db_getdevexp function), leave the function. */

	if (ptr == NULL)
		return(DS_OK);

/* At least one call to db_dataget function before using db_free ? */

	if (first_devexp == 0) 
		return(DS_NOTOK);

/* Does this pointer exist in the tab_clstu array ? */

	for (i = 0;i < func_ptr;i++)
	{
		if (tab_clstu[i].res.res_val.arr1_val == ptr)
			break;
	}

	if (i == func_ptr)
		return(DS_NOTOK);

/* TCP was used for this call ? */

	if (tab_clstu[i].tcp == 1)
	{

/* Free the memory, close the socket and destroy the RPC connection */

#ifdef __STDCPP__
		for (l = 0;l < tab_clstu[i].res.res_val.arr1_len;l++)
			delete [] ptr[l];
		delete [] ptr;
#else
		if (!clnt_freeres(tab_clstu[i].cl,(xdrproc_t)xdr_db_res,(char *)&(tab_clstu[i].res)))
			return(DS_NOTOK);
#endif /* __STDCPP__ */

#ifndef WIN32
		close(tab_clstu[i].tcp_so);
#else
		closesocket(tab_clstu[i].tcp_so);
#endif
		clnt_destroy(tab_clstu[i].cl);
	}

	else
	{

/* Free the memory */

#ifdef __STDCPP__
		for (l = 0;l < tab_clstu[i].res.res_val.arr1_len;l++)
			delete ptr[l];
		delete ptr;
#else
		if (!clnt_freeres(tab_clstu[i].cl,(xdrproc_t)xdr_db_res,(char *)&(tab_clstu[i].res)))
			return(DS_NOTOK);
#endif
	}

	return(DS_OK);
	
}



/**@ingroup dbaseAPImisc
 * The static database is also used to store (as resources) command name associated to command code
 * (in the CMD domain). This function returns the command code associated to a command name.
 *
 * @param cmd_name	The command name
 * @param cmd_code	The command code
 * @param perr		The error code in case of trouble
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_cmd_query(const char *cmd_name, u_int *cmd_code, DevLong *perr)
{
	int i;
	cmd_que *serv_ans;
	DevLong error;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters  (non NULL pointer and one
   \ character in device server name) */

	if ((cmd_name == NULL) || (cmd_code == NULL))
	{
		*cmd_code = 0;
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

# ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*cmd_code = 0;
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Call server */

#ifdef ALONE
	serv_ans = db_cmd_query_1((DevString *)&cmd_name,cl,&error);
#else
	serv_ans = db_cmd_query_1((DevString *)&cmd_name,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (serv_ans != NULL)
	{
		if (serv_ans->db_err == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				serv_ans = db_cmd_query_1((DevString *)&cmd_name,cl,&error);
#else
				serv_ans = db_cmd_query_1((DevString *)&cmd_name,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(serv_ans == NULL)
					break;
				if ((serv_ans->db_err == DS_OK) || (serv_ans->db_err != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it a time-out, try to reconnect to
   a new database server. Don't forget to free memory in case of error. */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&cmd_name,(void **)&serv_ans,&cl,(int)DB_CMDQUERY,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&cmd_name,(void **)&serv_ans,
				     &db_info.conf->clnt,(int)DB_CMDQUERY,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			*cmd_code = 0;
			*perr = error;
			return(DS_NOTOK);
		}
	}	

/* Any problem during database access ? */

	if (serv_ans->db_err != DS_OK)
	{
		*cmd_code = 0;
		*perr = serv_ans->db_err;
		return(DS_NOTOK);
	}

/* No error */

	*cmd_code = serv_ans->xcmd_code;
	*perr = DS_OK;
	return(DS_OK);

}


/**@ingroup dbaseAPImisc
 * The static database is also used to store (as resources) event name associated to event code
 * (in the EVENT domain). This function returns the event code associated to a event name.
 *
 * @param event_name	The event name					
 * @param event_code	The event code					
 * @param perr		The error caode in case of trouble
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "perr". 
 * @retval DS_OK otherwise.
 */
int _DLLFunc db_event_query(char *event_name, u_int *event_code,  DevLong *perr)
{
	int i;
	event_que *serv_ans;
	DevLong error;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters  (non NULL pointer and one
   \ character in device server name) */

	if ((event_name == NULL) || (event_code == NULL))
	{
		*event_code = 0;
		*perr = DbErr_BadParameters;
		return(DS_NOTOK);
	}

# ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*event_code = 0;
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Call server */

#ifdef ALONE
	serv_ans = db_event_query_1(&event_name,cl,&error);
#else
	serv_ans = db_event_query_1(&event_name,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (serv_ans != NULL)
	{
		if (serv_ans->db_err == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				serv_ans = db_event_query_1(&event_name,cl,&error);
#else
				serv_ans = db_event_query_1(&event_name,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(serv_ans == NULL)
					break;
				if ((serv_ans->db_err == DS_OK) || (serv_ans->db_err != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it a time-out, try to reconnect to
   a new database server. Don't forget to free memory in case of error. */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&event_name,(void **)&serv_ans,&cl,(int)DB_EVENTQUERY,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&event_name,(void **)&serv_ans,
				     &db_info.conf->clnt,(int)DB_EVENTQUERY,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			*event_code = 0;
			*perr = error;
			return(DS_NOTOK);
		}
	}	

/* Any problem during database access ? */

	if (serv_ans->db_err != DS_OK)
	{
		*event_code = 0;
		*perr = serv_ans->db_err;
		return(DS_NOTOK);
	}

/* No error */

	*event_code = serv_ans->xevent_code;
	*perr = DS_OK;
	return(DS_OK);

}
#endif /* _OSK */



/**@ingroup dbaseAPIps
 * Register pseudo devices into the database. This feature
 * has been implemented only for control system debug purpose. It helps the debugger
 * to know which process has created pseudo devices and on which computer they are
 * running.
 *
 * @param psdev		The array of pseudo devices information structures
 *               	In each structure, the caller initialize the pseudo device
 *			name and the update period in the dc
 * @param num_psdev 	The pseudo devices number
 * @param p_err		The error code in case of failure
 *
 * @retval DS_NOTOK In case of major trouble, the function sets the error_code 
 *			variable of the error structure. In case of error for a
 *			pseudo device in the list, the function returns 1.
 * @retval DS_OK otherwise.
 *
 * @see db_psdev_unregister
 */
int _DLLFunc db_psdev_register(db_psdev_info *psdev,long num_psdev,db_error *p_err)
{
	register long i;
	int l;
	psdev_reg_x *tmp_x;
	psdev_elt *tmp_x_low;
	DevLong error;
	db_psdev_error *serv_ans;
	static char hostna[HOST_NAME_LENGTH];
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                p_err->error_code = DbErr_NoDatabase;
		p_err->psdev_err = DS_OK;
                return(DS_NOTOK);
        } 

/* Try to verify the function parameters  (non NULL pointer) */

	if ((psdev == NULL) || (num_psdev == 0))
	{
		p_err->error_code = DbErr_BadParameters;
		p_err->psdev_err = DS_OK;
		return(DS_NOTOK);
	}

	for (i = 0;i < num_psdev;i++)
	{
		l = 0;
		NB_CHAR(l,psdev[i].psdev_name,'/');
		if (l != 2)
		{
			p_err->error_code = DbErr_BadParameters;
			p_err->psdev_err = DS_OK;
			return(DS_NOTOK);
		}
	}

# ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			p_err->error_code = DbErr_CannotCreateClientHandle;
			p_err->psdev_err = DS_OK;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory for XDR transfer */

	if ((tmp_x = (psdev_reg_x *)malloc(sizeof(psdev_reg_x))) == NULL)
	{
		p_err->error_code = DbErr_ClientMemoryAllocation;
		p_err->psdev_err = DS_OK;
		return(DS_NOTOK);
	}
	if ((tmp_x_low = (psdev_elt *)calloc(num_psdev,sizeof(psdev_elt))) == NULL)
	{
		free(tmp_x);
		p_err->error_code = DbErr_ClientMemoryAllocation;
		p_err->psdev_err = DS_OK;
		return(DS_NOTOK);
	}

/* Init. the previously allocated structures */

#if defined(WIN32)
	tmp_x->pid = _getpid();
#else  /* WIN32 */
#if !defined (vxworks)
	tmp_x->pid = getpid();
#else  /* !vxworks */
	tmp_x->pid = taskIdSelf();
#endif /* !vxworks */
#endif /* WIN32 */
	taco_gethostname(hostna,sizeof(hostna));
	tmp_x->h_name = hostna;

	tmp_x->psdev_arr.psdev_arr_len = num_psdev;
	tmp_x->psdev_arr.psdev_arr_val = tmp_x_low;

	for (i = 0;i < num_psdev;i++)
	{
		tmp_x_low[i].poll = psdev[i].poll_interval;
		tmp_x_low[i].psdev_name = psdev[i].psdev_name;
	}

/* Call server */

#ifdef ALONE
	serv_ans = db_psdev_reg_1(tmp_x,cl,&error);
#else
	serv_ans = db_psdev_reg_1(tmp_x,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (serv_ans != NULL)
	{
		if (serv_ans->error_code == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				serv_ans = db_psdev_reg_1(tmp_x,cl,&error);
#else
				serv_ans = db_psdev_reg_1(tmp_x,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(serv_ans == NULL)
					break;
				if ((serv_ans->error_code == DS_OK) || (serv_ans->error_code != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it a time-out, try to reconnect to
   a new database server. Don't forget to free memory in case of error. */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)tmp_x,(void **)&serv_ans,&cl,(int)DB_PSDEV_REG,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)tmp_x,(void **)&serv_ans,
				     &db_info.conf->clnt,(int)DB_PSDEV_REG,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			free(tmp_x);
			free(tmp_x_low);
			p_err->error_code = error;
			p_err->psdev_err = DS_OK;
			return(DS_NOTOK);
		}
	}	

/* Free memory */

	free(tmp_x);
	free(tmp_x_low);

/* Any problem during database access ? */

	if (serv_ans->error_code != DS_OK)
	{
		p_err->error_code = serv_ans->error_code;
		p_err->psdev_err = serv_ans->psdev_err;
		return(1);
	}

/* No error */

	p_err->error_code = DS_OK;
	p_err->psdev_err = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPIps
 * Unregister pseudo devices from the database.
 *
 * @param psdev_list	The pseudo devices name list
 * @param num_psdev	The pseudo devices number
 * @param p_err		The error code in case of failure
 *
 * @retval DS_NOTOK In case of major trouble, the function sets the error_code  
 *			variable of the error structure. In case of error for a
 *			pseudo device in the list, the function returns 1.			
 * @retval DS_OK otherwise.
 *
 * @see db_psdev_register
 */
int _DLLFunc db_psdev_unregister(char *psdev_list[],long num_psdev,db_error *p_err)
{
	register long i,j,l;
	arr1 send;
	DevLong error;
	db_psdev_error *serv_ans;
	long i_nethost = 0;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                p_err->error_code = DbErr_NoDatabase;
		p_err->psdev_err = DS_OK;
                return(DS_NOTOK);
        } 
/* Try to verify the function parameters  (non NULL pointer) */

	if ((psdev_list == NULL) || (num_psdev == 0))
	{
		p_err->error_code = DbErr_BadParameters;
		p_err->psdev_err = DS_OK;
		return(DS_NOTOK);
	}

	for (i = 0;i < num_psdev;i++)
	{
		l = 0;
		NB_CHAR(l,psdev_list[i],'/');
		if (l != 2)
		{
			p_err->error_code = DbErr_BadParameters;
			p_err->psdev_err = DS_OK;
			return(DS_NOTOK);
		}
	}

# ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			p_err->error_code = DbErr_CannotCreateClientHandle;
			p_err->psdev_err = DS_OK;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Allocate memory for the array of pointeur to char */

	if((send.arr1_val = (DevString *)calloc(num_psdev,sizeof(DevString))) == NULL)
	{
		p_err->error_code = DbErr_ClientMemoryAllocation;
		p_err->psdev_err = DS_OK;
		return(DS_NOTOK);
	}

/* Initialize the array of pointer to pseudo device name */

	for (i = 0;i < num_psdev;i++)
	{
		l = strlen(psdev_list[i]);
		if ((send.arr1_val[i] = (DevString)malloc(l + 2)) == NULL)
		{
			p_err->error_code = DbErr_ClientMemoryAllocation;
			p_err->psdev_err = DS_OK;
			for (j = 0;j < i;j++)
				free(send.arr1_val[j]);
			free(send.arr1_val);
			return(DS_NOTOK);
		}

		strcpy_tolower(send.arr1_val[i],psdev_list[i]);
	}
	send.arr1_len = num_psdev;

/* Call server */

#ifdef ALONE
	serv_ans = db_psdev_unreg_1(&send,cl,&error);
#else
	serv_ans = db_psdev_unreg_1(&send,db_info.conf->clnt,&error);
#endif /* ALONE */

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

	if (serv_ans != NULL)
	{
		if (serv_ans->error_code == DbErr_DatabaseNotConnected)
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
#ifdef ALONE
				serv_ans = db_psdev_unreg_1(&send,cl,&error);
#else
				serv_ans = db_psdev_unreg_1(&send,db_info.conf->clnt,&error);
#endif /* ALONE */
				if(serv_ans == NULL)
					break;
				if ((serv_ans->error_code == DS_OK) || (serv_ans->error_code != DbErr_DatabaseNotConnected))
					break;
			}
		}
	}

/* Any problem with server ? If yes and if it a time-out, try to reconnect to
   a new database server. Don't forget to free memory in case of error. */

	else
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&send,(void **)&serv_ans,&cl,(int)DB_PSDEV_UNREG,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&send,(void **)&serv_ans,
				     &db_info.conf->clnt,(int)DB_PSDEV_UNREG,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			for (i = 0;i < num_psdev;i++)
				free(send.arr1_val[i]);
			free(send.arr1_val);
			p_err->error_code = error;
			p_err->psdev_err = DS_OK;
			return(DS_NOTOK);
		}
	}	

/* Free memory */

	for (i = 0;i < num_psdev;i++)
		free(send.arr1_val[i]);
	free(send.arr1_val);

/* Any problem during database access ? */

	if (serv_ans->error_code != DS_OK)
	{
		p_err->error_code = serv_ans->error_code;
		p_err->psdev_err = serv_ans->psdev_err;
		return(1);
	}

/* No error */

	p_err->error_code = DS_OK;
	p_err->psdev_err = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPImisc
 * Ask the database server to close all the files needed to store 
 * database data (the ndbm files) allowing another process to open these files.
 * When this function is called, no further call to database server will work until
 * the  @ref db_svc_reopen function will be executed.
 *
 * @param perr	The error code
 * 
 * @retval DS_OK if no errors occurs 
 * @retval DS_NOTOK if a problem occurs
 * 
 * @see db_import
 * @see db_svc_reopen
 */
int _DLLFunc db_svc_close( DevLong *perr)
{
	int *pdb_err;
	DevLong error;
	int padd;
	long i_nethost = 0;
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

#ifdef ALONE

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Call server */

#ifdef ALONE
	pdb_err = db_clodb_1(cl,&error);
#else
	pdb_err = db_clodb_1(db_info.conf->clnt,&error);
#endif

/* Any problem with server ? */

	if (pdb_err == NULL)
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&padd,(void **)&pdb_err,&cl,(int)DB_CLODB,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&padd,(void **)&pdb_err,
				     &db_info.conf->clnt,(int)DB_CLODB,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* Any trouble during database access ? */

	if (*pdb_err != DS_OK)
	{
		*perr = *pdb_err;
		return(DS_NOTOK);
	}

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}



/**@ingroup dbaseAPImisc
 * Ask the database server to reopen database files.
 * 
 * @param perr	The error code
 * 
 * @retval DS_OK if no errors occurs 
 * @retval DS_NOTOK if a problem occurs
 *
 * @see db_import
 * @see db_svc_close
 */
int _DLLFunc db_svc_reopen( DevLong *perr)
{
	int *pdb_err;
	DevLong error;
	int padd;
	long i_nethost = 0;
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

        if (config_flags && config_flags->no_database)
        {
                *perr = DbErr_NoDatabase;
                return(DS_NOTOK);
        } 

#ifdef ALONE
/* Create RPC connection if it's the first call */

	if (!first)
	{
		cl = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		if (cl == NULL)
		{
			*perr = DbErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
		clnt_control(cl,CLSET_TIMEOUT,(char *)&timeout);
		clnt_control(cl,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
		first++;
	}
#endif /* ALONE */

/* Call server */

#ifdef ALONE
	pdb_err = db_reopendb_1(cl,&error);
#else
	pdb_err = db_reopendb_1(db_info.conf->clnt,&error);
#endif

/* Any problem with server ? */

	if (pdb_err == NULL)
	{
		if (error == DevErr_RPCTimedOut || error == DbErr_RPCreception)
		{
#ifdef ALONE
			to_reconnection((void *)&padd,(void **)&pdb_err,&cl,(int)DB_REOPENDB,i_nethost,DB_UDP,&error);
#else
			to_reconnection((void *)&padd,(void **)&pdb_err,
				     &db_info.conf->clnt,(int)DB_REOPENDB,
				     i_nethost,DB_UDP,&error);
#endif /* ALONE */
		}
		if (error != DS_OK)
		{
			*perr = error;
			return(DS_NOTOK);
		}
	}

/* Any trouble during database access ? */

	if (*pdb_err != DS_OK)
	{
		*perr = *pdb_err;
		return(DS_NOTOK);
	}

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}

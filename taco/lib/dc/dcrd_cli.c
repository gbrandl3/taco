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
 * File:	dcrd_cli.c
 *
 * Description:
 *
 * Author(s):	Emmanuel Taurel
 *		$Author: jkrueger1 $
 *
 * Original:	1992
 *
 * Version:	$Revision: 1.22 $
 *
 * Date:	$Date: 2008-10-23 05:26:24 $
 *
 ******************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <dc.h>
#include <dcP.h>

#include <private/ApiP.h>
#include <DevErrors.h>

extern dbserver_info           db_info;

#include "dc_xdr.h"
#include "taco_utils.h"

#ifdef _OSK
#	ifdef _UCC
#		include <stdlib.h>
#		include <string.h>
#	else
#		include <strings.h>
#	endif
#	include <inet/socket.h>
#	include <inet/netdb.h>
#else /* _OSK */
#	include <stdlib.h>
#	include <string.h>
#	include <ctype.h>
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

/* Some global variables */

dc_nethost_info *dc_multi_nethost;
long dc_max_nethost;
long dc_first_call = True;

/* Some functions declaration */

static int test_server(int,serv *,int,CLIENT **,long,DevLong *);
static int re_test_server(int,serv *,int,int,long,DevLong *);
static int rpc_connect(char *,CLIENT **,int,long,DevLong *);
static long build_nethost_arr(dc_dev_imp *,unsigned int,nethost_call **,long *,long**,DevLong *);
static long build_net_arr_index(dc_dev_retdat *,unsigned int,nethost_index **,long *,DevLong *);
static long build_net_arr_indexm(dc_dev_mretdat *,unsigned int,nethost_index **,long *,DevLong *);
static long dc_get_i_nethost_by_name(char *);
static long dc_get_i_nethost_in_call(char *,nethost_call *,long);
static long init_imp(long,DevLong *);
static void get_nethost(char *,char *);
static char *dc_extract_device_name(char *);
static int comp(const void*, const void*);
static int check_dc(int,int,char **,int *,dc_dev_imp *,int *,long,DevLong *);
static int call_dcserv(int,long *,dc_dev_retdat *,int,int *,long,long,DevLong *);
static int call_dcservm(long,long *,dc_dev_mretdat *,int,int *,long,DevLong *);
static long set_err_nethv(long,long *,long,dc_dev_retdat *);
static long set_err_nethm(long,long *,long,dc_dev_mretdat *);

long nb_rd_serv = 10;

/* This function will return an integer between 1 and n */
static int alea(int n)
{
   int i,partSize,maxUsefull,draw;
   
   if(n <= 0)
   	return 1;
   i = n-1;
   partSize   = (i == RAND_MAX) ?        1 : 1 + (RAND_MAX - i)/(i+1);
   maxUsefull = partSize * i + (partSize-1);
   
   do
   {
      draw = rand();
   } while (draw > maxUsefull);
   return (draw/partSize + 1);
}

/**@ingroup dcAPI
 * Determine if a device is registered in the data
 * collector and in this case in which data collector
 *
 * @param dc_devimp 	The caller array with device name
 * @param num_device 	The device number
 * @param error 	Pointer to error code
 *
 * @retval DS_NOTOK In case of major trouble, if there is a problem on only some 
 *			devices, this function returns the number of faulty devices. 
 * @retval DS_OK otherwise.
 */
int dc_import(dc_dev_imp *dc_devimp, unsigned int num_device, DevLong *error)
{
	char 		**tmp_dev,
	 		*tmp,
			*classname,
	 		***devname_arr;
	int		i,
			j,
			l,
	 		**caller_num,
	 		dev_unk = 0,
			dev,
			back = 0;
	nethost_call	*nethost_array;
	DevLong 	err;
	long 		i_nethost,
			i_net_call,
	 		nb_dc_host,
			nb_nethost,
	 		*ind_net_call,
			index,
			num,
			old_nb;
	char 		*env_nethost;

/* 
 * Verify function parameters 
 */
	if (dc_devimp == NULL || error == NULL || num_device == 0)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * Check device name (correct number of /) with or without nethost defined 
 */
	for (i = 0;i < (int)num_device;i++)
	{
		tmp = dc_devimp[i].device_name;
		if (tmp == NULL)
		{
			*error = DcErr_BadParameters;
			return(DS_NOTOK);
		}
		l = 0;
		NB_CHAR(l,tmp,'/');
		if (tmp[0] == '/')
		{
			if ((tmp[1] != '/') || (l != 5))
			{
				*error = DcErr_BadParameters;
				return(DS_OK);
			}
		}
		else if (l != 2)
		{	
			*error = DcErr_BadParameters;
			return(DS_NOTOK);
		}
	}

/* 
 * For the first call, check that the first device has a nethost defined or
 * that the NETHOST environment variable is set. If it is correct, 
 * initialise the default nethost in the global array 
 */
	if (dc_first_call == True)
	{
/* 
 * Allocate memory for the first nethost block 
 */
		if ((dc_multi_nethost = (dc_nethost_info *)calloc(NETHOST_BLOCK,sizeof(dc_nethost_info))) == NULL)
		{
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		dc_max_nethost = NETHOST_BLOCK;
		
/* 
 * First of all, do some init. in the multi nethost array 
 */
		for (i = 0;i < NETHOST_BLOCK;i++)
		{
			dc_multi_nethost[i].first_imp = 0;
			dc_multi_nethost[i].nethost[0] = '\0';
			dc_multi_nethost[i].host_dc.sequence = NULL;
		}

		tmp = dc_devimp[0].device_name;

/* 
 * Get NETHOST env. variable and change it to lower case letter 
 */
		env_nethost = getenv("NETHOST");
		if (env_nethost != NULL)
		{
			env_nethost = str_tolower(env_nethost);
		}

/* 
 * If the NETHOST is specified in the device name and the NETHOST env. variable
 * is also defined, the NETHOST env. variable is the default nethost 
 */
		if (strncmp(tmp,"//",2) == 0)
		{
			if (env_nethost == NULL)
			{
				l = strlen(tmp);
				for (i = 2;i < l;i++)
				{
					if (tmp[i] == '/')
					{
						dc_multi_nethost[0].nethost[i - 2] = '\0';
						break;
					}
					dc_multi_nethost[0].nethost[i - 2] = tolower(tmp[i]);
				}
			}
			else
				strcpy(dc_multi_nethost[0].nethost,env_nethost);
		}

/* 
 * If the nethost is not defined in the device name, the default nethost is
 * set by the NETHOST env. variable 
 */
		else
		{
			if (env_nethost == NULL)
			{
				*error = DcErr_NethostNotDefined;
				return(DS_NOTOK);
			}
			else
				strcpy(dc_multi_nethost[0].nethost,env_nethost);
		}
		dc_first_call = False;
	}

/* 
 * Allocates memory for the nethost array (in tis call) and for the
 * index array 
 */
	if ((nethost_array = (nethost_call *)calloc(NETHOST_BLOCK,sizeof(nethost_call))) == NULL)
	{
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	if ((ind_net_call = (long *)calloc(NETHOST_BLOCK,sizeof(long))) == NULL)
	{
		free(nethost_array);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
/* 
 * Init the index in nethost call array to 0 
 */
	for (i = 0;i < NETHOST_BLOCK;i++)
	{
		ind_net_call[i] = 0;
		nethost_array[i].nb_dev = 0;
		nethost_array[i].dc_ok = True;
	}

/* 
 * Build one array of nethost name used in this call 
 */
	if (build_nethost_arr(dc_devimp,num_device,&nethost_array,&nb_nethost,&ind_net_call,error))
	{
		free(nethost_array);
		free(ind_net_call);
		return(-1);
	}	

/* 
 * Build the multi-nethost array 
 */
	for (i = 0;i < nb_nethost;i++)
	{
		i_nethost = dc_get_i_nethost_by_name(nethost_array[i].nethost);

/* 
 * If the nethost is not known by the process, add it in the multi nethost array 
 */
		if (i_nethost == -1)
		{
			for (j = 1;j < dc_max_nethost;j++)
				if (dc_multi_nethost[j].nethost[0] == '\0')
				{
					strncpy(dc_multi_nethost[j].nethost, nethost_array[i].nethost, sizeof(dc_multi_nethost[j].nethost));
					i_nethost = j;
					break;
				}
			if (j == dc_max_nethost)
			{
				old_nb = dc_max_nethost;
				if ((dc_multi_nethost = (dc_nethost_info *)realloc(dc_multi_nethost,(dc_max_nethost + NETHOST_BLOCK) * sizeof(dc_nethost_info))) == NULL)
				{
					for (l = 0;l < nb_nethost;l++)
						free(nethost_array[l].nethost);
					*error = DcErr_ClientMemoryAllocation;
					return(DS_NOTOK);
				}
				dc_max_nethost = dc_max_nethost + NETHOST_BLOCK;
				for (l = old_nb;l < dc_max_nethost;l++)
				{
					dc_multi_nethost[l].first_imp = 0;
					dc_multi_nethost[l].nethost[0] = '\0';
					dc_multi_nethost[l].host_dc.sequence = NULL;
				}
				
				strncpy(dc_multi_nethost[j].nethost,nethost_array[i].nethost, sizeof(dc_multi_nethost[j].nethost));
			}
		}
	}

/* 
 * Allocate memory for the arrays of pointers to device name. There is one
 * array for each nethost involved in this call. 
 */
	if ((devname_arr = (char ***)calloc(nb_nethost,sizeof(char **))) == NULL)
	{
		for (i = 0;i < nb_nethost;i++)
			free(nethost_array[i].nethost);
		free(nethost_array);
		free(ind_net_call);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (i = 0;i < nb_nethost;i++)
	{
		if ((devname_arr[i] = (char **)calloc(nethost_array[i].nb_dev,sizeof(char *))) == NULL)
		{
			for (j = 0;j < i;j++)
				free(devname_arr[j]);
			free(devname_arr);
			for (j = 0;j < nb_nethost;j++)
				free(nethost_array[j].nethost);
			free(nethost_array);
			free(ind_net_call);
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
	}

/* 
 * Allocate memory for the array of caller device number. There is also one
 * array for each nethost involved in this call. 
 */
	if ((caller_num = (int **)calloc(nb_nethost,sizeof(int *))) == NULL)
	{
		for (i = 0;i < nb_nethost;i++)
		{
			free(nethost_array[i].nethost);
			free(devname_arr[i]);
		}
		free(devname_arr);
		free(nethost_array);
		free(ind_net_call);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (i = 0;i < nb_nethost;i++)
	{
		if ((caller_num[i] = (int *)calloc(nethost_array[i].nb_dev,sizeof(int))) == NULL)
		{
			for (j = 0;j < i;j++)
				free(caller_num[j]);
			free(caller_num);
			for (j = 0;j < nb_nethost;j++)
			{
				free(nethost_array[j].nethost);
				free(devname_arr[j]);
			}
			free(devname_arr);
			free(nethost_array);
			free(ind_net_call);
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
	}

/* 
 * Sort every device by nethost and store device name in one array per nethost.
 * Also store index in the caller array in another nethost specific array.
 * The index in these arrays are stored in another array called ind_net_call.
 * For every device, allocate memory for device name, device name in
 * lowercase letters and initialise the array of caller device number 
 */
	for (i = 0;i < (int)num_device;i++)
	{
		i_net_call = dc_get_i_nethost_in_call(dc_devimp[i].device_name, nethost_array,nb_nethost);
		classname = dc_extract_device_name(dc_devimp[i].device_name);
		l = strlen(classname);
		index = ind_net_call[i_net_call];
		tmp_dev = devname_arr[i_net_call];
		if ((tmp_dev[index] = (char *)malloc((l + 1))) == NULL)
		{
			for (j = 0;j < nb_nethost;j++)
			{
				free(caller_num[j]);
				free(nethost_array[j].nethost);
				for (l = 0;l < ind_net_call[j];l++)
					free(devname_arr[j][l]);
				free(devname_arr[j]);
			}
			free(caller_num);
			free(devname_arr);
			free(nethost_array);
			free(ind_net_call);
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		strcpy_tolower(tmp_dev[index],classname);
		caller_num[i_net_call][index] = i;
		ind_net_call[i_net_call]++;
	}

/* 
 * For each nethost involved in this call, initialise connection if it is the first time it is used 
 */
	back = 0;
	for (i = 0;i < nb_nethost;i++)
	{
		i_nethost = dc_get_i_nethost_by_name(nethost_array[i].nethost);
		if (dc_multi_nethost[i_nethost].first_imp == 0)
		{
			if (init_imp(i_nethost,&err))
			{
				nethost_array[i].dc_ok = False;
				dev = nethost_array[i].nb_dev;
				for (l = 0;l < dev;l++)
				{
					num = caller_num[i][l];
					*(dc_devimp[num].dc_dev_error) = err;
				}
				back += dev;
			}
			else
				dc_multi_nethost[i_nethost].first_imp++;
		}
	}

/* 
 * Init. all the datco pointers to NULL 
 */
	for (i = 0;i < (int)num_device;i++)
		dc_devimp[i].dc_ptr = NULL;

/* 
 * For every nethost and for every dc host in this nethost , ask if the dc knows caller devices 
 */
	for (l = 0;l < nb_nethost;l++)
	{
		if (nethost_array[l].dc_ok == False)
			continue;

		i_nethost = dc_get_i_nethost_by_name(nethost_array[l].nethost);
		dev = nethost_array[l].nb_dev;
		nb_dc_host = dc_multi_nethost[i_nethost].host_dc.length;
		for (i = 0;i < nb_dc_host;i++)
		{
			if (check_dc(i,dev,devname_arr[l],caller_num[l],dc_devimp,&dev_unk,i_nethost,&err) == -1)
			{
				for (j = 0;j < dev;j++)
				{
					num = caller_num[l][j];
					*(dc_devimp[num].dc_dev_error) = err;
				}
				dev_unk = dev;
				break;
			}
			else
				dev = dev_unk;
			if (dev_unk == 0) 
				break;
		}
		back += dev_unk;
	}

/* 
 * Return memory previously allocated 
 */
	for (j = 0;j < nb_nethost;j++)
	{
		free(caller_num[j]);
		free(nethost_array[j].nethost);
		for (l = 0;l < ind_net_call[j];l++)
		{
			if (devname_arr[j][l] != NULL)
				free(devname_arr[j][l]);
		}
		free(devname_arr[j]);
	}
	free(caller_num);
	free(devname_arr);
	free(nethost_array);
	free(ind_net_call);

/* 
 * Leave function 
 */
	return(back);
}


/**@ingroup dcAPIintern
 * Do some initalization. All the work done in this function must be done only once
 *
 * @param i_nethost	The index in the multi nethost array of the nethost to initialize
 * @param perr 		A pointer to an error code
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "error". 
 * @retval DS_OK otherwise.
 */
static long init_imp(long i_nethost, DevLong *perr)
{
	DevLong		error;
	int 		i;
	char 		dc_dev_name[DEV_NAME_LENGTH];
	db_resource 	dcclass_tab[] = {
				{"host",D_VAR_STRINGARR},
				{"max_call",D_LONG_TYPE},
				};
	int 		dcclass_tab_size = sizeof(dcclass_tab)/sizeof(db_resource);
	dc_nethost_info *tmp_net = &(dc_multi_nethost[i_nethost]);;

/* 
 * Build the device name used to retrieve resources 
 */
	snprintf(dc_dev_name, sizeof(dc_dev_name), "//%s/CLASS/DC/1", tmp_net->nethost);

/* 
 * Retrieve some resources 
 */
	dcclass_tab[0].resource_adr = &(tmp_net->host_dc);
	dcclass_tab[1].resource_adr = &(tmp_net->max_call_rd);
	if (db_getresource(dc_dev_name,dcclass_tab,dcclass_tab_size,&error))
	{
		*perr = DcErr_CantGetDcResources;
		return(DS_NOTOK);
	}

#ifdef DEBUG
	printf("Number of dc host : %d\n",tmp_net->host_dc.length);
	for (i = 0;i < tmp_net->host_dc.length;i++) 
		printf("Data collector host : %s\n",tmp_net->host_dc.sequence[i]);
	printf("Number of calls between reconnection : %d\n",tmp_net->max_call_rd);
#endif /* DEBUG */

/* 
 * Reset the dchost array 
 */
	for (i = 0;i < DC_MAX;i++)
	{
		tmp_net->dchost_arr[i].dc_host_name[0] = 0;
		tmp_net->dchost_arr[i].rpc_handle = NULL;
		tmp_net->dchost_arr[i].nb_connect = -1;
		tmp_net->dchost_arr[i].nb_call = 0;
		tmp_net->dchost_arr[i].cantcont_error = 0;
	}

/* 
 * Leave function 
 */
	return(0);
}



/**@ingroup dcAPIintern
 * To compare two values as requested by the qsort
 * function. The definition of this function is
 * available as a UNIX man page
 * @param vpa
 * @param vpb
 * @retval -1 if vpa < vpb
 * @retval 0 if vpa == vpa
 * @retval 1 otherwise
 */
#ifndef WIN32
static int comp(const void *vpa,const void *vpb)
{
#else
static int comp(const void *vpa,const void *vpb)
{
#endif  /* WIN32 */
	serv *a=(serv*)vpa;
	serv *b=(serv*)vpb;
	if (a->request < b->request)
		return(-1);
	else if (a->request == b->request)
		return(0);
	else
		return(1);
}



/**@ingroup dcAPIintern
 * Build an RPC connection to the less heavily loaded data collector server
 *
 * @param serv_name 	The host where the connection must be built
 * @param ind 		The number in the dc host info array
 * @param i_net 	Index in the dc multi nethost array
 * @param prpc 		The address where to store the RPC client handle
 * @param perr 		The address of the error variable
 *
 * @retval DS_OK when no problem occurs. 
 * @retval DS_NOTOK otherwise, the error variable is set according to the error
 */
static int rpc_connect(char *serv_name,CLIENT **prpc,int ind,long i_net,DevLong *perr)
{
	char 			tmp_name[DEV_NAME_LENGTH],
				psd_name[DEV_NAME_LENGTH];
	struct hostent 		*host;
	int 			i,
				nb_server,
				res;
	DevLong			error;
	serv 			serv_info[10];
	unsigned char 		tmp = 0;
	int			rand_nb;
	static db_resource	nb_serv_res[1];
	char 			res_name_rd[64];
#ifdef OSK
	char 			*tmp1;
	unsigned int 		diff;
#endif /* OSK */

/*
 * Get the number of data collector read servers
 */
 	strcpy(res_name_rd, serv_name);
	strcat(res_name_rd, "_rd");
	nb_serv_res[0].resource_name = res_name_rd;
	nb_serv_res[0].resource_type = D_LONG_TYPE;
	nb_serv_res[0].resource_adr = &nb_rd_serv;
	if (db_getresource("class/dc/server_nb",nb_serv_res,1,perr))
	{
		fprintf(stderr,"rpc_connect: Can't retrieve class/dc/server_nb/%s resources\n",res_name_rd);
		return -1;
	}
	
	nb_server = nb_rd_serv;
	srand((int)time(NULL));
	rand_nb = alea(nb_server);

/* 
 * Get data collector server network information 
 */
	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*perr = DcErr_CantGetDcHostInfo;
		return(-1);
	}
	tmp = (unsigned int)host->h_addr[3];

	for (i = 0;i < 10;i++)
	{
		serv_info[i].numb = i + 1;
	}
/* 
 * Build the pseudo device name used to retrieve request resource 
 */
	snprintf(tmp_name, sizeof(tmp_name), "//%s/sys/dc_rd_%u", dc_multi_nethost[i_net].nethost, tmp);
	snprintf(psd_name, sizeof(psd_name), "%s/request", tmp_name);
	strncpy(dc_multi_nethost[i_net].dchost_arr[ind].dc_req_name, psd_name, sizeof(dc_multi_nethost[i_net].dchost_arr[ind].dc_req_name));

/* 
 * Test every server and build the connection with the first one which answers 
 */
	for (i = 0;i < nb_server;i++)
	{
		res = test_server(ind,serv_info,(i+rand_nb) % nb_server,prpc,i_net,&error);
		if (res == 0)
			return(DS_OK);
	}

	if (i == nb_server)
	{
		*perr = error;
		return(DS_NOTOK);
	}

/* 
 * Leave function 
 */
	return(DS_OK);
}



/**@ingroup dcAPIintern
 * Test if a data collector server answers to request.
 *
 * This allows the data collector system to run even if
 * one of its server dies!				
 *
 * @param ind 		The indice in the array of dc host information	
 * @param serv_info 	A pointer to an array of structures. There is one structure for 
 *			each server and one element of the structure is the server request number	
 * @param min 		The indice in the previous array  
 * @param i_net 	Index in the dc multi nethost array		
 * @param clnt_ptr 	A pointer to the client handle which will be used later on  				
 * @param perr 		The address of the error variable
 *
 * @retval DS_OK  when no problem occurs. 
 * @retval DS_NOTOK otherwise, the error variable is set according to the error
 */
static int test_server(int ind,serv *serv_info,int min,CLIENT **clnt_ptr,long i_net,DevLong *perr)
{
	char 		dev_name[40],
			*ret_str,
			*tmp_ptr,
			*tmp1,
			ret_array[DEV_NAME_LENGTH];
	unsigned int 	diff;
	DevLong		error;
	db_devinf_imp 	*serv_net_ptr;
	int 		res = 0;
	CLIENT 		*cl_read;

/* 
 * Build the device name associated with the less heavily loaded server 
 */
	strncpy(dev_name, dc_multi_nethost[i_net].dchost_arr[ind].dc_req_name, sizeof(dev_name));
#ifdef OSK
	if ((tmp_ptr = rindex(dev_name,'/')) != NULL)
#else
	if ((tmp_ptr = strrchr(dev_name,'/')) != NULL)
#endif /* OSK */
	{
		diff = (u_int)(tmp_ptr - dev_name);
		dev_name[diff + 1] = 0;
	}
	snprintf(&(dev_name[strlen(dev_name)]), sizeof(dev_name) - strlen(dev_name), "%d",serv_info[min].numb);


/* 
 * Ask the static database for this server network parameters (host_name,
 * program number and version number) 
 */
	tmp_ptr = dev_name;
	if (db_dev_import(&tmp_ptr,&serv_net_ptr,1,&error))
	{
		*perr = DcErr_CantGetDcServerNetInfo;
		return(DS_NOTOK);
	}

/* 
 * Remove the .esrf.fr at the end of the host name (if any) 
 */

#ifdef OSK
	if ((tmp1 = index(serv_net_ptr->host_name,'.')) != NULL)
#else
	if ((tmp1 = strchr(serv_net_ptr->host_name,'.')) != NULL)
#endif /* OSK */
	{
		diff = (u_int)(tmp1 - serv_net_ptr[0].host_name);
		serv_net_ptr[0].host_name[diff] = 0;
	}

/* 
 * Build the RPC connection to the data collector server 
 */
	cl_read = clnt_create(serv_net_ptr->host_name,serv_net_ptr->pn,serv_net_ptr->vn,"tcp");
	free(serv_net_ptr);
	if (cl_read == NULL)
	{
		*perr = DcErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}

/* 
 * Try to contact this data collector server 
 */
	ret_str = &(ret_array[0]);
	res = dc_rpc_check_clnt_1(cl_read,&ret_str,&error);

/* 
 * If error, destroy the connection and leave function with error code set 
 */
	if (res == DS_NOTOK)
	{
		clnt_destroy(cl_read);
		*perr = error;
		return(DS_NOTOK);
	}

/* 
 * The server answers correctly 
 */
	*clnt_ptr = cl_read;
	dc_multi_nethost[i_net].dchost_arr[ind].serv_num = serv_info[min].numb;
	*perr = 0;
	return(DS_OK);
}



/**@ingroup dcAPIintern
 * Send to a dc server on a specific host a dc_devdef request. The dc server will check 
 * if it knows the devices and, if not send back an error
 *
 * @param ind 		The number of the dc host in the list
 * @param dev_numb 	The number of devices
 * @param devname_arr 	The address of an array to pointers to device name
 * @param call_numb 	The address of an array with the device number in the caller list
 * @param caller_arr 	The caller dc_dev_imp structures array
 * @param i_net 	Index of nethost in the multi_nethost_array
 * @param dev_unk 	A pointer to store the name of unknown devices for this dc 
 * @param perr 		Pointer to error code
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "error". 
 * @retval DS_OK otherwise.
 */
static int check_dc(int ind,int dev_numb,char **devname_arr,int *call_numb,dc_dev_imp *caller_arr,int *dev_unk,long i_net,DevLong *perr)
{
	DevLong	error;
	imppar 	send;
	outpar 	*recev;
	int 	dev_err,
		dev_unknown,
		i,
		j,
		k,
		ind1;
	datco 	*tmp_datco;

/* 
 * Create RPC connection to this dc host if it is not already done 
 */
	if (dc_multi_nethost[i_net].dchost_arr[ind].nb_connect <= 0)
	{
		if (rpc_connect(dc_multi_nethost[i_net].host_dc.sequence[ind],
			        &(dc_multi_nethost[i_net].dchost_arr[ind].rpc_handle),
				ind,i_net,&error))
		{
			*perr = error;
			return(DS_NOTOK);
		}
		strncpy(dc_multi_nethost[i_net].dchost_arr[ind].dc_host_name,dc_multi_nethost[i_net].host_dc.sequence[ind],
				sizeof(dc_multi_nethost[i_net].dchost_arr[ind].dc_host_name));
		dc_multi_nethost[i_net].dchost_arr[ind].nb_connect = 0;
	}

/* 
 * Initialise parameters sent to the server 
 */
	send.imppar_len = dev_numb;
	send.imppar_val = devname_arr;

/* 
 * Call server 
 */
	recev = dc_devdef_1(&send,dc_multi_nethost[i_net].dchost_arr[ind].rpc_handle,&error);

/* 
 * Any problem with data transmission 
 */
	if (recev == NULL)
	{
		*perr = error;
		return(DS_NOTOK);
	}

/* 
 * Major problem during server job ? 
 */
	if (recev->xgen_err != 0)
	{
		*perr = recev->xgen_err;
		return(DS_NOTOK);
	}

	dev_err = recev->taberr.taberr_len;
	j = k = dev_unknown = 0;
	ind1 = ind + 1;
	for (i = 0;i < dev_numb;i++)
	{
/* 
 * Test if the server sent back an error for this device 
 */
		if (dev_err != 0)
		{
			if (i == recev->taberr.taberr_val[j].devnumb)
			{
/* 
 * If it is the last dc host, return error for this device to the caller.
 * Otherwise, set the arrays for the call to the next dc host 
 */
				if (ind1 == (int)dc_multi_nethost[i_net].host_dc.length)
				{
					*(caller_arr[call_numb[i]].dc_dev_error) = (recev->taberr.taberr_val[j]).deverr;
					dev_err--;
					j++;
				}
				else
				{
					call_numb[k] = call_numb[i];
					if (i == 0)
						devname_arr[k++] = devname_arr[i];
					else
					{
						if (i != k)
						{
							free(devname_arr[k]);
							devname_arr[k++] = devname_arr[i];
							devname_arr[i] = NULL;
						}
						else
							k++;
					}
					dev_err--;					
					j++;
					dev_unknown++;
				}
			}
			else
			{
/* 
 * The error is not for this device, so allocate the "datco" structure and initialise it 
 */
				if ((tmp_datco = (datco *)malloc(sizeof(datco))) == NULL)
				{
					caller_arr[call_numb[i]].dc_ptr = NULL;
					*(caller_arr[call_numb[i]].dc_dev_error) = DcErr_ClientMemoryAllocation;
				}
				strncpy(tmp_datco->device_name, devname_arr[i], sizeof(tmp_datco->device_name));
				strncpy(tmp_datco->dc_host_name, dc_multi_nethost[i_net].dchost_arr[ind].dc_host_name, sizeof(tmp_datco->dc_host_name));
				tmp_datco->indice = ind;
				tmp_datco->net_ind = i_net;
				caller_arr[call_numb[i]].dc_ptr = tmp_datco;
				*(caller_arr[call_numb[i]].dc_dev_error) = 0;
				dc_multi_nethost[i_net].dchost_arr[ind].nb_connect++;
			}
		}
		else
		{
/* 
 * Allocate memory for the datco structure and init. its elements 
 */
			if ((tmp_datco = (datco *)malloc(sizeof(datco))) == NULL)
			{
				caller_arr[call_numb[i]].dc_ptr = NULL;
				*(caller_arr[call_numb[i]].dc_dev_error) = DcErr_ClientMemoryAllocation;
			}
			strncpy(tmp_datco->device_name,devname_arr[i], sizeof(tmp_datco->device_name));
			strncpy(tmp_datco->dc_host_name,dc_multi_nethost[i_net].dchost_arr[ind].dc_host_name, sizeof(tmp_datco->dc_host_name));
			tmp_datco->indice = ind;
			tmp_datco->net_ind = i_net;
			caller_arr[call_numb[i]].dc_ptr = tmp_datco;
			*(caller_arr[call_numb[i]].dc_dev_error) = 0;
			dc_multi_nethost[i_net].dchost_arr[ind].nb_connect++;
		}
	}

	if (ind1 == (int)dc_multi_nethost[i_net].host_dc.length)
		*dev_unk = j;
	else
		*dev_unk = dev_unknown;

/* 
 * Free the memory allocated by XDR to return argument 
 */
	clnt_freeres(dc_multi_nethost[i_net].dchost_arr[ind].rpc_handle,
		     (xdrproc_t)xdr_outpar,(char *)recev);

/* 
 * If there are no devices known by this server, destroy the RPC connection 
 */
	if (*dev_unk == dev_numb && dc_multi_nethost[i_net].dchost_arr[ind].nb_connect <= 0)
	{
		clnt_destroy(dc_multi_nethost[i_net].dchost_arr[ind].rpc_handle);
		dc_multi_nethost[i_net].dchost_arr[ind].dc_host_name[0] = 0;
		dc_multi_nethost[i_net].dchost_arr[ind].rpc_handle = NULL;
	}

/* 
 * Leave function 
 */
	return(DS_OK);
}



/**@ingroup dcAPI
 * Close a connection between a client and the data collector for a list of devices. 
 * The datco structures allocated in the dc_import function will be freed here.
 * 
 * @param dc_devfree 	The caller array
 * @param num_device 	The number of device to be freed
 * @param error 	Pointer to error code
 *
 * @retval DS_NOTOK In case of major trouble, if there is a problem on only some 
 *			devices, this function returns the number of faulty devices. 
 * @retval DS_OK otherwise.
 */
int dc_free(dc_dev_free *dc_devfree,unsigned int num_device,DevLong *error)
{
	int 		i,
			l,
			ind,
			dev_err = 0;
	long 		i_net;
	dc_nethost_info *tmp_net;

/* 
 * Verify function parameters 
 */
	if (dc_devfree == NULL || error == NULL || num_device == 0)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

	for (i = 0;i < (int)num_device;i++)
		if (dc_devfree[i].dc_dev_error == NULL)
		{
			*error = DcErr_BadParameters;
			return(DS_NOTOK);
		}
		
/* 
 * For every device in the list, free the datco structure and decrement
 * the host connection number (nb_connect in host dchost structure) 
 */
	for (i = 0;i < (int)num_device;i++)
		if (dc_devfree[i].dc_ptr != NULL)
		{
			ind = (dc_devfree[i].dc_ptr)->indice;
			i_net = (dc_devfree[i].dc_ptr)->net_ind;
			tmp_net = &dc_multi_nethost[i_net];
			tmp_net->dchost_arr[ind].nb_connect--;
			free(dc_devfree[i].dc_ptr);
			*(dc_devfree[i].dc_dev_error) = 0;
		}
		else
		{
			*(dc_devfree[i].dc_dev_error) = DcErr_DeviceNotDcImported;
			dev_err++;
		}

/* 
 * If a connection to a dc host is empty, destroy it 
 */
	for (l = 0;l < dc_max_nethost;l++)
	{
		tmp_net = &dc_multi_nethost[l];
		if (tmp_net->first_imp != 0)
			for (i = 0;i < DC_MAX;i++)
				if ((tmp_net->dchost_arr[i].nb_connect == 0) && 
				    (tmp_net->dchost_arr[i].rpc_handle != NULL))
				{
					clnt_destroy(tmp_net->dchost_arr[i].rpc_handle);
					tmp_net->dchost_arr[i].dc_host_name[0] = '\0';
					tmp_net->dchost_arr[i].rpc_handle = NULL;
					tmp_net->dchost_arr[i].nb_call = 0;
					tmp_net->dchost_arr[i].dc_req_name[0] = 0;
				}
	}
		
/* 
 * Leave function 
 */
	return(dev_err);
}



/**@ingroup dcAPI
 * Retrieve from the data collector the result of a command for one device.
 *
 * @param dc_ptr 	Pointer to dc device handle
 * @param cmd_code 	The command code
 * @param argout_type 	The command result data type
 * @param argout   	Pointer where the command result will be stored
 * @param error 	Pointer to error code
 *
 * @retval DS_NOTOK In case of trouble, the function sets the variable pointed 
 *			to by "error". 
 * @retval DS_OK otherwise.
 */
int dc_devget(datco *dc_ptr,long cmd_code,DevArgument argout,DevType argout_type,DevLong *error)
{
	int 		l;
	xdevget 	send;
	xres_clnt	*recev;
	DevLong		err;
	register int 	ind;
	dc_nethost_info *tmp_net;

/* 
 * Try to verify the function parameters (non NULL pointer) 
 */
	if (dc_ptr == NULL || error == NULL)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * Retrieve nethost and dc server used for this device 
 */
	ind = dc_ptr->indice;
	tmp_net = &dc_multi_nethost[dc_ptr->net_ind];
	
/* 
 * Is it necessary to reconnect to dc server ? 
 */
	if (tmp_net->dchost_arr[ind].cantcont_error == MAXERR)
	{
		if (rpc_reconnect_rd(ind,dc_ptr->net_ind,&err))
		{
			*error = err;
			return(-1);
		}
		tmp_net->dchost_arr[ind].nb_call = 0;
	}

/* 
 * Allocate memory for the device name 
 */
	l = strlen(dc_ptr->device_name);
	if ((send.xdev_name = (char *)malloc(l + 1)) == NULL)
	{
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(send.xdev_name,dc_ptr->device_name);

/* 
 * Initialize the structure sent to the server 
 */
	send.xcmd = cmd_code;
	send.xargout_type = argout_type;

/* 
 * Call server 
 */
	recev = dc_devget_clnt_1(&send,tmp_net->dchost_arr[ind].rpc_handle,argout,argout_type,&err);
	tmp_net->dchost_arr[ind].nb_call++;

/* 
 * Return memory 
 */
	free(send.xdev_name);

/* 
 * Any problem with server ? 
 */
	if (recev == NULL)
	{
		*error = err;
		if ((err == DcErr_CantContactServer) || (err == DcErr_RPCTimedOut))
			tmp_net->dchost_arr[ind].cantcont_error++;
		else
			tmp_net->dchost_arr[ind].cantcont_error = 0;
		return(DS_NOTOK);
	}
	else
		tmp_net->dchost_arr[ind].cantcont_error = 0;

/* 
 * Any problem with data collector access ? 
 */	
	*error = recev->xerr;
	if ((recev->xerr != 0) && (((recev->xerr >> DS_CAT_SHIFT) & DS_CAT_MASK) != WarningError))
		return(DS_NOTOK);

/* 
 * No error 
 */
	argout = (DevArgument)recev->xargout;
	return(DS_OK);
}



/**@ingroup dcAPI
 * Retrieve from the data collector the result of the
 * same command for several devices in the same call.
 * 
 * @param dev_retdat 	The caller array with dc devices handle, pointer to where 
 *			the command result should be stored
 * @param num_device 	The device number
 * @param cmd_code 	The command code
 * @param error 	Pointer to error code
 * 
 * @retval DS_NOTOK In case of major trouble, if there is a problem on only some 
 *			devices, this function returns the number of faulty devices. 
 * @retval DS_OK otherwise.
 */
int dc_devgetv(dc_dev_retdat *dev_retdat,unsigned int num_device,long cmd_code,DevLong *error)
{
	int 		tmpind;
	DevLong		error1;
	long 		**ptr_tabind,
			***tab_ind,
	 		nb_nethost,
	 		max_nethost_call,
	 		i_net,
			nb_dc_host,
			last_ind,
	 		nb_dev,
			nb_err;
	int 		dev_err_host = 0,
			i,
			j,
			k,
			l,
			m,
			max,
			back = 0;
	nethost_index 	*nethost_array;
	dc_nethost_info *tmp_net;

/* 
 * Try to verify function parameters 
 */

	if (dev_retdat == NULL || error == NULL || num_device == 0)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}

/* 
 * Allocate memory for the array where nethost involved in this call will be 
 * stored 
 */
   	if ((nethost_array = (nethost_index *)calloc(NETHOST_BLOCK,sizeof(nethost_index))) == NULL)
	{
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	max_nethost_call = NETHOST_BLOCK;
	
/* 
 * Build nethost array involved in this call 
 */
	for (i = 0;i < max_nethost_call;i++)
	{
		nethost_array[i].nb_dev = 0;
		for (l = 0;l < DC_MAX;l++)
			nethost_array[i].dc_host_ok[l] = True;
	}

	if (build_net_arr_index(dev_retdat,num_device,&nethost_array,&nb_nethost,error) == DS_NOTOK)
		return(DS_NOTOK);

/* 
 * Allocate array(s) used to store device number for each dc host and for 
 * each nethost. We need a three level array. The first level is the
 * the nethost, the second level is the dc host in a specific nethost
 * and the third level is the device recorded in this dc host for this
 * nethost. No headache ?? 
 * Take care of all the memory which must be freed if some allocation
 * failed 
 */
	if ((tab_ind = (long ***)calloc(nb_nethost,sizeof(long **))) == NULL)
	{
		free(nethost_array);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (i = 0;i < nb_nethost;i++)
	{
		i_net = nethost_array[i].net_index;
		nb_dev = nethost_array[i].nb_dev;
		nb_dc_host = dc_multi_nethost[i_net].host_dc.length;

		if ((tab_ind[i] = (long **)calloc(nb_dc_host,sizeof(long *))) == NULL)
		{
			for (k = 0;k < i;k++)
				free(tab_ind[k]);
			free(tab_ind);
			free(nethost_array);
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		for (l = 0;l < nb_dc_host;l++)
			if ((tab_ind[i][l] = (long *)calloc(nb_dev,sizeof(long))) == NULL)
			{
				for (k = 0;k < i;i++)
				{
					nb_dc_host = dc_multi_nethost[nethost_array[k].net_index].host_dc.length;
					for (m = 0;m < nb_dc_host;m++)
						free(tab_ind[k][m]);
					free(tab_ind[k]);
				}
				for (k = 0;k < l;k++)
					free(tab_ind[i][k]);
				free(tab_ind[i]);
				free(tab_ind);
				free(nethost_array);
				*error = DcErr_ClientMemoryAllocation;
				return(-1);
			}
	}

/* 
 * Also allocate memory for the array where device number for each dc host
 * on each nethost will be stored.
 * Now, we need only a two level array. The first level is the
 * the nethost, the second level is the dc host in a specific nethost 
 */
	if ((ptr_tabind = (long **)calloc(nb_nethost,sizeof(long *))) == NULL)
	{
		for (i = 0;i < nb_nethost;i++)
		{
			nb_dc_host = dc_multi_nethost[nethost_array[i].net_index].host_dc.length;
			for (j = 0;l < nb_dc_host;j++)
				free(tab_ind[i][j]);
			free(tab_ind[i]);
		}
		free(tab_ind);
		free(nethost_array);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (i = 0;i < nb_nethost;i++)
	{
		nb_dc_host = dc_multi_nethost[nethost_array[i].net_index].host_dc.length;
		if ((ptr_tabind[i] = (long *)calloc(nb_dc_host,sizeof(long))) == NULL)
		{
			for (k = 0;k < i;k++)
				free(ptr_tabind[k]);
			free(ptr_tabind);
			for (k = 0;k < nb_nethost;k++)
			{
				nb_dc_host = dc_multi_nethost[nethost_array[k].net_index].host_dc.length;
				for (j = 0;l < nb_dc_host;j++)
					free(tab_ind[k][j]);
				free(tab_ind[k]);
			}
			free(tab_ind);
			free(nethost_array);
			*error = DcErr_ClientMemoryAllocation;
			return(-1);
		}
/* 
 * Initialise the newly created array 
 */
		for (l = 0;l < nb_dc_host;l++)
			ptr_tabind[i][l] = 0;
	}

/* 
 * Copy caller device number in the previous array(s) 
 */
	for (i = 0;i < (int)num_device;i++)
		if (dev_retdat[i].dc_ptr != NULL)
		{
			for (j = 0;j < nb_nethost;j++)
			{
				if (nethost_array[j].net_index == dev_retdat[i].dc_ptr->net_ind)
					break;
			}
			tmpind = dev_retdat[i].dc_ptr->indice;
			last_ind = ptr_tabind[j][tmpind];
			tab_ind[j][tmpind][last_ind] = i;
			ptr_tabind[j][tmpind]++;
		}
		else
		{
			*(dev_retdat[i].cmd_error) = DcErr_BadParameters;
			back++;
		}

/* 
 * Is it necessary to reconnect to dc server (on each nethost involved in
 * this call) ? If it is not possible to reconnect to a dc server, mark
 * the nethost/dc-host as not ok and set error code for all the devices
 * registered on this nethost/dc-host in this call. 
 */
	for (j = 0;j < nb_nethost;j++)
	{
		i_net = nethost_array[j].net_index;
		tmp_net = &dc_multi_nethost[i_net];
		nb_dc_host = tmp_net->host_dc.length;
		for (i = 0;i < nb_dc_host;i++)
			if (tmp_net->dchost_arr[i].cantcont_error == MAXERR)
			{
				if (rpc_reconnect_rd(i,i_net,&error1))
				{
					nb_err = set_err_nethv(ptr_tabind[j][i],
							       tab_ind[j][i],
							       error1,
							       dev_retdat);
					back = back + nb_err;
					nethost_array[j].dc_host_ok[i] = False;
				}
				else
				{
					tmp_net->dchost_arr[i].nb_call = 0;
					nethost_array[j].dc_host_ok[i] = True;
				}
			}
	}

/* 
 * For each nethost and each dc host, call the call_dcserv function which 
 * will interogate the dc on one host. Skip the nethost/dc-host if it has been
 * marked as wrong. If the call_dcserv function returns -1, set all the error
 * for the device registered in this nethost/dc-host with the error code
 * except if it is a memory allocation error where the function exits and
 * returns -1. 
 */
	for (j = 0;j < nb_nethost;j++)
	{
		i_net = nethost_array[j].net_index;
		nb_dc_host = dc_multi_nethost[i_net].host_dc.length;
		for (i = 0;i < nb_dc_host;i++)
		{
			if (nethost_array[j].dc_host_ok[i] == False)
				continue;

			if (ptr_tabind[j][i] != 0)
			{
				if (call_dcserv(ptr_tabind[j][i],tab_ind[j][i],
					        dev_retdat,i,&dev_err_host,
						cmd_code,i_net,&error1) == -1)
				{
					if (error1 == DcErr_ClientMemoryAllocation)
					{
						for (k = 0;k < nb_nethost;k++)
						{
							nb_dc_host = dc_multi_nethost[nethost_array[k].net_index].host_dc.length;
							for (l = 0;l < nb_dc_host;l++)
								free(tab_ind[k][l]);
							free(tab_ind[k]);
							free(ptr_tabind[k]);
						}
						free(tab_ind);
						free(ptr_tabind);
						free(nethost_array);
						*error = error1;
						return(-1);
					}
					else
					{
						nb_err = set_err_nethv(ptr_tabind[j][i],
							       tab_ind[j][i],
							       error1,
							       dev_retdat);
						back += nb_err;
					}
				}
				else
					back += dev_err_host;
			}
		}
	}

/* 
 * Return memory previously allocated 
 */
	for (i = 0;i < nb_nethost;i++)
	{
		nb_dc_host = dc_multi_nethost[nethost_array[i].net_index].host_dc.length;
		for (j = 0;j < nb_dc_host;j++)
			free(tab_ind[i][j]);
		free(tab_ind[i]);
		free(ptr_tabind[i]);
	}
	free(tab_ind);
	free(ptr_tabind);
	free(nethost_array);

/* 
 * Leave function 
 */
	return(back);
}



/**@ingroup dcAPIintern
 * To call a dc server on a specific host for a dc_devgetv function
 * 
 * @param num_device 	The device number
 * @param dev_numb 	Pointer to the array where the device number in the caller array 
 *			are on this nethost and on this dc host	
 * @param dev_retdat 	Caller array
 * @param ind 		Index of the dc host for this nethost
 * @param cmd_code 	The command code
 * @param i_net 	index of this nethost in the multi nethost array 
 * @param nb_deverr 	Pointer where the number of faulty devices will be stored
 * @param perr 		Pointer to error code
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "error". 
 * @retval DS_OK otherwise.
 */
static int call_dcserv(int num_device,long *dev_numb,dc_dev_retdat *dev_retdat,int ind,int *nb_deverr,long cmd_code,long i_net,DevLong *perr)
{
	int 		i,
			l,
			k,
	 		tmp_err,
			call_num,
			ret = 0;
	xdevgetv 	send;
	xresv_clnt 	*recev;
	xres_clnt 	*tmp_ptr;
	DevLong		err;
	dc_nethost_info *tmp_net = &dc_multi_nethost[i_net];;

/* 
 * Allocate memory to build the device name in lowercase letters 
 */
	if ((send.xdevgetv_val = (xdevget *)calloc(num_device,sizeof(xdevget))) == NULL)
	{
		*perr = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (k = 0;k < num_device;k++)
	{
		call_num = dev_numb[k];
		l = strlen(dev_retdat[call_num].dc_ptr->device_name);
		if ((send.xdevgetv_val[k].xdev_name = (char *)malloc(l + 1)) == NULL)
		{
			for (i = 0;i < k;i++)
				free(send.xdevgetv_val[i].xdev_name);
			free(send.xdevgetv_val);
			*perr = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		strcpy(send.xdevgetv_val[k].xdev_name,dev_retdat[call_num].dc_ptr->device_name);
		send.xdevgetv_val[k].xcmd = cmd_code;
		send.xdevgetv_val[k].xargout_type = dev_retdat[call_num].argout_type;
	}

/* 
 * Initialize the structure sent to the server 
 */
	send.xdevgetv_len = num_device;

/* Allocate memory for the array of "xres_clnt" structure used by XDR
   routine to deserialise data */

	if ((tmp_ptr = (xres_clnt *)calloc(num_device,sizeof(xres_clnt))) == NULL)
	{
		*perr = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	for (i = 0;i < num_device;i++)
	{
		call_num = dev_numb[i];
		tmp_ptr[i].xerr = 0;
		tmp_ptr[i].xargout_type = (DevType)(dev_retdat[call_num].argout_type);
		tmp_ptr[i].xargout = (DevArgument)(dev_retdat[call_num].argout);
	}

/* 
 * Call server 
 */
	recev = dc_devgetv_clnt_1(&send,tmp_net->dchost_arr[ind].rpc_handle,tmp_ptr,&err);
	tmp_net->dchost_arr[ind].nb_call++;

/* 
 * Return memory allocated to send data to server 
 */
	for (i = 0;i < num_device;i++)
		free(send.xdevgetv_val[i].xdev_name);
	free(send.xdevgetv_val);

/* 
 * Any problem with server ? If it was not possible to contact dc server,
 * set error code for all devices 
 */
	if (recev == NULL)
	{
		free(tmp_ptr);
		if ((err == DcErr_CantContactServer) || (err == DcErr_RPCTimedOut))
		{
			tmp_net->dchost_arr[ind].cantcont_error++;
			for (i = 0;i < num_device;i++)
			{
				call_num = dev_numb[i];
				*(dev_retdat[call_num].cmd_error) = err;
			}
			*nb_deverr = num_device;
			return(0);
		}
		else
			tmp_net->dchost_arr[ind].cantcont_error = 0;
		*perr = err;
		return(-1);
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
		*perr = tmp_err;
		return(DS_NOTOK);
	}

/* 
 * Copy results into caller structures 
 */
	for (i = 0;i < num_device;i++)
	{
		call_num = dev_numb[i];
		tmp_err = recev->xresa_clnt.xresv_clnt_val[i].xerr;
		if (tmp_err != 0)
			ret++;
		*(dev_retdat[call_num].cmd_error) = tmp_err;
		dev_retdat[call_num].argout = (DevArgument)recev->xresa_clnt.xresv_clnt_val[i].xargout;
	}

/* 
 * Free the memory allocated for the XDR routines used for deserialization 
 */
	free(tmp_ptr);

/* 
 * Leave function 
 */
	*nb_deverr = ret;
	*perr = 0;
	return(DS_OK);
}



/**@ingroup dcAPI
 * To retrieve from the data collector the result of the several command result 
 * for several devices in the same call.
 *
 * @param dev_mretdat 	The caller array with dc devices handle, pointer to where the 
 *			command result should be stored, command code ...
 * @param num_device 	The device number
 * @param error 	Pointer to error code
 *
 * @retval DS_NOTOK In case of major trouble, if there is a problem on only some 
 *			devices, this function returns the number of faulty devices. 
 * @retval DS_OK otherwise.
 */
int dc_devgetm(dc_dev_mretdat *dev_mretdat,unsigned int num_device,DevLong *error)
{
	int 		i,
			j,
			k,
			l,
			m,
			tmpind,
			dev_err_host = 0,
			max,
			back = 0;
	long 		**ptr_tabind,
			***tab_ind,
			nb_nethost;
	DevLong		error1;
	nethost_index 	*nethost_array;
	dc_nethost_info *tmp_net;
	long 		i_net,
			nb_dc_host,
			last_ind,
			nb_dev,
			nb_err,
			max_nethost_call;

/* Try to verify the function parameters (non NULL pointer) */

	if (dev_mretdat == NULL || error == NULL || num_device == 0)
	{
		*error = DcErr_BadParameters;
		return(DS_NOTOK);
	}
	for (i = 0;i < (int)num_device;i++)
		if (dev_mretdat[i].cmd_mretdat == NULL)
		{
			*error = DcErr_BadParameters;
			return(DS_NOTOK);
		}
	
/* 
 * Allocate memory for the array where nethost involved in this call will be stored 
 */
   	if ((nethost_array = (nethost_index *)calloc(NETHOST_BLOCK,sizeof(nethost_index))) == NULL)
	{
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	max_nethost_call = NETHOST_BLOCK;
	
/* 
 * Build nethost array involved in this call 
 */
	for (i = 0;i < max_nethost_call;i++)
	{
		nethost_array[i].nb_dev = 0;
		for (l = 0;l < DC_MAX;l++)
			nethost_array[i].dc_host_ok[l] = True;
	}

	if (build_net_arr_indexm(dev_mretdat,num_device,&nethost_array,&nb_nethost,error) == DS_NOTOK)
		return(DS_NOTOK);

/* 
 * Allocate array(s) used to store device number for each dc host and for 
 * each nethost. We need a three level array. The first level is the
 * the nethost, the second level is the dc host in a specific nethost
 * and the third level is the device recorded in this dc host for this
 * nethost. No headache ?? 
 * Take care of all the memory which must be freed if some allocation
 * failed 
 */
	if ((tab_ind = (long ***)calloc(nb_nethost,sizeof(long **))) == NULL)
	{
		free(nethost_array);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (i = 0;i < nb_nethost;i++)
	{
		i_net = nethost_array[i].net_index;
		nb_dev = nethost_array[i].nb_dev;
		nb_dc_host = dc_multi_nethost[i_net].host_dc.length;

		if ((tab_ind[i] = (long **)calloc(nb_dc_host,sizeof(long *))) == NULL)
		{
			for (k = 0;k < i;k++)
				free(tab_ind[k]);
			free(tab_ind);
			free(nethost_array);
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		for (l = 0;l < nb_dc_host;l++)
		{
			if ((tab_ind[i][l] = (long *)calloc(nb_dev,sizeof(long))) == NULL)
			{
				for (k = 0;k < i;i++)
				{
					nb_dc_host = dc_multi_nethost[nethost_array[k].net_index].host_dc.length;
					for (m = 0;m < nb_dc_host;m++)
						free(tab_ind[k][m]);
					free(tab_ind[k]);
				}
				for (k = 0;k < l;k++)
					free(tab_ind[i][k]);
				free(tab_ind[i]);
				free(tab_ind);
				free(nethost_array);
				*error = DcErr_ClientMemoryAllocation;
				return(DS_NOTOK);
			}
		}
	}

/* 
 * Also allocate memory for the array where device number for each dc host
 * on each nethost will be stored.
 * Now, we need only a two level array. The first level is the
 * the nethost, the second level is the dc host in a specific nethost 
 */
	if ((ptr_tabind = (long **)calloc(nb_nethost,sizeof(long *))) == NULL)
	{
		for (i = 0;i < nb_nethost;i++)
		{
			nb_dc_host = dc_multi_nethost[nethost_array[i].net_index].host_dc.length;
			for (j = 0;l < nb_dc_host;j++)
				free(tab_ind[i][j]);
			free(tab_ind[i]);
		}
		free(tab_ind);
		free(nethost_array);
		*error = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (i = 0;i < nb_nethost;i++)
	{
		nb_dc_host = dc_multi_nethost[nethost_array[i].net_index].host_dc.length;
		if ((ptr_tabind[i] = (long *)calloc(nb_dc_host,sizeof(long))) == NULL)
		{
			for (k = 0;k < i;k++)
				free(ptr_tabind[k]);
			free(ptr_tabind);
			for (k = 0;k < nb_nethost;k++)
			{
				nb_dc_host = dc_multi_nethost[nethost_array[k].net_index].host_dc.length;
				for (j = 0;l < nb_dc_host;j++)
					free(tab_ind[k][j]);
				free(tab_ind[k]);
			}
			free(tab_ind);
			free(nethost_array);
			*error = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}

/* 
 * Initialise the newly created array 
 */
		for (l = 0;l < nb_dc_host;l++)
			ptr_tabind[i][l] = 0;
	}

/* 
 * Copy caller device number in the previous array(s) 
 */
	for (i = 0;i < (int)num_device;i++)
	{
		if (dev_mretdat[i].dc_ptr != NULL)
		{
			for (j = 0;j < nb_nethost;j++)
			{
				if (nethost_array[j].net_index == dev_mretdat[i].dc_ptr->net_ind)
					break;
			}
			tmpind = dev_mretdat[i].dc_ptr->indice;
			last_ind = ptr_tabind[j][tmpind];
			tab_ind[j][tmpind][last_ind] = i;
			ptr_tabind[j][tmpind]++;
		}
		else
		{
			for (j = 0;j < (int)dev_mretdat[i].nb_cmd;j++)
			{
				*(dev_mretdat[i].cmd_mretdat[j].cmd_error) = DcErr_BadParameters;
				back++;
			}
		}
	}

/* 
 * Is it necessary to reconnect to dc server (on each nethost involved in
 * this call) ? If it is not possible to reconnect to a dc server, mark
 * the nethost/dc-host as not ok and set error code for all the devices
 * registered on this nethost/dc-host in this call. 
 */
	for (j = 0;j < nb_nethost;j++)
	{
		i_net = nethost_array[j].net_index;
		tmp_net = &dc_multi_nethost[i_net];
		nb_dc_host = tmp_net->host_dc.length;

		for (i = 0;i < nb_dc_host;i++)
		{
			if (tmp_net->dchost_arr[i].cantcont_error == MAXERR)
			{
				if (rpc_reconnect_rd(i,i_net,&error1))
				{
					nb_err = set_err_nethm(ptr_tabind[j][i], tab_ind[j][i], error1, dev_mretdat);
					back += nb_err;
					nethost_array[j].dc_host_ok[i] = False;
				}
				else
				{
					tmp_net->dchost_arr[i].nb_call = 0;
					nethost_array[j].dc_host_ok[i] = True;
				}
			}
		}
	}

/* 
 * For each nethost and each dc host, call the call_dcserv function which 
 * will interogate the dc on one host. Skip the nethost/dc-host if it has been
 * marked as wrong. If the call_dcserv function returns -1, set all the error
 * for the device registered in this nethost/dc-host with the error code
 * except if it is a memory allocation error where the function exits and
 * returns DS_NOTOK. 
 */

	for (j = 0;j < nb_nethost;j++)
	{
		i_net = nethost_array[j].net_index;
		nb_dc_host = dc_multi_nethost[i_net].host_dc.length;
		for (i = 0;i < nb_dc_host;i++)
		{
			if (nethost_array[j].dc_host_ok[i] == False)
				continue;

			if (ptr_tabind[j][i] != 0)
			{
				if (call_dcservm(ptr_tabind[j][i],tab_ind[j][i], dev_mretdat,i,&dev_err_host, i_net,&error1) == -1)
				{
					if (error1 == DcErr_ClientMemoryAllocation)
					{
						for (k = 0;k < nb_nethost;k++)
						{
							nb_dc_host = dc_multi_nethost[nethost_array[k].net_index].host_dc.length;
							for (l = 0;l < nb_dc_host;l++)
								free(tab_ind[k][l]);
							free(tab_ind[k]);
							free(ptr_tabind[k]);
						}
						free(tab_ind);
						free(ptr_tabind);
						free(nethost_array);
						*error = error1;
						return(-1);
					}
					else
					{
						nb_err = set_err_nethm(ptr_tabind[j][i],
							       tab_ind[j][i],
							       error1,
							       dev_mretdat);
						back = back + nb_err;
					}
				}
				else
					back = back + dev_err_host;
			}
		}
	}

/* 
 * Return memory 
 */
	for (i = 0;i < nb_nethost;i++)
	{
		nb_dc_host = dc_multi_nethost[nethost_array[i].net_index].host_dc.length;
		for (j = 0;j < nb_dc_host;j++)
			free(tab_ind[i][j]);
		free(tab_ind[i]);
		free(ptr_tabind[i]);
	}
	free(tab_ind);
	free(nethost_array);
	free(ptr_tabind);

/* 
 * Leave function 
 */
	return(back);

}



/**@ingroup dcAPIintern
 * To retrieve from the data collector the result of the several command result 
 * for several devices in the same call. 
 *
 * @param num_device 	The device number
 * @param dev_numb 	Pointer to the array where the device number in the caller 
 *			array are on this nethost and on this dc host
 * @param dev_mretdat 	Caller array
 * @param ind 		Index of the dc host for this nethost
 * @param i_net 	index of this nethost in the multi nethost array 
 * @param nb_deverr 	Pointer where the number of faulty devices will be stored
 * @param perr 		Pointer to error code
 *
 * @retval DS_NOTOK In case of trouble, the function sets the err variable pointed 
 *			to by "error". 
 * @retval DS_OK otherwise.
 */
static int call_dcservm(long num_device,long *dev_numb,dc_dev_mretdat *dev_mretdat,int ind,int *nb_deverr,long i_net,DevLong *perr)
{
	int 		i,
			j,
			k,
			l,
			call_num,
	 		nb_cmd,
			nb_cmd_sum,
			tmp_err,
			ret = 0;
	mpar 		send;
	xresm_clnt 	*recev;
	DevLong		err;
	xres_clnt 	*tmp_ptr;
	mint 		*tmp_ptr_mint;
	register mxdev 	*tmp;
	register mint 	*tmp1;
	dc_nethost_info *tmp_net = &dc_multi_nethost[i_net];

/* 
 * Allocate memory to build the device name in lowercase letters 
 */
	if ((send.mpar_val = (mxdev *)calloc(num_device,sizeof(mxdev))) == NULL)
	{
		*perr = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	for (k = 0;k < num_device;k++)
	{
		call_num = dev_numb[k];
		l = strlen((dev_mretdat[call_num].dc_ptr)->device_name);
		tmp = &(send.mpar_val[k]);
		if ((tmp->xdev_name = (char *)malloc(l + 1)) == NULL)
		{
			for (i = 0;i < k;i++)
				free(send.mpar_val[i].xdev_name);
			free(send.mpar_val);
			*perr = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		strcpy(tmp->xdev_name,(dev_mretdat[call_num].dc_ptr)->device_name);
	}

/* 
 * Allocate memory for the arrays of "xcmdpar" structures and initialize them 
 */
	nb_cmd_sum = 0;
	for (k = 0;k < num_device;k++)
	{
		call_num = dev_numb[k];
		nb_cmd = dev_mretdat[call_num].nb_cmd;
		nb_cmd_sum = nb_cmd_sum + nb_cmd;
		tmp = &(send.mpar_val[k]);
		tmp->mcmd.mcmd_len = nb_cmd;
		if ((tmp->mcmd.mcmd_val = (xcmdpar *)calloc(nb_cmd,sizeof(xcmdpar))) == NULL)
		{
			for (i = 0;i < num_device;i++)
				free(send.mpar_val[i].xdev_name);
			free(send.mpar_val);
			for (i = 0;i < k;i++)
				free(send.mpar_val[i].mcmd.mcmd_val);
			*perr = DcErr_ClientMemoryAllocation;
			return(DS_NOTOK);
		}
		for (i = 0;i < nb_cmd;i++)
		{
			tmp->mcmd.mcmd_val[i].xcmd = dev_mretdat[call_num].cmd_mretdat[i].cmd_code;
			tmp->mcmd.mcmd_val[i].xargout_type = dev_mretdat[call_num].cmd_mretdat[i].argout_type;
		}
	}
		
/* 
 * Initialize the structure sent to the server 
 */
	send.mpar_len = num_device;

/* 
 * Allocate memory for the array of "mint" structure used by XDR routine to deserialise data 
*/
	if ((tmp_ptr_mint = (mint *)calloc(num_device,sizeof(mint))) == NULL)
	{
		*perr = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

/* 
 * Allocate memory for the array of "xres_clnt" structure used by XDR routine to deserialise data 
 */
	if ((tmp_ptr = (xres_clnt *)calloc(nb_cmd_sum,sizeof(xres_clnt))) == NULL)
	{
		free(tmp_ptr_mint);
		*perr = DcErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}

	l = 0;
	for (i = 0;i < num_device;i++)
	{
		call_num = dev_numb[i];
		nb_cmd = dev_mretdat[call_num].nb_cmd;
		for (j = 0;j < nb_cmd;j++)
		{
			tmp_ptr[l].xerr = 0;
			tmp_ptr[l].xargout_type = (DevType)(dev_mretdat[call_num].cmd_mretdat[j].argout_type);
			tmp_ptr[l].xargout = (DevArgument)(dev_mretdat[call_num].cmd_mretdat[j].argout);
			l++;
		}
	}

/* 
 * Init in the array of "mint" structures, the pointer to the array of "xres_clnt" structures 
 */
	l = 0;
	for (i = 0;i < num_device;i++)
	{
		nb_cmd = dev_mretdat[dev_numb[i]].nb_cmd;
		tmp_ptr_mint[i].mint_val = &(tmp_ptr[l]);
		l = l + nb_cmd;
	}

/* 
 * Call server 
 */
	recev = dc_devgetm_clnt_1(&send,tmp_net->dchost_arr[ind].rpc_handle,tmp_ptr_mint,&err);
	tmp_net->dchost_arr[ind].nb_call++;

/* 
 * Return memory allocated to send data to server 
 */
	for (i = 0;i < num_device;i++)
	{
		free(send.mpar_val[i].xdev_name);
		free(send.mpar_val[i].mcmd.mcmd_val);
	}
	free(send.mpar_val);

/* 
 * Any problem with server ? 
 */
	if (recev == NULL)
	{
		free(tmp_ptr);
		free(tmp_ptr_mint);
		if ((err == DcErr_CantContactServer) || (err == DcErr_RPCTimedOut))
			tmp_net->dchost_arr[ind].cantcont_error++;
		else
			tmp_net->dchost_arr[ind].cantcont_error = 0;
		*perr = err;
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
		free(tmp_ptr_mint);
		*perr = tmp_err;
		return(DS_NOTOK);
	}

/* 
 * Copy results into caller structures 
 */
	for (i = 0;i < num_device;i++)
	{
		call_num = dev_numb[i];
		nb_cmd = dev_mretdat[call_num].nb_cmd;
		tmp1 = &(recev->x_clnt.x_clnt_val[i]);
		for (j = 0;j < nb_cmd;j++)
		{
			tmp_err = tmp1->mint_val[j].xerr;
			if (tmp_err != 0)
				ret++;
			*(dev_mretdat[call_num].cmd_mretdat[j].cmd_error) = tmp_err;
			dev_mretdat[call_num].cmd_mretdat[j].argout = (DevArgument)(tmp1->mint_val[j].xargout);
		}
	}

/* 
 * Free the memory allocated for the XDR routines used for deserialization 
 */
	free(tmp_ptr);
	free(tmp_ptr_mint);

/* 
 * Leave function 
 */
	*nb_deverr = ret;
	*perr = 0;
	return(ret);
}



/**@ingroup dcAPI
 * Ask the static database for every dc read servers
 * request number, to verify that this task is connected 
 * with the less heavily loaded server and if it is not
 * the case, to connect this task to the less heavily 
 * loaded dc write server.
 *
 * @param ind 	The indice in the array of dc host information
 * @param i_net The index in the multi nethost array
 * @param perr  Pointer to error code
 *
 * @retval DS_OK when no problem occurs
 * @retval DS_NOTOK otherwise the error variable is set according to the error
 */
int rpc_reconnect_rd(int ind,long i_net,DevLong *perr)
{
	int 			i,
				res,
				nb_server;
	DevLong			error;
	dc_nethost_info 	*tmp_net;
	serv 			serv_info[10];
	int			rand_nb;

	for (i = 0;i < 10;i++)
	{
		serv_info[i].numb = i + 1;
	}
	nb_server = nb_rd_serv;
	rand_nb = alea(nb_server);

/* 
 * Test every server and keep the connection with the first one which answers 
 */
	for (i = 0;i < nb_server;i++)
	{
		res = re_test_server(ind,serv_info,(i+rand_nb) % nb_server,nb_server,i_net,&error);
		if (res == 0)
			return(DS_OK);
	}
	if (i == nb_server)
	{
		*perr = error;
		return(DS_NOTOK);
	}

/* 
 * Leave function 
 */
	return(DS_OK);

}



/**@ingroup dcAPIintern
 * Test if a data collector server answers to request
 * This allows the data collector system to run even if one of its server dies!
 * 
 * @param ind 		The indice in the array of dc host information
 * @param serv_info 	A pointer to an array of structures. There is one structure 
 *			for each server and one element of the structure is the server request number
 * @param min 		The index in the previous array 
 * @param nb_server 	The data collector read server number on this host
 * @param i_net 	Index into the multi nethost array
 * @param perr 		Pointer to error code
 *
 * @retval DS_OK when no problem occurs. 
 * @retval DS_NOTOK otherwise the error variable is set according to the error
 */
static int re_test_server(int ind,serv *serv_info,int min,int nb_server,long i_net,DevLong *perr)
{
	char dev_name[40];
	char *ret_str;
	char ret_array[40];
	unsigned int diff;
	char *tmp_ptr;
	DevLong error;
	db_devinf_imp *serv_net_ptr;
	int res = 0;
	int already_con = False;
	CLIENT *cl_read;
	dc_nethost_info *tmp_net;
	char *tmp1;

/* If the server is the one on which we are already connected, no need to
   ask server network parameters and to build the RPC connection */

	tmp_net = &dc_multi_nethost[i_net];
	if ((tmp_net->dchost_arr[ind].serv_num == serv_info[min].numb) && (tmp_net->dchost_arr[ind].cantcont_error < MAXERR))
	{
		cl_read = tmp_net->dchost_arr[ind].rpc_handle;
		already_con = True;
	}
	else
	{

/* Build the device name associated with the less heavily loaded server */

		strcpy(dev_name,tmp_net->dchost_arr[ind].dc_req_name);
#ifdef OSK
		tmp_ptr = rindex(dev_name,'/');
#else
		tmp_ptr = strrchr(dev_name,'/');
#endif /* OSK */
		diff = (u_int)(tmp_ptr - dev_name);
		dev_name[diff + 1] = 0;
		snprintf(&(dev_name[strlen(dev_name)]), sizeof(dev_name) - strlen(dev_name), "%d",serv_info[min].numb);

/* Ask the static database for this server network parameters (host_name,
   program number and version number) */

		tmp_ptr = dev_name;

		if (db_dev_import(&tmp_ptr,&serv_net_ptr,1,&error))
		{
			*perr = DcErr_CantGetDcServerNetInfo;
			return(-1);
		}

/* Remove the .esrf.fr at the end of the host name (if any) */

#ifdef OSK
		if ((tmp1 = index(serv_net_ptr[0].host_name,'.')) != NULL)
		{
			diff = (u_int)(tmp1 - serv_net_ptr[0].host_name);
			serv_net_ptr[0].host_name[diff] = 0;
		}
#else
		if ((tmp1 = strchr(serv_net_ptr[0].host_name,'.')) != NULL)
		{
			diff = (u_int)(tmp1 - serv_net_ptr[0].host_name);
			serv_net_ptr[0].host_name[diff] = 0;
		}
#endif /* OSK */

/* Build the RPC connection to the data collector server */

		cl_read = clnt_create(serv_net_ptr[0].host_name,serv_net_ptr[0].pn,serv_net_ptr[0].vn,"tcp");
		free(serv_net_ptr);
		if (cl_read == NULL)
		{
			*perr = DcErr_CannotCreateClientHandle;
			return(-1);
		}
	}

/* Try to contact this data collector server */

	ret_str = &(ret_array[0]);
	res = dc_rpc_check_clnt_1(cl_read,&ret_str,&error);

/* If error, destroy the connection and leave function with error code set */

	if (res == -1)
	{

/* Destroy the connection to this server. If it is the last server, let us say
   that we are not connected to any server on this host */
		if (already_con == False)
			clnt_destroy(cl_read);
		*perr = error;
		return(-1);
	}

/* The server answers correctly, so destroy the old RPC connection and
   leave function without error code */

	else
	{
		if (already_con == False)
		{
			clnt_destroy(tmp_net->dchost_arr[ind].rpc_handle);
			tmp_net->dchost_arr[ind].rpc_handle = cl_read;
			tmp_net->dchost_arr[ind].serv_num = serv_info[min].numb;
		}
		tmp_net->dchost_arr[ind].cantcont_error = 0;
		*perr = 0;
		return(0);
	}

}



/**@ingroup dcAPIintern
 * Build an array of all the nethost used in this call
 *
 * @param p_input 	The caller array with device name
 * @param num_dev 	The device number
 * @param p_array 	The nethost in call array
 * @param p_nethost_nb 	Pointer to nethost number in the array
 * @param p_ind 	Pointer to index array
 * @param p_err 	Pointer to error code
 *
 * @retval DS_OK when no problem occurs. 
 * @retval DS_NOTOK otherwise the variable to which p_err points is set according to error
 */
static long build_nethost_arr(dc_dev_imp *p_input,unsigned int num_dev,
 			      nethost_call **p_array,long *p_nethost_nb,
			      long **p_ind,DevLong *p_err)
{
	register long 	i,
			j,
			k;
	char 		*tmp,
			nethost[HOST_NAME_LENGTH];
	long 		nb_nethost = 0,
			nb_block = 1,
			old_nb_elt,
			new_nb_elt;

/* 
 * A loop for each device 
 */
	for (i = 0;i < num_dev;i++)
	{
		get_nethost(p_input[i].device_name,nethost);
		for (j = 0;j < nb_nethost;j++)
			if (strcmp(nethost,(*p_array)[j].nethost) == 0)
			{
				(*p_array)[j].nb_dev++;
				break;
			}

/* 
 * If the nethost is not found, it is a new nethost 
 */
		if (j == nb_nethost)
		{
			nb_nethost++;

/* 
 * Allocate new arrays if too many nethost are involved in this call 
 */

			if (nb_nethost == (nb_block * NETHOST_BLOCK))
			{
				old_nb_elt = nb_block * NETHOST_BLOCK;
				nb_block++;
				new_nb_elt = nb_block * NETHOST_BLOCK;
				if ((*p_array = (nethost_call*)realloc(*p_array,(new_nb_elt * sizeof(nethost_call)))) == NULL)
				{
					*p_err = DcErr_ClientMemoryAllocation;
					return(DS_NOTOK);
				}
				
				if ((*p_ind = (long *)realloc(*p_ind,(new_nb_elt * sizeof(long)))) == NULL)
				{
					*p_err = DcErr_ClientMemoryAllocation;
					return(DS_NOTOK);
				}
				for (k = old_nb_elt;k < new_nb_elt;k++)
				{
					(*p_ind)[k] = 0;
					(*p_array)[k].nb_dev = 0;
					(*p_array)[k].dc_ok = True;
				}
			}

/* 
 * Allocate memory for a new nethost name 
 */
			if (((*p_array)[nb_nethost - 1].nethost = (char*)malloc(strlen(nethost) + 1)) == NULL)
			{
				for (k = 0;k < nb_nethost;k++)
					free((*p_array)[k].nethost);
				*p_err = DcErr_ClientMemoryAllocation;
				return(DS_NOTOK);
			}
			strcpy((*p_array)[nb_nethost - 1].nethost,nethost);
			(*p_array)[nb_nethost - 1].nb_dev++;
		}
	}
	*p_nethost_nb = nb_nethost;
	return(DS_OK);
}



/**@ingroup dcAPIintern
 * Build an array of all the nethost used in this call 
 *
 * @param p_input 	The caller array with device name
 * @param num_dev 	The device number
 * @param p_array 	The nethost in call array
 * @param p_nethost_nb 	Pointer to nethost number in the array
 * @param p_err 	Pointer to error code
 *
 * @retval DS_OK when no problem occurs. 
 * @retval DS_NOTOK otherwise the variable to which p_err points is set according to error
 */
static long build_net_arr_index(dc_dev_retdat *p_input,unsigned int num_dev,
				nethost_index **p_array,long *p_nethost_nb,
				DevLong *p_err)
{
	register long 	i,
			j,
			k,
			l;
	char 		*tmp;
	long 		nb_nethost = 0,
			nb_block = 1,
			old_nb_elt,
			new_nb_elt;

/* 
 * A loop for each device 
 */
	for (i = 0;i < num_dev;i++)
	{
/* 
 * Skip device if it was not correctly dc imported 
 */
		if (p_input[i].dc_ptr == NULL)
			continue;

		for (j = 0;j < nb_nethost;j++)
			if (p_input[i].dc_ptr->net_ind == (*p_array)[j].net_index)
			{
				(*p_array)[j].nb_dev++;
				break;
			}

/* 
 * If the nethost is not found, it is a new nethost 
 */
		if (j == nb_nethost)
		{
			nb_nethost++;

/* 
 * If the array is full, reallocate memory for a larger one 
 */
			if (nb_nethost == (nb_block * NETHOST_BLOCK))
			{
				old_nb_elt = nb_block * NETHOST_BLOCK;
				nb_block++;
				new_nb_elt = nb_block * NETHOST_BLOCK;
				if ((*p_array = (nethost_index *)realloc(*p_array,(new_nb_elt * sizeof(nethost_index)))) == NULL)
				{
					*p_err = DcErr_ClientMemoryAllocation;
					return(DS_NOTOK);
				}
				
				for (k = old_nb_elt;k < new_nb_elt;k++)
				{
					(*p_array)[k].nb_dev = 0;
					for (l = 0;l < DC_MAX;l++)
						(*p_array)[k].dc_host_ok[l] = True;
				}
			}

/* 
 * It is a new nethost 
 */
			(*p_array)[nb_nethost - 1].net_index = p_input[i].dc_ptr->net_ind;
			(*p_array)[nb_nethost - 1].nb_dev++;
		}
	}
	*p_nethost_nb = nb_nethost;
	return(DS_OK);
}



/**@ingroup dcAPIintern
 * Build an array of all the nethost used in this call 
 * This function is used by the dc_devgetm call
 *
 * @param p_input 	The caller array with device name
 * @param num_dev 	The device number
 * @param p_array 	The nethost in call array
 * @param p_nethost_nb 	Pointer to nethost number in the array
 * @param p_err 	Pointer to error code
 *
 * @retval DS_OK when no problem occurs. 
 * @retval DS_NOTOK otherwise the variable to which p_err points is set according to error
 */
static long build_net_arr_indexm(dc_dev_mretdat *p_input,unsigned int num_dev,
				 nethost_index **p_array,long *p_nethost_nb,
				 DevLong *p_err)
{
	register long 	i,
			j,
			k,
			l;
	char 		*tmp;
	long 		nb_nethost = 0,
			nb_block = 1,
			old_nb_elt,
			new_nb_elt;

/* 
 * A loop for each device 
 */
	for (i = 0;i < num_dev;i++)
	{
/* 
 * Skip device if it was not correctly dc imported 
 */
		if (p_input[i].dc_ptr == NULL)
			continue;

		for (j = 0;j < nb_nethost;j++)
			if (p_input[i].dc_ptr->net_ind == (*p_array)[j].net_index)
			{
				(*p_array)[j].nb_dev++;
				break;
			}

/* 
 * If the nethost is not found, it is a new nethost 
 */
		if (j == nb_nethost)
		{
			nb_nethost++;
			
/* 
 * If the array is full, reallocate memory for a larger one 
 */
			if (nb_nethost == (nb_block * NETHOST_BLOCK))
			{
				old_nb_elt = nb_block * NETHOST_BLOCK;
				nb_block++;
				new_nb_elt = nb_block * NETHOST_BLOCK;
				if ((*p_array = (nethost_index *)realloc(*p_array,(new_nb_elt * sizeof(nethost_index)))) == NULL)
				{
					*p_err = DcErr_ClientMemoryAllocation;
					return(DS_NOTOK);
				}
				
				for (k = old_nb_elt;k < new_nb_elt;k++)
				{
					(*p_array)[k].nb_dev = 0;
					for (l = 0;l < DC_MAX;l++)
						(*p_array)[k].dc_host_ok[l] = True;
				}
			}

/* 
 * It is a new nethost 
 */
			(*p_array)[nb_nethost - 1].net_index = p_input[i].dc_ptr->net_ind;
			(*p_array)[nb_nethost - 1].nb_dev++;
		}
	}
	*p_nethost_nb = nb_nethost;
	return(DS_OK);
}



/**@ingroup dcAPIintern
 * Return the nethost for a device. This nethost can
 * be specified in the device name. If it is not the case,
 * the default nethost is returned
 *
 * This function also removes .esrf.fr at the end of
 * nethost if it exists
 * 
 * @param dev_name The device name
 * @param nethost  The nethost name. Memory must be allocated for this pointer
 */
static void get_nethost(char *dev_name, char *nethost)
{
	register long 	j;
	long 		l;
		
/* 
 * If the nethost is specified in the device name, extract it from here.
 * If it is not defined in the device name, take the default nethost from
 * the global dc_multi_nethost array 
 */
	if (strncmp(dev_name,"//",2) == 0)
	{
		l = strlen(dev_name + 2);
		for (j = 2;j < l;j++)
		{
			if ((dev_name[j] == '/') || (dev_name[j] == '.'))
			{
				nethost[j - 2] = 0;
				break;
			}
			nethost[j - 2] = tolower(dev_name[j]);
		}
	}
	else
		strcpy(nethost,dc_multi_nethost[0].nethost);
	return;
}



/**@ingroup dcAPIintern
 * Get the index for a nethost in the global multi nethost array	
 * 
 * @param nethost The nethost name
 *
 * @return the index in the array 
 * @retval DS_NOTOK if the nethost is not found in the array
 */
static long dc_get_i_nethost_by_name(char *nethost)
{
	register long i;

/* 
 * Loop through the array of nethost information 
 */
	for (i = 0;i < dc_max_nethost;i++)
		if (strcmp(nethost,dc_multi_nethost[i].nethost) == 0)
			return(i);
	return(DS_NOTOK);
}



/**@ingroup dcAPIintern
 * Return the nethost for a device. This nethost can be specified in the 
 * device name. if it is not the case, the default nethost is returned
 * 
 * @param dev_name 	The device name
 * @param net_array 	The array with nethosts name involved in this call
 * @param nb_net 	the number of nethost involved in this call
 *
 * @return the index in the array 
 * @retval DS_NOTOK if the nethost is not found in the array
 */
static long dc_get_i_nethost_in_call(char *dev_name,nethost_call *net_array,long nb_net)
{
	char 		nethost[DEV_NAME_LENGTH];
	register long 	i;

	get_nethost(dev_name,nethost);

	for (i = 0;i < nb_net;i++)
		if (strcmp(nethost, net_array[i].nethost) == 0)
			return(i);
	return(DS_NOTOK);
}



/**@ingroup dcAPIintern
 * Extract the domain/family/memeber part of a device name
 * from a full device name
 * 
 * @param full_name The full device name
 *
 * @return a pointer to the classical part of the full device name
 */
static char *dc_extract_device_name(char *full_name)
{
	char 		*dev_name = full_name;

/* 
 * If nethost is specified in the device name, set the pointer to the
 * classical part of full device name (domain/family/member) 
 */
	if (strncmp(full_name,"//",2) == 0)
	{
		for (dev_name += 2;*dev_name != '/'; dev_name++);
		dev_name++;
	}
	return(dev_name);
}



/**@ingroup dcAPIintern
 * Set all the error code for devices registered in the same nethost (in a dc_devgetv call)
 *
 * @param nb_dev	Pointer to the array of device number for this
 *			nethost (array because the da can be distributed)
 * @param p_ind 	Pointer to array of index in the caller structures 
 *			of the devices for this nethost
 * @param error 	The error code
 * @param dev_retdat 	The caller structures array
 *
 * @return the number of devices for which it sets the error code
 */
static long set_err_nethv(long nb_dev, long *p_ind, long error, dc_dev_retdat *dev_retdat)
{
	long l;
	long call_numb;
	long ret = 0;

	for (l = 0;l < nb_dev;l++)
	{
		call_numb = p_ind[l];
		*(dev_retdat[call_numb].cmd_error) = error;
		ret++;
	}
	return(ret);
}



/**@ingroup dcAPIintern
 * Set all the error code for devices registered in the
 * same nethost (in a dc_devgetm call)
 *
 * @param nb_dev 	Pointer to the array of device number for this
 *			nethost (array because the da can be distributed)
 * @param p_ind 	Pointer to array of index in the caller structures
 *			of the devices for this nethost
 * @param error 	The error code
 * @param dev_mretdat 	The caller structures array
 *
 * @return the number of devices for which it sets the error code	
 */
static long set_err_nethm(long nb_dev,long *p_ind,long error,dc_dev_mretdat *dev_mretdat)
{
	long 	l,
		j,
		call_numb,
		nb_cmd,
		ret = 0;

	for (l = 0;l < nb_dev;l++)
	{
		call_numb = p_ind[l];
		nb_cmd = dev_mretdat[call_numb].nb_cmd;
		for (j = 0;j < nb_cmd;j++)
		{
			*(dev_mretdat[call_numb].cmd_mretdat[j].cmd_error) = error;
			ret++;
		}
	}
	return(ret);
}

/*
 * Author(s):     Emmanuel Taurel
 *		$Author: jkrueger1 $
 *
 * Original:      1992
 *
 * Version:       $Revision: 1.4 $
 *
 * Date:          $Date: 2003-05-22 08:04:17 $
 *
 * Copyright (c) 1990 by European Synchrotron Radiation Facility,
 *                       Grenoble, France
 */

#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>
#include <db_setup.h>

#include <dc.h>
#include <dcP.h>
#include <dc_xdr.h>

#ifndef _OSK
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#ifndef _NT
#include <sys/socket.h>
#ifndef __hp9000s300
#include <netinet/in.h>
#endif
#include <netdb.h>
#endif  /* _NT */
#else /* _OSK */
#ifdef _UCC
#include <stdlib.h>
#include <string.h>
#else
#include <strings.h>
#endif /* _UCC */
#include <inet/socket.h>
#include <inet/netdb.h>
#endif /* _OSK */

/* Some global variables */

CLIENT 		*cl;
int 		first = 0,
 		call_numb = 0,
 		max_call = 0,
		err_count = 0;
char 		psd_name[40],
 		tmp_name[40];
struct timeval 	time_out;

/* 
 * Some extern variables (I know that this is against the GPS !!) 
 */

extern configuration_flags config_flags;
extern dbserver_info db_info;

/* 
 * Some functions declaration 
 */

static int test_server();
static int re_test_server();
static int rpc_reconnect();
static int rpc_connect();
#ifndef _NT
static int comp();
#else
static int comp(const void*, const void*);
#endif   /* _NT */
static int get_dc_host(char **p_serv_name,long *perr);



/**@ingroup dcAPI
 * Initialise data storing in data collector.          
 *
 * This function is called by the update daemon before    *
 * any dc_dataput call. This function will initialize     *
 * information in the data collector database part and    *
 * allocate shared memory used to store data.             *
 * 
 * @param tab
 * @param num_dev
 * @param perr
 *
 * @return In case of trouble, the function returns DS_NOTOK and sets the err variable
 *    pointed to by "perr". Otherwise, the function returns  DS_OK 
 */
int dc_open(dc_dev *tab,unsigned int num_dev,dc_error *perr)
{
	int 			i,
				j,
				k,
				l,
	 			*ps_dev_arr,
				elt,
				ind;
	dc_open_in 		send;
	register dc_dev_x 	*ptr;
	long 			error,
				nb_psdev,
	 			vers;
	dc_xdr_error 		*recev;
	Db_devinf_imp 		p_db_imp;
	db_psdev_info 		*ps_dev;
	db_error 		db_err;

/* 
 * Try to verify the function parameters (non NULL pointer and two '/' characters in the device name) 
 */

	if (tab == NULL || num_dev == 0)
	{
		perr->error_code = DcErr_BadParameters;
		perr->dev_error = 0;
		return(-1);
	}

	for (i=0;i<(int)num_dev;i++)
	{
		l = 0;
		NB_CHAR(l,tab[i].device_name,'/');
		if (l != 2)
		{
			perr->error_code = DcErr_BadParameters;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
	}

/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		if (rpc_connect(&error))
		{
			perr->error_code = error;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		first++;
	}

/* 
 * Reconnect (if needed) to a new dc server if the distribution load is not correct 
 */
	if ((call_numb == max_call) || (err_count == MAXERR))
	{
		if (rpc_reconnect(&error))
		{
			perr->error_code = error;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		call_numb = 0;
	}

/* 
 * Get database server version 
 */
	vers = db_info.conf->vers_number;

/* 
 * Manage pseudo devices only if the database server version is greater than 2 
 */
	if (vers > DB_VERS_2)
	{
/* 
 * Allocate memory to store pseudo devices index in the user array 
 */
		if ((ps_dev_arr = (int *)calloc(num_dev,sizeof(long))) == (int *)NULL)
		{
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		nb_psdev = 0;
/* 
 * Try a db_dev_import on every devices from the user array. If it fails,
 * this means that it is a pseudo device 
 */
		ind = 0;
		for (k = 0;k < (int)num_dev;k++)
		{
			if (db_dev_import(&(tab[k].device_name),&p_db_imp,1,&error) == -1)
			{
				if (error == DbErr_DeviceNotDefined)
				{
					ps_dev_arr[ind] = k;
					nb_psdev++;
					ind++;
				}
				else
				{
					if (error == DbErr_DeviceNotExported)
						continue;
					else
					{
						free(ps_dev_arr);
						perr->error_code = DcErr_CantTestPSDevice;
						perr->dev_error = 0;
						return(-1);
					}
				}
				continue;
			}
			free(p_db_imp);
		}
	}
	else
		nb_psdev = 0;

/* 
 * Register pseudo devices in the database 
 */
	if (nb_psdev != 0)
	{
/* 
 * Allocate mem. for the structures used in the db_psdev_register call 
 */
		if ((ps_dev = (db_psdev_info *)calloc(nb_psdev,sizeof(db_psdev_info))) == (db_psdev_info *)NULL)
		{
			free(ps_dev_arr);
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}

/* 
 * Init structures 
 */
		for (k = 0;k < nb_psdev;k++)
		{
			elt = ps_dev_arr[k];
			ps_dev[k].psdev_name = tab[elt].device_name;
			ps_dev[k].poll_interval = tab[elt].poll_interval;
		}

/* 
 * Register pseudo devices 
 */
		if (db_psdev_register(ps_dev,nb_psdev,&db_err) == -1)
		{
			free(ps_dev_arr);
			free(ps_dev);
			perr->error_code = DcErr_CantRegisterPSDevice;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		free(ps_dev);
	}
	if (vers > DB_VERS_2)
		free(ps_dev_arr);

/* 
 * Allocate memory to build the array of pointers to strings and all device
 * names in lowercase letters 
 */
	if ((send.dc_open_in_val = (dc_dev_x *)calloc(num_dev,sizeof(dc_dev_x))) == NULL)
	{
		perr->error_code = DcErr_ClientMemoryAllocation;
		perr->dev_error = 0;
		return(DS_NOTOK);
	}

	for (k=0;k<(int)num_dev;k++)
	{
		ptr = &(send.dc_open_in_val[k]);
		l = strlen(tab[k].device_name);
		if ((ptr->dev_name = (char *)malloc(l + 1)) == NULL)
		{
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			for(i=0;i<k;i++)
				free(send.dc_open_in_val[i].dev_name);
			free(send.dc_open_in_val);
			return(DS_NOTOK);
		}
		strcpy(ptr->dev_name,tab[k].device_name);
		for(j=0;j<l;j++)
			ptr->dev_name[j] = tolower(ptr->dev_name[j]);
	}

/* 
 * Structure initialization (structure sended to server) 
 */
	send.dc_open_in_len = num_dev;
	for (k=0;k<(int)num_dev;k++)
	{
		send.dc_open_in_val[k].dc_cmd_ax.dc_cmd_ax_len = tab[k].nb_cmd;
		send.dc_open_in_val[k].dc_cmd_ax.dc_cmd_ax_val = (dc_cmd_x *)tab[k].dev_cmd;
		send.dc_open_in_val[k].poll_int = tab[k].poll_interval;
	}

/* 
 * Call server 
 */
	recev = dc_open_1(&send,cl,&error);
	call_numb++;

/* 
 * Return memory 
 */
	for (i=0;i<(int)num_dev;i++)
		free(send.dc_open_in_val[i].dev_name);
	free(send.dc_open_in_val);

/* 
 * Any problem with server ? 
 */
	if (recev == NULL)
	{
		perr->error_code = error;
		perr->dev_error = 0;
		if (error == DcErr_CantContactServer)
			err_count++;
		else
			err_count = 0;
		return(DS_NOTOK);
	}
	else
		err_count = 0;

/* 
 * Any problem with data collector access ? 
 */	
	if (recev->error_code != 0)
	{
		*perr = *(dc_error *)recev;
		if (recev->dev_error == 0)
			return(DS_NOTOK);
		else
			return(1);
	}

/* 
 * No error 
 */
	perr->error_code = 0;
	perr->dev_error = 0;
	return(DS_OK);
}



/**@ingroup dcAPI
 * To unregister a list of devices from the data collector
 * This function is called by the update daemon when it
 * realize that a device which was polled is not polled anymore	
 * 
 * @param dev_array
 * @param num_dev
 * @param perr
 *
 * @return In case of trouble, the function returns DS_NOTOK and set the err variable
 *    pointed to by "perr". Otherwise, the function returns DS_OK
 */
int dc_close(char **dev_array,unsigned int num_dev,dc_error *perr)
{
	int 		i,
			j,
			k,
			l,
			*ps_dev_arr,
			elt,
			ind;
	name_arr 	send;
	dc_xdr_error 	*recev;
	long 		error,
			vers,
			nb_psdev;
	Db_devinf_imp 	p_db_imp;
	char 		**ps_dev;
	db_error 	db_err;

/* 
 * Try to verify the function parameters (non NULL pointer and two '/' characters in the device name) 
 */
	if (num_dev == 0)
	{
		perr->error_code = DcErr_BadParameters;
		perr->dev_error = 0;
		return(DS_NOTOK);
	}

	for (i=0;i<(int)num_dev;i++)
	{
		l = 0;
		NB_CHAR(l,dev_array[i],'/');
		if (l != 2)
		{
			perr->error_code = DcErr_BadParameters;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
	}

/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		if (rpc_connect(&error))
		{
			perr->error_code = error;
			perr->dev_error = 0;
			return(-1);
		}
		first++;
	}

/* 
 * Reconnect (if needed) to a new dc server if the distribution load is not correct 
 */
	if ((call_numb == max_call) || (err_count == MAXERR))
	{
		if (rpc_reconnect(&error))
		{
			perr->error_code = error;
			perr->dev_error = 0;
			return(-1);
		}
		call_numb = 0;
	}

/* 
 * Get database server version and manage pseudo devices only the database 
 * server version is greater than 2 
 */
	vers = db_info.conf->vers_number;
	if (vers > DB_VERS_2)
	{
/* 
 * Allocate memory to store pseudo devices index in the user array 
 */
		if ((ps_dev_arr = (int *)calloc(num_dev,sizeof(long))) == (int *)NULL)
		{
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		nb_psdev = 0;

/* 
 * Try to db_dev_import every devices from the user array. If it fails,
 * this means that it is a pseudo device 
 */
		ind = 0;
		for (k = 0;k < (int)num_dev;k++)
		{
			if (db_dev_import(&(dev_array[k]),&p_db_imp,1,&error) == -1)
			{
				if (error == DbErr_DeviceNotDefined)
				{
					ps_dev_arr[ind] = k;
					nb_psdev++;
					ind++;
				}
				else
				{
					if (error = DbErr_DeviceNotExported)
						continue;
					else
					{
						free(ps_dev_arr);
						perr->error_code = DcErr_CantTestPSDevice;
						perr->dev_error = 0;
						return(DS_NOTOK);
					}
				}
				continue;
			}
			free(p_db_imp);
		}
	}
	else
		nb_psdev = 0;

/* 
 * Unregister pseudo devices in the database 
 */
	if (nb_psdev != 0)
	{

/* 
 * Allocate mem. for the array used in the db_psdev_unregister call 
 */
		if ((ps_dev = (char **)calloc(nb_psdev,sizeof(char *))) == (char **)NULL)
		{
			free(ps_dev_arr);
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}

/* 
 * Init array 
 */
		for (k = 0;k < nb_psdev;k++)
		{
			elt = ps_dev_arr[k];
			ps_dev[k] = dev_array[elt];
		}

/* 
 * Unregister pseudo devices 
 */
		if (db_psdev_unregister(ps_dev,nb_psdev,&db_err) == -1)
		{
			free(ps_dev_arr);
			free(ps_dev);
			perr->error_code = DcErr_CantUnregisterPSDevice;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}

		free(ps_dev);
	}
	if (vers > DB_VERS_2)
		free(ps_dev_arr);

/* 
 * Allocate memory to build the array of pointers to strings and all device
 * names in lowercase letters 
 */
	if ((send.name_arr_val = (name *)calloc(num_dev,sizeof(name))) == NULL)
	{
		perr->error_code = DcErr_ClientMemoryAllocation;
		perr->dev_error = 0;
		return (DS_NOTOK);
	}

	for (k=0;k<(int)num_dev;k++)
	{
		l = strlen(dev_array[k]);
		if ((send.name_arr_val[k] = (char *)malloc(l + 1)) == NULL)
		{
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			for(i=0;i<k;i++)
				free(send.name_arr_val[i]);
			free(send.name_arr_val);
			return(DS_NOTOK);
		}
		strcpy(send.name_arr_val[k],dev_array[k]);
		for(j=0;j<l;j++)
			send.name_arr_val[k][j] = tolower(send.name_arr_val[k][j]);
	}

/* 
 * Initialize the device number in the structure sent to the server 
 */
	send.name_arr_len = num_dev;

/* 
 * Call server 
 */
	recev = dc_close_1(&send,cl,&error);
	call_numb++;

/* 
 * Return memory 
 */
	for (i=0;i<(int)num_dev;i++)
		free(send.name_arr_val[i]);
	free(send.name_arr_val);

/* 
 * Any problem with server ? 
 */
	if (recev == NULL)
	{
		perr->error_code = error;
		perr->dev_error = 0;
		if (error == DcErr_CantContactServer)
			err_count++;
		else
			err_count = 0;
		return(DS_NOTOK);
	}
	else
		err_count = 0;

/* 
 * Any problem with data collector access ? 
 */	
	if (recev->error_code != 0)
	{
		*perr = *(dc_error *)recev;
		if (recev->dev_error == 0)
			return(DS_NOTOK);
		else
			return(1);
	}

/* 
 * No error 
 */
	perr->error_code = 0;
	perr->dev_error = 0;
	return(DS_OK);
}



/**@ingroup dcAPI
 * Store in the data collector data buffer multiple (if wanted) command results 
 * for multiple (if wanted) devices.
 *
 * @param dev_data
 * @param num_dev
 * @param perr
 *
 * @return In case of trouble, the function returns DS_NOTOK and set the err variable
 *    pointed to by "perr". Otherwise, the function returns DS_OK
 */
int dc_dataput(dc_dev_dat *dev_data,unsigned int num_dev,dc_error *perr)
{
	int 			i,
				j,
				k,
				l;
	dev_datarr 		send;
	register dev_dat 	*ptr;
	long 			error;
	dc_xdr_error 		*recev;

/* 
 * Try to verify the function parameters (non NULL pointer and two '/' characters in the device name) 
 */
	if (dev_data == NULL || num_dev == 0)
	{
		perr->error_code = DcErr_BadParameters;
		perr->dev_error = 0;
		return(DS_NOTOK);
	}

	for (i=0;i<(int)num_dev;i++)
	{
		if (dev_data[i].cmd_data == NULL)
		{
			perr->error_code = DcErr_BadParameters;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		l = 0;
		NB_CHAR(l,dev_data[i].device_name,'/');
		if (l != 2)
		{
			perr->error_code = DcErr_BadParameters;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
	}

/* 
 * Create RPC connection if it's the first call 
 */
	if (!first)
	{
		if (rpc_connect(&error))
		{
			perr->error_code = error;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		first++;
	}

/* 
 * Reconnect (if needed) to a new dc server if the distribution load is not correct 
 */
	if ((call_numb == max_call) || (err_count == MAXERR))
	{
		if (rpc_reconnect(&error))
		{
			perr->error_code = error;
			perr->dev_error = 0;
			return(DS_NOTOK);
		}
		call_numb = 0;
	}

/* 
 * Allocate memory to build the array of dc_dev_dat structures and all device
 * names in lowercase letters 
 */
	if ((send.dev_datarr_val = (dev_dat *)calloc(num_dev,sizeof(dev_dat))) == NULL)
	{
		perr->error_code = DcErr_ClientMemoryAllocation;
		perr->dev_error = 0;
		return(DS_NOTOK);
	}

	for (k=0;k<(int)num_dev;k++)
	{
		ptr = &(send.dev_datarr_val[k]);
		l = strlen(dev_data[k].device_name);
		if ((ptr->xdev_name = (char *)malloc(l + 1)) == NULL)
		{
			perr->error_code = DcErr_ClientMemoryAllocation;
			perr->dev_error = 0;
			for(i=0;i<k;i++)
				free(send.dev_datarr_val[i].xdev_name);
			free(send.dev_datarr_val);
			return(DS_NOTOK);
		}
		strcpy(ptr->xdev_name,dev_data[k].device_name);
		for(j=0;j<l;j++)
			ptr->xdev_name[j] = tolower(ptr->xdev_name[j]);
		ptr->xcmd_dat.xcmd_dat_len = dev_data[k].nb_cmd;
		ptr->xcmd_dat.xcmd_dat_val = (cmd_dat *)dev_data[k].cmd_data;
	}

/* 
 * Structure initialization (structure sended to server) 
 */
	send.dev_datarr_len = num_dev;

/* 
 * Call server 
 */
	recev = dc_dataput_1(&send,cl,&error);
	call_numb++;

/* 
 * Return memory 
 */
	for (i=0;i<(int)num_dev;i++)
		free(send.dev_datarr_val[i].xdev_name);
	free(send.dev_datarr_val);

/* 
 * Any problem with server ? 
 */
	if (recev == NULL)
	{
		perr->error_code = error;
		perr->dev_error = 0;
		if (error == DcErr_CantContactServer)
			err_count++;
		else
			err_count = 0;
		return(DS_NOTOK);
	}
	else
		err_count = 0;

/* 
 * Any problem with data collector access ? 
 */	
	if (recev->error_code != 0)
	{
		*perr = *(dc_error *)recev;
		if (recev->dev_error == 0)
			return(DS_NOTOK);
		else
			return(1);
	}

/* 
 * No error 
 */
	perr->error_code = 0;
	perr->dev_error = 0;
	return(DS_OK);
}



/**@ingroup dcAPIintern
 * Compare two values as requested by the qsort 
 * function. The definition of this qsort function is 
 * available as a UNIX man page.
 */
static int comp(serv *a, serv *b)
{
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
 * @param The address of the error variable
 * 
 * @return This function returns DS_OK when no problem occurs. Otherwise the return
 *    value is DS_NOTOK and the error variable is set according to the error
 */
static int rpc_connect(long *perr)
{
	char 			*serv_name;
	struct hostent 		*host;
	int 			i,
				res,
				nb_server;
	long 			error;
	serv 			serv_info[10];
	unsigned char 		tmp = 0;
	static db_resource 	res_call[] = {
					{"max_call",D_LONG_TYPE},
					{"timeout",D_LONG_TYPE},
					},
				res_tab[] = {
					{"1",D_LONG_TYPE},
					{"2",D_LONG_TYPE},
					{"3",D_LONG_TYPE},
					{"4",D_LONG_TYPE},
					{"5",D_LONG_TYPE},
					{"6",D_LONG_TYPE},
					{"7",D_LONG_TYPE},
					{"8",D_LONG_TYPE},
					{"9",D_LONG_TYPE},
					{"10",D_LONG_TYPE},
					};

/* 
 * If the RPC connection to static database server is not built, build one.
 * The "config_flags" variable is defined as global by the device server
 * API library. 
 */
	if ((config_flags.database_server != True) && db_import(&error))
	{
		*perr = DcErr_CantBuildStaDbConnection;
		return(DS_NOTOK);
	}

/* 
 * Get the max number of call to server betwen each RPC reconnection 
 */
	res_call[0].resource_adr = &max_call;
	res_call[1].resource_adr = &(time_out.tv_sec);
	if (db_getresource("CLASS/DC/1",res_call,2,&error))
	{
		*perr = DcErr_CantGetDcResources;
		return(DS_NOTOK);
	}

/* 
 * Get the host name where the data collector is running 
 */
	if (get_dc_host(&serv_name,&error))
	{
		*perr = error;
		return(DS_NOTOK);
	}

/* 
 * Get data collector server network information 
 */
	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*perr = DcErr_CantGetDcHostInfo;
		return(DS_NOTOK);
	}
	tmp = (unsigned char)host->h_addr[3];

/* 
 * Build the pseudo device name used to retrieve request resource 
 */
	snprintf(tmp_name, sizeof(tmp_name), "sys/dc_wr_%u", tmp_name);
	snprintf(psd_name, sizeof(psd_name), "%s/request", tmp_name);

/* 
 * Fullfil the resource array 
 */
	for (i = 0;i < 10;i++)
	{
		serv_info[i].request = 0xFFFFFFFF;
		res_tab[i].resource_adr = &(serv_info[i].request);
		serv_info[i].numb = i + 1;
	}

/* 
 * Get resource values from database 
 */
	if (db_getresource(psd_name,res_tab,10,&error))
	{
		*perr = DcErr_CantGetDcResources;
		return(DS_NOTOK);
	}

/* 
 * Find how many servers are defined 
 */
	for (i = 0;i < 10;i++)
		if (serv_info[i].request == 0xFFFFFFFF)
			break;
	nb_server = i;

/* 
 * Sort the serv_info table in the ascending order 
 */
#ifndef _NT
	qsort(&(serv_info[0]),nb_server,sizeof(serv),(int (*)())comp);
#else
	qsort(&(serv_info[0]),nb_server,sizeof(serv),comp);
#endif   /* _NT */

/* 
 * Test every server and keep the connection with the first one which answers 
 */
	for (i = 0;i < nb_server;i++)
	{
		res = test_server(serv_info,i,&error);
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
 * Test a data collector server and to if the test is successful, to switch 
 * the calling process to this server
 * 
 * @param serv_info 	array of structures with one structure per server. The 
 *			server number of request is one element of this structure.
 * @param min 		The element in the structure array to take into account
 * @param perr 		The address of the error variable
 *
 * @return DS_OK when no problem occurs. Otherwise the return value is DS_NOTOK 
 *	and the error variable is set according to the error
 */
static int test_server(serv *serv_info, int min, long *perr)
{
	char 		serv1[40],
			*tmp_ptr,
			*ret_str,
	 		*tmp1,
			ret_array[40];
	db_devinf_imp 	*serv_net_ptr;
	long 		error;
	CLIENT 		*cl_write;
	int 		res;
	unsigned int 	diff;

/* 
 * Build the device name associated with this server 
 */
	snprintf(serv1, sizeof(serv1), "%s/%d", tmp_name, serv_info[min].numb);
	
/* 
 * Ask the static database for this server network parameters (host_name,
 * program number and version number) 
 */
	tmp_ptr = serv1;

	if (db_dev_import(&tmp_ptr,&serv_net_ptr,1,&error))
	{
		*perr = DcErr_CantGetDcServerNetInfo;
		return(DS_NOTOK);
	}

/* 
 * Remove the .esrf.fr at the end of the host name (if any) 
 */
#ifdef OSK
	if ((tmp1 = index(serv_net_ptr[0].host_name,'.')) != NULL)
#else
	if ((tmp1 = strchr(serv_net_ptr[0].host_name,'.')) != NULL)
#endif /* OSK */
	{
		diff = (u_int)(tmp1 - serv_net_ptr[0].host_name);
		serv_net_ptr[0].host_name[diff] = 0;
	}

/* 
 * Build the RPC connection to the data collector server 
 */
	cl_write = clnt_create(serv_net_ptr[0].host_name,serv_net_ptr[0].pn,serv_net_ptr[0].vn,"tcp");
	free(serv_net_ptr);
	if (cl_write == NULL)
	{
		*perr = DcErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}

/* 
 * Test the server 
 */
	ret_str = &(ret_array[0]);
	res = dc_rpcwr_check_clnt_1(cl_write,&ret_str,&error);

/* 
 * If error, destroy the connection 
 */
	if (res == DS_NOTOK)
	{
		clnt_destroy(cl_write);
		*perr = error;
		return(DS_NOTOK);
	}
/* 
 * The server answers correctly, so connect the process to it 
 */
	else
	{
		cl = cl_write;
		strncpy(tmp_name,serv1, sizeof(tmp_name));
	}

/* 
 * Leave function 
 */
	return(DS_NOTOK);
}




/**@ingroup dcAPIintern
 * Get the data collector name where this client must be connected
 * 
 * @param p_serv_name 	The address of a pointer where the dc host name must be copied.
 * @param perr 		The address of the error variable
 * 
 * @return DS_OK when no problem occurs. Otherwise the return value is DS_NOTOK 
 *	and the error variable is set according to the error
 */
int get_dc_host(char **p_serv_name,long *perr)
{
	static char 		hostna[32];
	char 			dcdev_name[40],
				res_name[32],
				*def_name,
				*tmp_ptr;
	struct hostent 		*host;
	db_devinf_imp 		*dc_net;
	long 			error;
	unsigned char 		ho = 0,
				net = 0;
	static db_resource 	res_def[1];

/* 
 * Miscellaneous init. 
 */
	res_name[0] = dcdev_name[0] = 0;

/* 
 * Get host name and host network IP number 
 */
	gethostname(hostna,sizeof(hostna));
	if ((host = gethostbyname(hostna)) == NULL)
	{
		*perr = DcErr_CantGetDcHostInfo;
		return(DS_NOTOK);
	}
	ho = (unsigned char)host->h_addr[3];
	net = (unsigned char)host->h_addr[2];

/* 
 * Build the name used by the data collector if one exist on this host 
 */
	snprintf(dcdev_name, sizeof(dcdev_name), "sys/dc_wr_i%u/1", ho);

/* 
 * Ask to the static database if a data collector is running on this host 
 */
	tmp_ptr = dcdev_name;
	if (db_dev_import(&tmp_ptr,&dc_net,1,&error))
	{
		if (error == DbErr_DeviceNotDefined || error == DbErr_DeviceNotExported)
		{
/* 
 * No data collector is running on this host, take the default one which is
 * defined as a resource. There is one default data collector host for every
 * ctrl network 
 */
			snprintf(res_name, sizeof(res_name), "%u_default",net);
			def_name = NULL;
			res_def[0].resource_name = res_name;
			res_def[0].resource_type = D_STRING_TYPE;
			res_def[0].resource_adr = &def_name;
			if (db_getresource("class/dc/1",res_def,1,&error))
			{
				free(dc_net);
				*perr = DcErr_CantGetDcResources;
				return(DS_NOTOK);
			}
			if (def_name == NULL)
			{
				free(dc_net);
				*perr = DcErr_NoDefaultDcForThisHost;
				return(DS_NOTOK);
			}
			else
			{
				strcpy(hostna,def_name);
				free(def_name);
			}
		}
		else
		{
			*perr = DcErr_CantGetDcServerNetInfo;
			return(DS_NOTOK);
		}
	}

/* 
 * The db_dev_import is successful, so a data colector is running on this host 
 */
#ifdef DEBUG
	printf("Dc host : %s\n",hostna);
#endif /* DEBUG */

	free(dc_net);
	*p_serv_name = hostna;
	return(DS_OK);
}





/**@ingroup dcAPIintern
 * Ask the static database for every dc write servers request number, to verify 
 * that this task is connected with the less heavily loaded server and if it is 
 * not the case, to connect this task to the less heavily loaded dc write server.
 *
 * @param perr The address of the error variable
 *
 * @return DS_OK when no problem occurs. Otherwise the return value is DS_NOTOK 
 * and the error variable is set according to the error
 */
static int rpc_reconnect(long *perr)
{
	int 			i,
				res,
				nb_server;
	long 			error;
	serv 			serv_info[10];
	static db_resource 	res_tab1[] = {
					{"1",D_LONG_TYPE},
					{"2",D_LONG_TYPE},
					{"3",D_LONG_TYPE},
					{"4",D_LONG_TYPE},
					{"5",D_LONG_TYPE},
					{"6",D_LONG_TYPE},
					{"7",D_LONG_TYPE},
					{"8",D_LONG_TYPE},
					{"9",D_LONG_TYPE},
					{"10",D_LONG_TYPE},
					};

/* 
 * Fullfil the resource array 
 */
	for (i = 0;i < 10;i++)
	{
		serv_info[i].request = 0xFFFFFFFF;
		res_tab1[i].resource_adr = &(serv_info[i].request);
		serv_info[i].numb = i + 1;
	}

/* 
 * Get resource values from database 
 */
	if (db_getresource(psd_name,res_tab1,10,&error))
	{
		*perr = DcErr_CantGetDcResources;
		return(DS_NOTOK);
	}

/* 
 * Find how many servers are defined 
 */
	for (i = 0;i < 10;i++)
		if (serv_info[i].request == 0xFFFFFFFF)
			break;
	nb_server = i;

/* 
 * Sort the serv_info table in the ascending order 
 */

#ifndef _NT
	qsort(&(serv_info[0]),nb_server,sizeof(serv),(int (*)())comp);
#else
	qsort(&(serv_info[0]),nb_server,sizeof(serv),comp);
#endif   /* _NT */

/* 
 * Test every server and keep the connection with the first one which answers 
 */
	for (i = 0;i < nb_server;i++)
	{
		res = re_test_server(serv_info,i,nb_server,&error);
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
 * Test a data collector server and to if the test is  successful, to switch 
 * the calling process to this server
 *
 * @param serv_info 	the array of structures with one structure per server. The 
 *			server number of request is one element of this structure.
 * @param min		The element in the structure array to take into account
 * @param nb_server	The number of server defined on this host
 * @param perr 		The address of the error variable
 *
 * @return DS_OK when no problem occurs. Otherwise the return value is DS_NOTOK 
 *	and the error variable is set according to the error
 */
static int re_test_server(serv *serv_info,int min,int nb_server,long *perr)
{
	char 		serv1[40],
	 		*tmp_ptr,
			*ret_str,
			ret_array[40],
	 		*tmp1;
	int 		old,
			res,
			already_con = False;
	db_devinf_imp 	*serv_net_ptr;
	long 		error;
	CLIENT 		*cl_write;
	unsigned int 	diff;

/* 
 * If the server to test is the one on which we are connected, no need to
 * build a new rpc connection 
 */
	old = (int)(tmp_name[strlen(tmp_name) - 1]) - 0x30;
	if (old == serv_info[min].numb)
	{
		cl_write = cl;
		already_con = True;
	}
	else
	{
/* 
 * Build the device name associated with this server 
 */
		snprintf(serv1, sizeof(serv1), "%s%u", tmp_name, serv_info[min].numb);
	
/* 
 * Ask the static database for this server network parameters (host_name, program number and version number) 
 */
		tmp_ptr = serv1;
		if (db_dev_import(&tmp_ptr,&serv_net_ptr,1,&error))
		{
			*perr = DcErr_CantGetDcServerNetInfo;
			return(DS_NOTOK);
		}

/* 
 * Remove the .esrf.fr at the end of the host name (if any) 
 */

#ifdef OSK
		if ((tmp1 = index(serv_net_ptr[0].host_name,'.')) != NULL)
#else
		if ((tmp1 = strchr(serv_net_ptr[0].host_name,'.')) != NULL)
#endif /* OSK */
		{
			diff = (u_int)(tmp1 - serv_net_ptr[0].host_name);
			serv_net_ptr[0].host_name[diff] = 0;
		}

/* 
 * Build the RPC connection to the data collector server 
 */
		cl_write = clnt_create(serv_net_ptr[0].host_name,serv_net_ptr[0].pn,serv_net_ptr[0].vn,"tcp");
		free(serv_net_ptr);
		if (cl_write == NULL)
		{
			*perr = DcErr_CannotCreateClientHandle;
			return(DS_NOTOK);
		}
	}

/* 
 * Test the server 
 */
	ret_str = &(ret_array[0]);
	res = dc_rpcwr_check_clnt_1(cl_write,&ret_str,&error);

/* 
 * If error, destroy the connection 
 */
	if (res == -1)
	{
		if (already_con == False)
			clnt_destroy(cl_write);
		if (min == (nb_server - 1))
			tmp_name[strlen(tmp_name) - 1] = '0';
		*perr = error;
		return(DS_NOTOK);
	}
/* 
 * The server answers correctly, so connect the process to it 
 */
	else
	{
		if (already_con == False)
		{
			clnt_destroy(cl);
			cl = cl_write;
			strcpy(tmp_name,serv1);
		}
		err_count = 0;
		*perr = 0;
	}
/* 
 * Leave function 
 */
	return(0);

}

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
 *		$Author: andy_gotz $
 *
 * Original   : January 1991
 *
 * Version    :	$Revision: 1.4 $
 *
 * Date       :	$Date: 2008-10-22 10:38:44 $
 * 
 *-*******************************************************************/
using namespace std;
#include <tango.h>

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#define PORTMAP

#include <macros.h>
#include <db_setup.h>
#include <db_xdr.h>
#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>

#include "taco_utils.h"

#if defined(WIN32)
#	include <rpc.h>
#	if 0
#		include <nm_rpc.h>
#	endif
#else
#	include <string.h>
#	if HAVE_SYS_SOCKET_H
#		include <sys/socket.h>
#	else
#		include <socket.h>
#	endif
#	if HAVE_NETDB_H
#		include <netdb.h>
#	else
#		include <hostLib.h>
#		include <taskLib.h>
#	endif
#	if HAVE_UNISTD_H
#		include <unistd.h>
#	endif
#endif	/* WIN32 */

#include <math.h>


#include <vector>
#include <string>

extern dbserver_info db_info;
extern configuration_flags *config_flags;
extern msgserver_info msg_info;

/* Static and Global variables */

extern int func_ptr;
static devexp_res *tab_clstu;

static int first_devexp = 0;
static int first_tcp_call = 0;
static struct sockaddr_in serv_adr;
#ifndef vxworks
static struct hostent *ht;
#else
static int host_addr;
#endif

static long get_tango_exp_devices(char *filter,std::vector<std::string> &dev_name_list,long *perr);
extern "C" int test_star(char *filter);
 

/**
 * This function allows a user to get the name of exported (and then ready to accept
 * command) devices for Taco and Tango. With the filter parameter, it is possible to limit the devices
 * name return by the function.
 *
 * @param filter	A filter to select the exported device names.
 * @param tab		the string's array containing the found exported devices
 * @param num_dev	The number of exported devices
 * @param perr		The error code in case of problems
 *
 * @return   In case of trouble, the function returns DS_NOTOK and set the err varaible
 *    pointed to by "perr". Otherwise, the function returns DS_OK
 */
int _DLLFunc db_getdevexp_tango(char *filter, char ***tab, u_int *num_dev,long *perr)
{
	char *filter1;
	register db_res *recev;
	int i,j,k,tp;
	CLIENT *tcp_cl;
	int tcp_so;
	int tcp_used = 0;
	DevLong error;
	struct timeval tout;

        if (config_flags->no_database)
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

	recev = db_getdevexp_1(&filter1,db_info.conf->clnt,&error);	
	
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

				recev = db_getdevexp_1(&filter1,db_info.conf->clnt,&error);

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
#ifndef vxworks
				if ((ht = gethostbyname(db_info.conf->server_host)) == NULL)
				{
#else  /* !vxworks */
				if ((host_addr = hostGetByName(db_info.conf->server_host)) == 0) {
#endif /* !vxworks */
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
			tcp_cl = clnttcp_create(&serv_adr,
					        db_info.conf->prog_number,
					        DB_VERS_3,&tcp_so,0,0);

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
		tab_clstu[func_ptr].cl = db_info.conf->clnt;
		tab_clstu[func_ptr].tcp = 0;
		tab_clstu[func_ptr].res.res_val.arr1_len = full_dev_nb;
		tab_clstu[func_ptr].res.res_val.arr1_val = *tab;
	}
	func_ptr++;

/* No error */

	*perr = DS_OK;
	return(DS_OK);

}

/**
 * The @ref db_getdevexp_tango function can return a lot of device names and allocate memory
 * to store them. This call is a local call and frees all the memory allocated by the
 * db_getdevexp_tango funtion.
 *
 * @param ptr The pointer to the array of exported device name strings.
 *   This pointer must have been initialized by a db_getdevexp function.
 *
 * @return DS_OK if no errors occurs or DS_NOTOK if a problem occurs
 * 
 * @see db_getdevexp_tango
 */
int _DLLFunc db_freedevexp_tango(char **ptr)
{
	register int i;
	int l;

        if (config_flags->no_database)
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

		for (l = 0;l < tab_clstu[i].res.res_val.arr1_len;l++)
			delete [] ptr[l];
		delete [] ptr;


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
 
		for (l = 0;l < tab_clstu[i].res.res_val.arr1_len;l++)
			delete ptr[l];
	}

	return(DS_OK);
	
}

/**
 * Get the list of exported devices from the Tango database
 *
 * @param filter	A filter to select the exported device names.
 * @param dev_name_list		the vector of strings containing the found exported devices
 * @param perr		The error code in case of problems
 *
 * @return   In case of trouble, the function returns DS_WARNING 
 *           Otherwise, the function returns DS_OK
 */
static long get_tango_exp_devices(char *filter,std::vector<std::string> &dev_name_list,long *perr)
{
	std::string filt(filter);

	try
	{
	
//
// Connect to TANGO database
//
	
		Tango::Database *db = new Tango::Database();

//
// Get Tango device list
//
	
		Tango::DbDatum list = db->get_device_exported(filt);
	
		list >> dev_name_list;
	}
	catch (Tango::DevFailed &e)
	{
		return 1;
	}

//
// Add "tango::" in front of each device name
//
	
	int i;
	int nb_dev = dev_name_list.size();
	for (i = 0;i < nb_dev;i++)
	{
		dev_name_list[i].insert(0,"tango:");
	}
	
	return 0;
}



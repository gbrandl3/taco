static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/utils_cli.c,v 1.1 2003-03-18 16:16:34 jkrueger1 Exp $";

/*

 Author(s):     Emmanuel Taurel
                $Author: jkrueger1 $

 Original:      1993

 Version:       $Revision: 1.1 $

 Date:          $Date: 2003-03-18 16:16:34 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

 *  */

#include <API.h>
#include <ApiP.h>
#include <DevErrors.h>

#include <dc.h>
#include <dcP.h>
#include <dc_xdr.h>

#ifndef _OSK
#include <stdlib.h> 
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



/****************************************************************************
*                                                                           *
*		dc_info function code                                       *
*               -------                                                     *
*                                                                           *
*    Function rule : To ask to a data collector system general information  *
*		     This function is mainly used by the dc_info command    *
*                                                                           *
*    Argins : - serv_name : The dc host name				    *
*                                                                           *
*    Argout : - dc_inf : Pointer to where function result will be stored    *
*	      - error : Pointer for error code				    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the err variable   *
*    pointed to by "perr". Otherwise, the function returns 0                *
*                                                                           *
*****************************************************************************/


#ifdef __STDC__
int dc_info(char *serv_name,servinf *dc_inf,long *error)
#else
int dc_info(serv_name,dc_inf,error)
char *serv_name;
servinf *dc_inf;
long *error;
#endif /* __STDC__ */
{
	long err;
	int send;
	dc_infox_back *recev;
	struct hostent *host;
	unsigned char tmp = 0;
	char dev_name[40];
	char *tmp_ptr;
	db_devinf_imp *serv_net;
	CLIENT *cl_info;
	int i;
	unsigned int diff;
	char *tmp1;

/* Try to verify function parameters */

	if (serv_name == NULL || dc_inf == NULL || error == NULL)
	{
		*error = DcErr_BadParameters;
		return(-1);
	}

/* Miscellaneous initialisation */

	for (i = 0;i < MAX_DOM;i++)
	{
		dc_inf->dom_array[i].dom_nb_dev = 0;
		dc_inf->dom_array[i].dom_name[0] = 0;
	}

/* If the RPC connection to static database server is not built, build one.
   The "config_flags" variable is defined as global by the device server
   API library. */

	if (config_flags.database_server != True)
	{
		if (db_import(&err))
		{
			*error = DcErr_CantBuildStaDbConnection;
			return(-1);
		}
	}

/* Get data collector server host network parameters */

	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*error = DcErr_CantGetDcHostInfo;
		return(-1);
	}
	tmp = (unsigned char)host->h_addr[3];

/* Build the device name associated with the FIRST write server on the
   specified host */

	strcpy(dev_name,"sys/dc_wr_");
	sprintf(&(dev_name[strlen(dev_name)]),"%u",tmp);
	strcat(dev_name,"/1");

/* Ask the static database for this server network parameters */

	tmp_ptr = dev_name;
	if (db_dev_import(&tmp_ptr,&serv_net,1,&err))
	{
		*error = DcErr_CantGetDcServerNetInfo;
		return(-1);
	}

/* Remove the .esr.fr at the end of host name (if any) */

#ifdef OSK
        if ((tmp1 = index(serv_net[0].host_name,'.')) != NULL)
        {
                diff = (u_int)(tmp1 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }
#else
        if ((tmp1 = strchr(serv_net[0].host_name,'.')) != NULL)
        {
                diff = (u_int)(tmp1 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }
#endif /* OSK */

/* Build the RPC connection to the dc server */

	cl_info = clnt_create(serv_net[0].host_name,serv_net[0].pn,serv_net[0].vn,"tcp");
	if (cl_info == NULL)
	{
		free(serv_net);
		*error = DcErr_CannotCreateClientHandle;
		return(-1);
	}
	free(serv_net);

/* Call server */

	recev = dc_info_1(&send,cl_info,&err);

/* Any problem with data transfer ? */

	if (recev == NULL)
	{
		clnt_destroy(cl_info);
		*error = err;
		return(-1);
	}

/* Any problem with data collector access ? */

	if (recev->err_code != 0)
	{
		clnt_destroy(cl_info);
		*error = recev->err_code;
		return(-1);
	}

/* Copy result to caller area */

	dc_inf->free_mem = recev->back.free_mem;
	dc_inf->mem = recev->back.mem;
	dc_inf->nb_dev = recev->back.nb_dev;
	dc_inf->dom_nb = recev->back.dom_ax.dom_ax_len;
	for (i = 0;i < dc_inf->dom_nb;i++)
	{
		dc_inf->dom_array[i].dom_nb_dev = recev->back.dom_ax.dom_ax_val[i].dom_nb_dev;
		strcpy(dc_inf->dom_array[i].dom_name,recev->back.dom_ax.dom_ax_val[i].dom_name);
	}

/* Free the memory allocated by XDR and destroy the RPC connection */

	clnt_freeres(cl_info,(xdrproc_t)xdr_dc_infox_back,(char *)recev);
	clnt_destroy(cl_info);

/* No error */

	*error = 0;
	return(0);

}



/****************************************************************************
*                                                                           *
*		dc_devall function code                                     *
*               ---------                                                   *
*                                                                           *
*    Function rule : To ask to a data collector system the name of all its  *
*		     registered devices					    *
*                                                                           *
*    Argins : - serv_name : The dc host name				    *
*                                                                           *
*    Argout : - devname_tab : Pointer where the device list will be stored  *
*	      - error : Pointer for error code				    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the err variable   *
*    pointed to by "perr". Otherwise, the function returns 0                *
*                                                                           *
*****************************************************************************/


#ifdef __STDC__
int dc_devall(char *serv_name,char ***devnametab,int *dev_n,long *error)
#else
int dc_devall(serv_name,devnametab,dev_n,error)
char *serv_name;
char ***devnametab;
int *dev_n;
long *error;
#endif /* __STDC__ */
{
	long err;
	int send,nb_dev;
	int i,j;
	dc_devallx_back *recev;
	struct hostent *host;
	unsigned char tmp = 0;
	char dev_name[40];
	char *tmp_ptr;
	db_devinf_imp *serv_net;
	CLIENT *cl_info;
	unsigned int diff;
	char *tmp1;

/* Try to verify function parameters */

	if (serv_name == NULL || devnametab == NULL || error == NULL)
	{
		*dev_n = 0;
		*error = DcErr_BadParameters;
		return(-1);
	}

/* If the RPC connection to static database server is not built, build one.
   The "config_flags" variable is defined as global by the device server
   API library. */

	if (config_flags.database_server != True)
	{
		if (db_import(&err))
		{
			*dev_n = 0;
			*error = DcErr_CantBuildStaDbConnection;
			return(-1);
		}
	}

/* Get data collector server host network parameters */

	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*dev_n = 0;
		*error = DcErr_CantGetDcHostInfo;
		return(-1);
	}
	tmp = (unsigned char)host->h_addr[3];

/* Build the device name associated with the FIRST write server on the
   specified host */

	strcpy(dev_name,"sys/dc_wr_");
	sprintf(&(dev_name[strlen(dev_name)]),"%u",tmp);
	strcat(dev_name,"/1");

/* Ask the static database for this server network parameters */

	tmp_ptr = dev_name;
	if (db_dev_import(&tmp_ptr,&serv_net,1,&err))
	{
		*dev_n = 0;
		*error = DcErr_CantGetDcServerNetInfo;
		return(-1);
	}

/* Remove the .esr.fr at the end of host name (if any) */

#ifdef OSK
        if ((tmp1 = index(serv_net[0].host_name,'.')) != NULL)
        {
                diff = (u_int)(tmp1 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }
#else
        if ((tmp1 = strchr(serv_net[0].host_name,'.')) != NULL)
        {
                diff = (u_int)(tmp1 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }
#endif /* OSK */

/* Build the RPC connection to the dc server */

	cl_info = clnt_create(serv_net[0].host_name,serv_net[0].pn,serv_net[0].vn,"tcp");
	if (cl_info == NULL)
	{
		free(serv_net);
		*dev_n = 0;
		*error = DcErr_CannotCreateClientHandle;
		return(-1);
	}
	free(serv_net);

/* Call server */

	recev = dc_devall_1(&send,cl_info,&err);

/* Any problem with data transfer ? */

	if (recev == NULL)
	{
		clnt_destroy(cl_info);
		*dev_n = 0;
		*error = err;
		return(-1);
	}

/* Any problem with data collector access ? */

	if (recev->err_code != 0)
	{
		clnt_destroy(cl_info);
		*dev_n = 0;
		*error = recev->err_code;
		return(-1);
	}

/* Allocate memory for the caller result */

	nb_dev = recev->dev_name.name_arr_len;
	if ((*devnametab = (char **)calloc(nb_dev,sizeof(char *))) == NULL)
	{
		clnt_freeres(cl_info,(xdrproc_t)xdr_dc_infox_back,(char *)recev);
		clnt_destroy(cl_info);
		*dev_n = 0;
		*error = DcErr_ClientMemoryAllocation;
		return(-1);
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
			return(-1);
		}
	}

/* Copy result to caller area */

	for (i = 0;i < nb_dev;i++)
		strcpy((*devnametab)[i],recev->dev_name.name_arr_val[i]);
	*dev_n = nb_dev;

/* Free the memory allocated by XDR and destroy the RPC connection */

	clnt_freeres(cl_info,(xdrproc_t)xdr_dc_devallx_back,(char *)recev);
	clnt_destroy(cl_info);

/* No error */

	*error = 0;
	return(0);

}



/****************************************************************************
*                                                                           *
*		dc_dinfo function code                                      *
*               --------                                                    *
*                                                                           *
*    Function rule : To ask to a data collector system information about a  *
*		     specific device. These information are :		    *
*			- The command used for polling and their argument   *
*			  types		    				    *
*			- The time needed to execute the command	    *
*			- The polling interval				    *
*			- Address of pointers in the shared memory segments *
*			- Time between the last five polling		    *
*                                                                           *
*    Argins : - dev_name : The device name				    *
*                                                                           *
*    Argout : - dc_dev_info : Pointer to a structure where all the infor.   *
*			      will be stored				    *
*             - error : Pointer to error code (in case of)		    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the err variable   *
*    pointed to by "perr". Otherwise, the function returns 0                *
*                                                                           *
*****************************************************************************/


#ifdef __STDC__
int dc_dinfo(char *dev_name,dc_devinf *dc_dev_info,long *error)
#else
int dc_dinfo(dev_name,dc_dev_info,error)
char *dev_name;
dc_devinf *dc_dev_info;
long *error;
#endif /* __STDC__ */
{
	long err;
	int i,ret,l;
	DevVarStringArray host_dc;
	db_resource res_tab;
	long nethost_defined = False;
	char dv_name[40];
	char nethost[40];

/* Try to verify function parameters */

	if (dev_name == NULL || dc_dev_info == NULL || error == NULL)
	{
		*error = DcErr_BadParameters;
		return(-1);
	}

/* Check device name (correct number of /) with or without nethost defined */

	l = 0;
	if (dev_name[0] == '/')
	{
		NB_CHAR(l,dev_name,'/');
		if (l != 5)
		{
			*error = DcErr_BadParameters;
			return(-1);
		}
		else
		{
			if (dev_name[1] != '/')
			{
				*error = DcErr_BadParameters;
				return(-1);
			}
			else
			{
				nethost_defined = True;
			}
		}
	}
	else
	{
		NB_CHAR(l,dev_name,'/');
		if (l != 2)
		{	
			*error = DcErr_BadParameters;
			return(-1);
		}
	}

/* Extract nethost name if any */

	if (nethost_defined == True)
	{
		l = strlen(dev_name);
		for (i = 2;i < l;i++)
		{
			if (dev_name[i] == '/')
			{
				break;
			}
			nethost[i - 2] = dev_name[i];
		}
		nethost[i - 2] = '\0';
	}
	else
		nethost[0] ='\0';

/* Get host name where the dc is distributed */

	if (nethost_defined == True)
	{
		strcpy(dv_name,"//");
		strcat(dv_name,nethost);
		strcat(dv_name,"/class/dc/1");
	}
	else
		strcpy(dv_name,"class/dc/1");

        host_dc.length = 0;
	host_dc.sequence = NULL;
        res_tab.resource_name = "host";
        res_tab.resource_type = D_VAR_STRINGARR;
        res_tab.resource_adr = &host_dc;

        if (db_getresource(dv_name,&res_tab,1,&err))
	{
		*error = DcErr_CantGetDcResources;
                return(-1);
	}


/* For each host where a single dc runs, ask if it knows the device */

        for (i = 0;i < (int)host_dc.length;i++)
	{
                ret = dc_devinfo(host_dc.sequence[i],dev_name,dc_dev_info,&err);
                if (ret == -1)
		{
                        if (err != DcErr_DeviceNotDefined)
			{
				*error = err;
                                return(-1);
			}
		}
                if (ret == 0)
                        break;
	}

	if (i == (int)host_dc.length)
	{
		*error = DcErr_DeviceNotDefined;
                return(-1);
	}

/* Free memory allocated by db_getresource */

	for (i = 0;i < (int)host_dc.length;i++)
		free(host_dc.sequence[i]);
	free(host_dc.sequence);
	
/* Leave function */

	return(0);

}



/****************************************************************************
*                                                                           *
*		dc_devinfo function code                                    *
*               ----------                                                  *
*                                                                           *
*    Function rule : To ask to a single data collector system information   *
*                    about a specific device. These information are :	    *
*			- The command used for polling and their argument   *
*			  types		    				    *
*			- The time needed to execute the command	    *
*			- The polling interval				    *
*			- Address of pointers in the shared memory segments *
*			- Time between the last five polling		    *
*                                                                           *
*    Argins : - serv_name : The dc host name				    *
*	      - dev_name : The device name				    *
*                                                                           *
*    Argout : - dc_dev_info : Pointer to structure where command result wil *
*			      be stored					    *
*	      - error : Poniter for error code				    *
*                                                                           *
*    In case of trouble, the function returns -1 and set the err variable   *
*    pointed to by "perr". Otherwise, the function returns 0                *
*                                                                           *
*****************************************************************************/


#ifdef __STDC__
int dc_devinfo(char *serv_name,char *dev_name,dc_devinf *dc_dev_info,long *error)
#else
int dc_devinfo(serv_name,dev_name,dc_dev_info,error)
char *serv_name;
char *dev_name;
dc_devinf *dc_dev_info;
long *error;
#endif /* __STDC__ */
{
	long err;
	int i,l;
	static char *send;
	dc_devinfx_back *recev;
	struct hostent *host;
	unsigned char tmp = 0;
	char dev_name1[40];
	char *tmp_ptr;
	db_devinf_imp *serv_net;
	CLIENT *cl_info;
	cmd_infox *tmp1;
	char *tmp_name;
	char nethost[40];
	unsigned int diff;
	char *tmp2;

/* Get data collector server host network parameters */

	if ((host = gethostbyname(serv_name)) == NULL)
	{
		*error = DcErr_CantGetDcHostInfo;
		return(-1);
	}
	tmp = (unsigned char)host->h_addr[3];

/* Extract nethost name if any */

	if (dev_name[0] == '/')
	{
		l = strlen(dev_name);
		for (i = 2;i < l;i++)
		{
			if (dev_name[i] == '/')
			{
				break;
			}
			nethost[i - 2] = dev_name[i];
		}
		nethost[i - 2] = '\0';
	}
	else
		nethost[0] = '\0';

/* Build the device name associated with the FIRST write server on the
   specified host */

	if (nethost[0] != '\0')
	{
		strcpy(dev_name1,"//");
		strcat(dev_name1,nethost);
		strcat(dev_name1,"/sys/dc_wr_");
	}
	else
		strcpy(dev_name1,"sys/dc_wr_");

	sprintf(&(dev_name1[strlen(dev_name1)]),"%u",tmp);
	strcat(dev_name1,"/1");

/* Ask the static database for this server network parameters */

	tmp_ptr = dev_name1;
	if (db_dev_import(&tmp_ptr,&serv_net,1,&err))
	{
		*error = DcErr_CantGetDcServerNetInfo;
		return(-1);
	}

/* Remove the .esr.fr at the end of host name (if any) */

#ifdef OSK
        if ((tmp2 = index(serv_net[0].host_name,'.')) != NULL)
        {
                diff = (u_int)(tmp2 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }
#else
        if ((tmp2 = strchr(serv_net[0].host_name,'.')) != NULL)
        {
                diff = (u_int)(tmp2 - serv_net[0].host_name);
                serv_net[0].host_name[diff] = 0;
        }
#endif /* OSK */

/* Build the RPC connection to the dc server */

	cl_info = clnt_create(serv_net[0].host_name,serv_net[0].pn,serv_net[0].vn,"tcp");
	if (cl_info == NULL)
	{
		free(serv_net);
		*error = DcErr_CannotCreateClientHandle;
		return(-1);
	}
	free(serv_net);

/* Extract real device name if nethost is specified */

	if (dev_name[0] == '/')
	{
		tmp_name = dev_name + 2;
		for (i = 2;dev_name[i] != '/';i++)
		{
			tmp_name++;
		}
		tmp_name++;
	}
	else
		tmp_name = dev_name;

/* Device name in lower case letters */

	l = strlen(tmp_name);
	if ((send = (char *)malloc(l + 1)) == NULL)
	{
		clnt_destroy(cl_info);
		*error = DcErr_ClientMemoryAllocation;
		return(-1);
	}
	strcpy(send,tmp_name);
	for (i = 0;i < l;i++)
		send[i] = tolower(send[i]);

/* Call server */

	recev = dc_devinfo_1(&send,cl_info,&err);

/* Free memory */

	free(send);

/* Any problem with data transfer ? */

	if (recev == NULL)
	{
		clnt_destroy(cl_info);
		dc_dev_info->devinf_nbcmd = 0;
		*error = err;
		return(-1);
	}

/* Any problem with data collector access ? */

	if (recev->err_code != 0)
	{
		clnt_destroy(cl_info);
		dc_dev_info->devinf_nbcmd = 0;
		*error = recev->err_code;
		return(-1);
	}

/* Copy result to caller area */

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

/* Free the memory allocated by XDR and destroy the RPC connection */

	clnt_freeres(cl_info,(xdrproc_t)xdr_dc_devinfx_back,(char *)recev);
	clnt_destroy(cl_info);

/* No error */

	*error = 0;
	return(0);

}

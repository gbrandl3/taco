/*+*******************************************************************

 File       :   tools_cli.c

 Project    :   Static database

 Description:   Application Programmers Interface

            :   Interface to access static database for all its utilities
                calls

 Author(s)  :   Emmanuel Taurel
                $Author: jkrueger1 $

 Original   :   April 1997

 Version:       $Revision: 1.2 $

 Date:          $Date: 2003-05-16 13:40:27 $

 Copyright (c) 1997 by European Synchrotron Radiation Facility,
                       Grenoble, France


 *-*******************************************************************/

#define PORTMAP

#include <macros.h>
#include <db_setup.h>
#include <db_xdr.h>

#if defined(_NT)
#include <API.h>
#include <ApiP.h>
#include <DevErrors.h>
#include <nm_rpc.h>

#else
#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>

#ifdef _OSK
#include <inet/socket.h>
#include <inet/netdb.h>
#include <strings.h>
#else
#include <string.h>
#include <sys/socket.h>
#ifndef vxworks
#include <netdb.h>
#else
#include <hostLib.h>
#include <taskLib.h>
#endif
#include <unistd.h>
#endif /* _OSK */
#endif	/* _NT */

#ifndef OSK
#include <stdlib.h>
#endif

#include <math.h>

#ifdef ALONE
extern CLIENT *cl;
extern int first;
#else
extern dbserver_info db_info;
#endif /* ALONE */

static CLIENT *cl_tcp;
static int first_tcp = 0;
static struct sockaddr_in serv_adr;
#ifndef vxworks
static struct hostent *ht;
#else
static int host_addr;
#endif

/* Static and Global variables */

static struct timeval timeout_browse={60,0};



/**@ingroup dbaseAPI
 * To get device information from the database	
 *									
 * @param dev_name	Device name
 * @param p_domain_nb 	Pointer for domain number			
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_deviceinfo(const char *dev_name,db_devinfo_call *p_info,long *p_error)
{
	db_devinfo_svc *recev;
	int i,k;
	char *name_sent;
	long error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((p_info == NULL) || (dev_name == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);

#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/* Initialize data sent to server */

	k = strlen(dev_name);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(name_sent,dev_name);
	for(i = 0;i < k;i++)
		name_sent[i] = tolower(name_sent[i]);

/* Call server */

	recev = db_deviceinfo_1(&name_sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* Any problems during database access ? */

	if(recev->db_err != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_devinfo_svc,(char *)recev);
		free(name_sent);
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	free(name_sent);
	
/* Copy data sent back to client into caller structure */

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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_devinfo_svc,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/**@ingroup dbaseAPI
 * To retrieve the list of resources for a given device
 *
 * @param dev_name 	The device name				
 * @param p_family_nb 	Pointer for family number			
 * @param ppp_list 	Pointer for the family name list. Memory is
 *			allocated by this function		    	
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return    	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_deviceres(long dev_nb,char **dev_name_list,long *p_res_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
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

/* Try to verify function parameters */

	if ((ppp_list == NULL) || (p_res_nb == NULL) ||
	    (dev_name_list == NULL) || (dev_nb == 0))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_res_nb = 0;
	*p_error = DS_OK;
		
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
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

/* Initialize data sent to server */

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
		strcpy(sent.res_val.arr1_val[i],dev_name_list[i]);
		for(j = 0;j < k;j++)
			sent.res_val.arr1_val[i][j] = tolower(sent.res_val.arr1_val[i][j]);
	}

/* Call server */

	recev = db_deviceres_1(&sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* Any problems during database access ? */

	if(recev->db_err != DS_OK)
	{
		for (i = 0;i < dev_nb;i++)
			free(sent.res_val.arr1_val[i]);
		free(sent.res_val.arr1_val);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);
		return(DS_NOTOK);
	}

/* Free memory used to send data to server */

	for (i = 0;i < dev_nb;i++)
		free(sent.res_val.arr1_val[i]);
	free(sent.res_val.arr1_val);
		
/* Initialize resource number */

	nb_res = recev->res_val.arr1_len;
	*p_res_nb = nb_res;

/* Allocate memory for resource names array and copy them */

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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	return(exit_code);

}




/**@ingroup dbaseAPI
 * To delete a device (or a pseudo device) from the database						
 *									
 * @param dev_name 	Device name
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_devicedelete(const char *dev_name,long *p_error)
{
	long *recev;
	int i,k;
	char *name_sent;
	long error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if (dev_name == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/* Initialize data sent to server */

	k = strlen(dev_name);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(name_sent,dev_name);
	for(i = 0;i < k;i++)
		name_sent[i] = tolower(name_sent[i]);

/* Call server */

	recev = db_devicedelete_1(&name_sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

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

/* Any problems during database access ? */

	if (*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(name_sent);
		*p_error = *recev;
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	free(name_sent);
	
/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/**@ingroup dbaseAPI
 * To delete all resources belonging to a device list	
 *									
 * @param dev_nb  	The device name number				
 * @param dev_name_list The device name list
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_devicedeleteres(long dev_nb,char **dev_name_list,db_error *p_error)
{
	db_psdev_error *recev;
	int i,j,k;
	db_res sent;
	long error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((dev_name_list == NULL) || (dev_nb == 0))
	{
		p_error->error_code = DbErr_BadParameters;
		p_error->psdev_err = DS_OK;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	p_error->error_code = DS_OK;
	p_error->psdev_err = DS_OK;
	
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/* Initialize data sent to server */


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
		strcpy(sent.res_val.arr1_val[i],dev_name_list[i]);
		for(j = 0;j < k;j++)
			sent.res_val.arr1_val[i][j] = tolower(sent.res_val.arr1_val[i][j]);
	}

/* Sort this device name list */

	kern_sort(sent.res_val.arr1_val,dev_nb);
	
/* Call server */

	recev = db_devicedeleteres_1(&sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

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

/* Any problems during database access ? */

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
	
/* Free memory used to send data to server */

	for (i = 0;i < dev_nb;i++)
		free(sent.res_val.arr1_val[i]);
	free(sent.res_val.arr1_val);
	
/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}


/**@ingroup dbaseAPI
 * To retrieve global database information		
 *
 * @param p_info 	Pointer to the structure where all the info will be
 *		        stored. These info are the number of devices defined in the 
 *			database, the number of resources defined in the database ...
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK 
 */
long db_stat(db_stat_call *p_info,long *p_error)
{
	db_info_svc *recev;
	int i;
	long error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if (p_info == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */


/* Call server */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

	recev = db_stat_1(local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
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

/* Any problems during database access ? */

	if(recev->db_err != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_info_svc,(char *)recev);
		return(DS_NOTOK);
	}

/* Allocate memory  to store domain name and element number */

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
		
/* Copy result to client structure */

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
		
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_info_svc,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/**@ingroup dbaseAPI
 * To unregister a server from the database. This will update all the server 
 * devices has been not exported
 *
 * @param ds_name  	The device server name			
 * @param pers_name 	The device server personal name		
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_servunreg(const char *ds_name,const char *pers_name,long *p_error)
{
	long *recev;
	int i,j,k;
	long error;
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

/* Try to verify function parameters */

	if ((ds_name == NULL) || (pers_name == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
		
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif

/* Initialize data sent to server */

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
	strcpy(sent.res_val.arr1_val[0],ds_name);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(pers_name);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],pers_name);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);
		
/* Call server */

	recev = db_servunreg_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

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

/* Any problems during database access ? */

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
	
/* Free memory used to send data to server */

	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);	

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/**@ingroup dbaseAPI
 * To device server information to the caller. These info are the device list, 
 * the server process name and its pid plus the host name
 *
 * @param ds_name 	The device server name			
 * @param pers_name 	The device server personal name		
 * @param p_inf 	Pointer for the structure with DS info		
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_servinfo(const char *ds_name,const char *pers_name, \
		 db_svcinfo_call *p_inf,long *p_error)
{
	svcinfo_svc *recev;
	int i,j,k;
	long error,tmp,tmp_dev;
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

/* Try to verify function parameters */

	if ((ds_name == NULL) || (pers_name == NULL) || (p_inf == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
	
	p_inf->embedded_server_nb = 0;
	p_inf->server = NULL;
	p_inf->process_name[0] = '\0';
	p_inf->pid  = 0;
	p_inf->host_name[0] = '\0';
	p_inf->program_num = 0;
		
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif

/* Initialize data sent to server */

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
	strcpy(sent.res_val.arr1_val[0],ds_name);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(pers_name);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],pers_name);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);
		
/* Call server */

	recev = db_servinfo_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* Any problems during database access ? */

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

/* Copy received data into caller structure */

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
		
/* Free memory used to send data to server */

	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);
		
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_svcinfo_svc,(char *)recev);
	
/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}


/**@ingroup dbaseAPI
 * To delete all the devices belonging to a device server in the static database				
 *
 * @param ds_name  	The device server name			
 * @param pers_name 	The device server personal name		
 * @param delres_flag 	A flag set to True if device resources should also be deleted				
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_servdelete(const char *ds_name,const char *pers_name, \
		   long delres_flag,long *p_error)
{
	long *recev;
	int i,j,k;
	long error;
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

/* Try to verify function parameters */

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

/* Miscellaneous init. */

	*p_error = DS_OK;
		
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif

/* Initialize data sent to server. The db_err long is used to transfer the
   delete resource flag over the network !! */

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
	strcpy(sent.res_val.arr1_val[0],ds_name);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(pers_name);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],pers_name);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);
		
/* Call server */

	recev = db_servdelete_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

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

/* Any problems during database access ? */

	if(*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = *recev;
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);	

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPI
 * To get poller information from a device name	
 *									
 * @param dev_name  	Device name
 * @param poll 		Pointer to the structure where poller info will be
 *		       	stored. these info are : ds_name, ds pers. name,
 *		       	host name, process name and PID			
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_getpoller(const char *dev_name,db_poller *poll,long *p_error)
{
	db_poller_svc *recev;
	int i,k;
	char *name_sent;
	long error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((dev_name == NULL) || (poll == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/* Initialize data sent to server */

	k = strlen(dev_name);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(name_sent,dev_name);
	for(i = 0;i < k;i++)
		name_sent[i] = tolower(name_sent[i]);

/* Call server */

	recev = db_getpoll_1(&name_sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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
	
/* Any problems during database access ? */

	if(recev->db_err != DS_OK)
	{
		free(name_sent);
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_poller_svc,(char *)recev);
		return(DS_NOTOK);
	}
		
/* Init caller structure */

	strcpy(poll->server_name,recev->server_name);
	strcpy(poll->personal_name,recev->personal_name);
	strcpy(poll->host_name,recev->host_name);
	strcpy(poll->process_name,recev->process_name);
	poll->pid = recev->pid;
		
/* Free memory used to send data to server */

	free(name_sent);
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_poller_svc,(char *)recev);
		
/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPI
 * To initialize a resource cache for the specified domain
 *									
 * @param domain 	Domain name
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_initcache(const char *domain,long *p_error)
{
	long *recev;
	int i,k;
	char *name_sent;
	long error;
	long exit_code = DS_OK;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if (domain == NULL)
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_error = DS_OK;
			
#ifdef ALONE
/* Create RPC connection if it's the first call */

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
#else
/* Get client time-out value and change it to a larger value more suitable
   for browsing call */
   
   	clnt_control(db_info.conf->clnt,CLGET_TIMEOUT,(char *)&old_tout);
	clnt_control(db_info.conf->clnt,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(db_info.conf->clnt,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
			
#endif /* ALONE */

#ifdef ALONE
	local_cl = cl;
#else
	local_cl = db_info.conf->clnt;
#endif /* ALONE */

/* Initialize data sent to server */

	k = strlen(domain);
	if ((name_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(name_sent,domain);
	for(i = 0;i < k;i++)
		name_sent[i] = tolower(name_sent[i]);

/* Call server */

	recev = db_initcache_1(&name_sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

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

/* If the server is not connected to the database (because a database update
   is just going on), sleep a while (20 mS) and redo the call */

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

/* Any problems during database access ? */

	if (*recev != DS_OK)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		free(name_sent);
		*p_error = *recev;
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	free(name_sent);
	
/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}

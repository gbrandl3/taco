/*+*******************************************************************

 File       :   browse_cli.c

 Project    :   Static database

 Description:   Application Programmers Interface

            :   Interface to access static database for all its browsnig
                calls

 Author(s)  :   Emmanuel Taurel
		$Author: jkrueger1 $

 Original   :   December 1997

 Version    :	$Revision: 1.4 $

 Date	    :	$Date: 2004-03-09 17:02:49 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

 *-*******************************************************************/
#include "config.h"
#define PORTMAP

#include <macros.h>
#include <db_setup.h>
#include <db_xdr.h>

#include <API.h>
#include <DevErrors.h>
#if defined(_NT)
#	include <ApiP.h>
#	include <rpc.h>
#else
#	include <private/ApiP.h>
#	if 0
#		include <ApiP.h>
#	endif
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


/* Static and Global variables */

static struct timeval timeout_browse={60,0};




/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of domain used for all devices defined
 * in the database.
 * 
 * @param p_domain_nb 	Pointer for domain number
 * @param ppp_list  	Pointer for the domain name list. Memory is 
 *			allocated by this function
 * @param p_error 	Pointer for the error code in case of problems 
 *
 * @return In case of trouble, the function returns DS_NOTOK and set the variable
 *    pointed to by p_error. Otherwise, the function returns DS_OK
 *
 */
long db_getdevdomainlist(long *p_domain_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j;
	long error;
	long nb_domain;
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

	if ((ppp_list == NULL) || (p_domain_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_domain_nb = 0;
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

	recev = db_getdevdomain_1(local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_GETDEVDOMAIN,(long)0,DB_UDP,&error);
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
			recev = db_getdevdomain_1(local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Initialize domain name number */

	nb_domain = recev->res_val.arr1_len;
	*p_domain_nb = nb_domain;

/* Allocate memory for domain names array and copy them */

	if ((*ppp_list = (char **)calloc(nb_domain,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		exit_code = DS_NOTOK;
	}
	else 
	{	
		for (i = 0;i < nb_domain;i++)
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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of families for all devices defined in the 
 * database with the first field set to a given domain name.
 * 
 * @param domain 	The domain name
 * @param p_family_nb 	Pointer for family number returned
 * @param ppp_list  	Pointer for the family name list. Memory is 
 *			allocated by this function
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return  	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by p_error. Otherwise, the function returns DS_OK
 */
long db_getdevfamilylist(char *domain,long *p_family_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_family;
	char *dom_sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((ppp_list == NULL) || (p_family_nb == NULL) || (domain == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_family_nb = 0;
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

	k = strlen(domain);
	if ((dom_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(dom_sent,domain);
	for(i = 0;i < k;i++)
		dom_sent[i] = tolower(dom_sent[i]);

/* Call server */

	recev = db_getdevfamily_1(&dom_sent,local_cl,&error);

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&dom_sent,(void **)&recev,&local_cl,
					(int)DB_GETDEVFAMILY,(long)0,DB_UDP,&error);
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
			recev = db_getdevfamily_1(&dom_sent,local_cl,&error);
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
		return(DS_NOTOK);
	}

/* Free memory used to send data to server */

	free(dom_sent);
		
/* Initialize family name number */

	nb_family = recev->res_val.arr1_len;
	*p_family_nb = nb_family;

/* Allocate memory for domain names array and copy them */

	if (nb_family != 0)
	{
		if ((*ppp_list = (char **)calloc(nb_family,sizeof(char *))) == NULL)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = DbErr_ClientMemoryAllocation;
			exit_code = DS_NOTOK;
		}
		else 
		{	
			for (i = 0;i < nb_family;i++)
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

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of members for all devices defined in the database with
 * the first field name set to a given domain and the second field name set to a given family.
 *
 * @param domain 	The domain name
 * @param family 	The family name
 * @param p_member_nb   Pointer for member number returned
 * @param ppp_list  	Pointer for the member name list. Memory is allocated by this function
 * @param p_error 	Pointer for the error code in case of problems 
 *
 * @return    	In case of trouble, the function returns DS_NOTOK and set the variable 
 *    		pointed to by p_error. Otherwise, the function returns DS_OK 
 */
long db_getdevmemberlist(char *domain,char *family,long *p_member_nb, 
			 char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_member;
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

	if ((ppp_list == NULL) || (p_member_nb == NULL) || 
	    (domain == NULL) || (family == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_member_nb = 0;
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
	
	if ((sent.res_val.arr1_val = (char **)calloc(2,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(domain);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[0],domain);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(family);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],family);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);
		
/* Call server */

	recev = db_getdevmember_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_GETDEVMEMBER,(long)0,DB_UDP,&error);
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
			recev = db_getdevmember_1(&sent,local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);	
		
/* Initialize family name number */

	nb_member = recev->res_val.arr1_len;
	*p_member_nb = nb_member;

/* Allocate memory for domain names array and copy them */

	if (nb_member != 0)
	{
		if ((*ppp_list = (char **)calloc(nb_member,sizeof(char *))) == NULL)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = DbErr_ClientMemoryAllocation;
			exit_code = DS_NOTOK;
		}
		else 
		{	
			for (i = 0;i < nb_member;i++)
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

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of domain used for all resources defined in the
 * database.
 * 
 * @param p_domain_nb : Pointer for domain number
 * @param ppp_list : Pointer for the domain name list. Memory is allocated by this function
 * @param p_error : Pointer for the error code in case of problems
 * 
 * @return    	In case of trouble, the function returns DS_NOTOK and set the variable 
 *    		pointed to by p_error. Otherwise, the function returns DS_OK 
 */
long db_getresdomainlist(long *p_domain_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j;
	long error;
	long nb_domain;
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

	if ((ppp_list == NULL) || (p_domain_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_domain_nb = 0;
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

/* Call server */

	recev = db_getresdomain_1(local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_GETRESDOMAIN,(long)0,DB_UDP,&error);
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
			recev = db_getresdomain_1(local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Initialize domain name number */

	nb_domain = recev->res_val.arr1_len;
	*p_domain_nb = nb_domain;

/* Allocate memory for domain names array and copy them */

	if ((*ppp_list = (char **)calloc(nb_domain,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		exit_code = DS_NOTOK;
	}
	else 
	{	
		for (i = 0;i < nb_domain;i++)
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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of families for all resources defined in the 
 * database with the first field name set to a given domain name.
 *
 * @param domain 	The domain name
 * @param p_family_nb 	Pointer for family number
 * @param ppp_list 	Pointer for the family name list. Memory is allocated by this function
 * @param p_error 	Pointer for the error code in case of problems 
 * 
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by p_error. Otherwise, the function returns DS_OK
 */
long db_getresfamilylist(char *domain,long *p_family_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_family;
	char *dom_sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((ppp_list == NULL) || (p_family_nb == NULL) || (domain == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_family_nb = 0;
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

	k = strlen(domain);
	if ((dom_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(dom_sent,domain);
	for(i = 0;i < k;i++)
		dom_sent[i] = tolower(dom_sent[i]);

/* Call server */

	recev = db_getresfamily_1(&dom_sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&dom_sent,(void **)&recev,&local_cl,
					(int)DB_GETRESFAMILY,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			free(dom_sent);
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
			recev = db_getresfamily_1(&dom_sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				free(dom_sent);
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
		free(dom_sent);
		*p_error = recev->db_err;
		return(DS_NOTOK);
	}

/* Free memory used to send data to server */

	free(dom_sent);
		
/* Initialize family name number */

	nb_family = recev->res_val.arr1_len;
	*p_family_nb = nb_family;

/* Allocate memory for domain names array and copy them */

	if ((*ppp_list = (char **)calloc(nb_family,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		exit_code = DS_NOTOK;
	}
	else 
	{	
		for (i = 0;i < nb_family;i++)
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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of members for all resources defined in the database
 * with the first field name set to a given domain and the second field name set to a given family.
 *
 * @param domain 	The domain name
 * @param family 	The family name
 * @param p_member_nb 	Pointer for member number returned
 * @param ppp_list 	Pointer for the member name list. Memory is allocated by this function
 * @param p_error 	Pointer for the error code in case of problems
 * 
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by p_error. Otherwise, the function returns DS_OK
 */
long db_getresmemberlist(char *domain,char *family,long *p_member_nb, \
			 char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_member;
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

	if ((ppp_list == NULL) || (p_member_nb == NULL) || 
	    (domain == NULL) || (family == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_member_nb = 0;
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
	
	if ((sent.res_val.arr1_val = (char **)calloc(2,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(domain);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[0],domain);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(family);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],family);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);
		
/* Call server */

	recev = db_getresmember_1(&sent,local_cl,&error);
	
/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_GETRESMEMBER,(long)0,DB_UDP,&error);
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
			recev = db_getresmember_1(&sent,local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	free(sent.res_val.arr1_val[0]);
	free(sent.res_val.arr1_val[1]);
	free(sent.res_val.arr1_val);	
		
/* Initialize family name number */

	nb_member = recev->res_val.arr1_len;
	*p_member_nb = nb_member;

/* Allocate memory for domain names array and copy them */

	if ((*ppp_list = (char **)calloc(nb_member,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		exit_code = DS_NOTOK;
	}
	else 
	{	
		for (i = 0;i < nb_member;i++)
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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of resources name for all resources defined in the database
 * for a device with a specified domain, family, and member field name.
 *
 * @param domain  	The domain name
 * @param family  	The family name
 * @param member  	The member name
 * @param p_reso_nb  	Pointer for resources number returned
 * @param ppp_list  	Pointer for the member name list. Memory is allocated by this function
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by p_error. Otherwise, the function returns DS_OK 
 */


long db_getresresolist(char *domain,char *family,char *member, \
		       long *p_reso_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_reso;
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

	if ((ppp_list == NULL) || (p_reso_nb == NULL) || 
	    (domain == NULL) || (family == NULL) || (member == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_reso_nb = 0;
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
	sent.res_val.arr1_len = 3;
	
	if ((sent.res_val.arr1_val = (char **)calloc(3,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(domain);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[0],domain);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(family);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],family);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);

	k = strlen(member);
	if ((sent.res_val.arr1_val[2] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[2],member);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[2][i] = tolower(sent.res_val.arr1_val[2][i]);
				
/* Call server */

	recev = db_getresreso_1(&sent,local_cl,&error);	

/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_GETRESRESO,(long)0,DB_UDP,&error);
		}
		if (error != DS_OK)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			for (i = 0; i < 3;i++)
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
			recev = db_getresreso_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				for (i = 0; i < 3;i++)
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
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		for (i = 0; i < 3;i++)
			free(sent.res_val.arr1_val[i]);
		free(sent.res_val.arr1_val);
		*p_error = recev->db_err;
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	for (i = 0; i < 3;i++)
	{
		free(sent.res_val.arr1_val[i]);
	}
	free(sent.res_val.arr1_val);	
		
/* Initialize family name number */

	nb_reso = recev->res_val.arr1_len;
	*p_reso_nb = nb_reso;

/* Allocate memory for domain names array and copy them */

	if (nb_reso != 0)
	{
		if ((*ppp_list = (char **)calloc(nb_reso,sizeof(char *))) == NULL)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = DbErr_ClientMemoryAllocation;
			exit_code = DS_NOTOK;
		}
		else 
		{	
			for (i = 0;i < nb_reso;i++)
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

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of resource values for all resource with
 * a domain, family, member, and name specified in the first four function parameters.
 * Member and resource field name can be set to wildcard (*).
 * Resources are returned in the following syntax:
 *
 * domain/family/member/resource: resource_value
 * 
 * If the wildcard is not used, only one resource value is returned.
 * If the wildcard is used, this call could return many data, use a TCP connection for 
 * this call 
 *
 * @param domain  	The domain name
 * @param family  	The family name
 * @param member  	The member name
 * @param resource 	The resource name
 * @param p_rval_nb  	Pointer for resources number
 * @param ppp_list  	Pointer for the resource value list. Memory is allocated by this function
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable 
 *    		pointed to by p_error. Otherwise, the function returns DS_OK 
 */
long db_getresresoval(char *domain,char *family,char *member,char *resource, \
		      long *p_rval_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_reso;
	db_res sent;
	CLIENT *local_cl;
	CLIENT *cl_tcp;
	struct sockaddr_in serv_adr;
	int tcp_so;
#ifndef vxworks
	static struct hostent *ht;
#else
	static int host_addr;
#endif
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((ppp_list == NULL) || (p_rval_nb == NULL) || 
	    (domain == NULL) || (family == NULL) || (member == NULL) ||
	    (resource == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_rval_nb = 0;
	*p_error = DS_OK;

/* Create RPC TCP connection */

#ifdef ALONE
	cl_tcp = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"tcp");
	if (cl_tcp == NULL)
	{
		*p_error = DbErr_CannotCreateClientHandle;
		return(DS_NOTOK);
	}
	clnt_control(cl_tcp,CLSET_TIMEOUT,(char *)&timeout_browse);
	clnt_control(cl_tcp,CLSET_RETRY_TIMEOUT,(char *)&timeout_browse);
#else /* ALONE */			
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

#endif /* ALONE */

	local_cl = cl_tcp;

/* Initialize data sent to server */

	sent.db_err = DS_OK;
	sent.res_val.arr1_len = 4;
	
	if ((sent.res_val.arr1_val = (char **)calloc(4,sizeof(char *))) == NULL)
	{
		clnt_destroy(cl_tcp);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	
	k = strlen(domain);
	if ((sent.res_val.arr1_val[0] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_destroy(cl_tcp);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[0],domain);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[0][i] = tolower(sent.res_val.arr1_val[0][i]);

	k = strlen(family);
	if ((sent.res_val.arr1_val[1] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_destroy(cl_tcp);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[1],family);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[1][i] = tolower(sent.res_val.arr1_val[1][i]);

	k = strlen(member);
	if ((sent.res_val.arr1_val[2] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_destroy(cl_tcp);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[2],member);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[2][i] = tolower(sent.res_val.arr1_val[2][i]);
		
	k = strlen(resource);
	if ((sent.res_val.arr1_val[3] = (char *)malloc(k + 1)) == NULL)
	{
		clnt_destroy(cl_tcp);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(sent.res_val.arr1_val[3],resource);
	for(i = 0;i < k;i++)
		sent.res_val.arr1_val[3][i] = tolower(sent.res_val.arr1_val[3][i]);
				
/* Call server */

	recev = db_getresresoval_1(&sent,local_cl,&error);
	
/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&sent,(void **)&recev,&local_cl,
					(int)DB_GETRESRESOVAL,(long)0,
					DB_TCP,&error);
		}
		if (error != DS_OK)
		{
			clnt_destroy(cl_tcp);
			for (i = 0; i < 4;i++)
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
			recev = db_getresresoval_1(&sent,local_cl,&error);
			if(recev == NULL)
			{
				clnt_destroy(local_cl);
				for (i = 0; i < 4;i++)
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
		*p_error = recev->db_err;
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);
		clnt_destroy(local_cl);
		for (i = 0; i < 4;i++)
			free(sent.res_val.arr1_val[i]);
		free(sent.res_val.arr1_val);
		return(DS_NOTOK);
	}
	
/* Free memory used to send data to server */

	for (i = 0; i < 4;i++)
	{
		free(sent.res_val.arr1_val[i]);
	}
	free(sent.res_val.arr1_val);	
		
/* Initialize resource value number */

	nb_reso = recev->res_val.arr1_len;
	*p_rval_nb = nb_reso;

/* Allocate memory for domain names array and copy them */

	if (nb_reso != 0)
	{
		if ((*ppp_list = (char **)calloc(nb_reso,sizeof(char *))) == NULL)
		{
			clnt_destroy(local_cl);
			*p_error = DbErr_ClientMemoryAllocation;
			exit_code = DS_NOTOK;
		}
		else 
		{	
			for (i = 0;i < nb_reso;i++)
			{
				if (((*ppp_list)[i] = (char *)malloc(strlen(recev->res_val.arr1_val[i]) + 1)) == NULL)
				{
					for (j = 0;j < i;j++)
						free((*ppp_list)[j]);
					free(*ppp_list);
					clnt_destroy(local_cl);
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

	clnt_destroy(local_cl);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of device server executeable name.
 * 
 * @param p_server_nb  	Pointer for server number returned
 * @param ppp_list  	Pointer for the server name list. Memory is allocated by this function
 * @param p_error  	Pointer for the error code in case of problems
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable 
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_getdsserverlist(long *p_server_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j;
	long error;
	long nb_server;
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

	if ((ppp_list == NULL) || (p_server_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_server_nb = 0;
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

/* Call server */

	recev = db_getdsserver_1(local_cl,&error);
	
/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_GETSERVER,(long)0,DB_UDP,&error);
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
			recev = db_getdsserver_1(local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Initialize domain name number */

	nb_server = recev->res_val.arr1_len;
	*p_server_nb = nb_server;

/* Allocate memory for domain names array and copy them */

	if ((*ppp_list = (char **)calloc(nb_server,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		exit_code = DS_NOTOK;
	}
	else 
	{	
		for (i = 0;i < nb_server;i++)
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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of devices server personal name for device server
 * with a given executeable name.
 * 
 * @param server 	The device server name
 * @param p_pers_nb 	Pointer for personal name number
 * @param ppp_list 	Pointer for the personal name list. Memory is allocated by this function
 * @param p_error 	Pointer for the error code in case of problems
 *
 * @return    	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_getdspersnamelist(char *server,long *p_pers_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j,k;
	long error;
	long exit_code = DS_OK;
	long nb_pers;
	char *serv_sent;
	struct timeval old_tout;
	CLIENT *local_cl;
#ifndef _OSK
	struct timeval tout;
#endif /* _OSK */
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Try to verify function parameters */

	if ((ppp_list == NULL) || (p_pers_nb == NULL) || (server == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_pers_nb = 0;
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

	k = strlen(server);
	if ((serv_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(serv_sent,server);
	for(i = 0;i < k;i++)
		serv_sent[i] = tolower(serv_sent[i]);

/* Call server */

	recev = db_getdspers_1(&serv_sent,local_cl,&error);
	
/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)&serv_sent,(void **)&recev,&local_cl,
					(int)DB_GETPERS,(long)0,DB_UDP,&error);
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
			recev = db_getdspers_1(&serv_sent,local_cl,&error);
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
		return(DS_NOTOK);
	}

/* Free memory used to send data to server */

	free(serv_sent);
		
/* Initialize family name number */

	nb_pers = recev->res_val.arr1_len;
	*p_pers_nb = nb_pers;

/* Allocate memory for domain names array and copy them */

	if (nb_pers != 0)
	{
		if ((*ppp_list = (char **)calloc(nb_pers,sizeof(char *))) == NULL)
		{
			clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*p_error = DbErr_ClientMemoryAllocation;
			exit_code = DS_NOTOK;
		}
		else 
		{	
			for (i = 0;i < nb_pers;i++)
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

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}




/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of host name where device server should run.
 *
 * @param p_host_nb  	Pointer for host number returned
 * @param ppp_list  	Pointer for the host name list. Memory is allocated by this function
 * @param p_error  	Pointer for the error code in case of problems 
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_gethostlist(long *p_host_nb,char ***ppp_list,long *p_error)
{
	db_res *recev;
	int i,j;
	long error;
	long nb_host;
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

	if ((ppp_list == NULL) || (p_host_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_host_nb = 0;
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

/* Call server */

	recev = db_gethost_1(local_cl,&error);
	
/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_GETHOST,(long)0,DB_UDP,&error);
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
			recev = db_gethost_1(local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Initialize domain name number */

	nb_host = recev->res_val.arr1_len;
	*p_host_nb = nb_host;

/* Allocate memory for domain names array and copy them */

	if ((*ppp_list = (char **)calloc(nb_host,sizeof(char *))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		exit_code = DS_NOTOK;
	}
	else 
	{	
		for (i = 0;i < nb_host;i++)
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
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_res,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}



/**@ingroup dbaseAPIbrowse
 * This function returns to the caller a list of device server which should run on the specified host.
 *
 * @param host  	The host name
 * @param p_ds_nb  	Pointer or the device server list number returned
 * @param ds_list  	Pointer for the ds info structure array
 * @param p_error  	Pointer for the error code in case of problems 
 *
 * @return   	In case of trouble, the function returns DS_NOTOK and set the variable
 *    		pointed to by "p_error". Otherwise, the function returns DS_OK
 */
long db_getdsonhost(char *host, long *p_ds_nb, db_svc **ds_list,long *p_error)
{
	db_svcarray_net *recev;
	char *host_sent;
	int i,j,k;
	long error;
	long nb_ds;
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

	if ((ds_list == NULL) || (p_ds_nb == NULL))
	{
		*p_error = DbErr_BadParameters;
		return(DS_NOTOK);
	}

/* Miscellaneous init. */

	*p_ds_nb = 0;
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

	k = strlen(host);
	if ((host_sent = (char *)malloc(k + 1)) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	strcpy(host_sent,host);
	for(i = 0;i < k;i++)
		host_sent[i] = tolower(host_sent[i]);

/* Call server */

	recev = db_getdsonhost_1(&host_sent,local_cl,&error);
	
/* Any problem with server ? If yes and if it is a time-out, try to reconnect
   to a new database server. */

	if(recev == NULL)
	{
		if(error == DevErr_RPCTimedOut)
		{
			to_reconnection((void *)NULL,(void **)&recev,&local_cl,
					(int)DB_GETDSHOST,(long)0,DB_UDP,&error);
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
			recev = db_getdsonhost_1(&host_sent,local_cl,&error);
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
		return(DS_NOTOK);
	}
	
/* Initialize ds number */

	nb_ds = recev->length;
	*p_ds_nb = nb_ds;

/* Allocate memory for domain names array and copy them */

	if ((*ds_list = (db_svc *)calloc(nb_ds,sizeof(db_svc))) == NULL)
	{
		clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		clnt_freeres(local_cl,(xdrproc_t)xdr_db_svcarray_net,(char *)recev);
		*p_error = DbErr_ClientMemoryAllocation;
		return(DS_NOTOK);
	}
	else 
	{	
		for (i = 0;i < nb_ds;i++)
		{
			strcpy((*ds_list)[i].server_name,recev->sequence[i].server_name);
			strcpy((*ds_list)[i].personal_name,recev->sequence[i].personal_name);
			strcpy((*ds_list)[i].host_name,recev->sequence[i].host_name);
			(*ds_list)[i].pid = recev->sequence[i].pid;
			(*ds_list)[i].program_num = recev->sequence[i].program_num;
		}
	}
	
/* Free memory allocated by XDR stuff */

	clnt_freeres(local_cl,(xdrproc_t)xdr_db_svcarray_net,(char *)recev);

/* Leave function */

	clnt_control(local_cl,CLSET_TIMEOUT,(char *)&old_tout);
	clnt_control(local_cl,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
	return(exit_code);

}

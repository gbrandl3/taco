/*+*******************************************************************

 File       :   reconnect_cli.c

 Project    :   Static database

 Description:   Application Programmers Interface

            :   Interface to access static database

 Author(s)  :   Emmanuel Taurel
		$Author: jkrueger1 $

 Original   :   September 1998
  
 Version    :	$Revision: 1.4 $

 Date	    : 	$Date: 2004-03-05 15:07:00 $

 Copyright (c) 1998 by European Synchrotron Radiation Facility,
                       Grenoble, France
  
 *-*******************************************************************/
#include "config.h"
#define PORTMAP

#include <macros.h>
#include <db_setup.h>
#include <db_xdr.h>
#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>

#if defined(_NT)
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
#			inclucde <socket.h>
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

#ifdef ALONE
extern CLIENT *cl;
#endif

extern dbserver_info 		db_info;
extern configuration_flags 	config_flags;
extern nethost_info 		*multi_nethost;


/**@ingroup dbaseAPI
 * To do automatic reconnection with the database server if it has been killed or restarted.		
 *
 * @param p_data	A pointer to the data to pass to the server	
 * @param pp_resul	A double pointer to the place where the result should be stored							
 * @param client	The client handle
 * @param call_type	The type of call
 * @param nethost_index
 * @param connect_type
 * @param error		The error code in case of failure
 *
 * @return   This function returns DS_OK and clears the error code if the reconnection
 *    was successful. Otherwise, the function returns DS_NOTOK and set the error code.								
 */
int to_reconnection(void *p_data, void **pp_result, CLIENT **client,
		    int call_type, long nethost_index,
		    long connect_type, long *error)
{
	union {
		db_res 		*getres_ptr;
		int 		*putres_ptr;
		int 		*delres_ptr;
		db_res 		*getdev_ptr;
		int 		*devexp_ptr;
		db_resimp 	*devimp_ptr;
		int 		*svcunr_ptr;
		svc_inf 	*svcchk_ptr;
		db_res 		*getdevexp_ptr;
		int 		*clodb_ptr;
		int 		*reopendb_ptr;
		cmd_que 	*cmdquery_ptr;
		db_psdev_error 	*psdevreg_ptr;
		db_psdev_error 	*psdevunreg_ptr;
		db_res 		*browse_ptr;
		db_devinfo_svc 	*devinfo_ptr;
		long 		*lg_ptr;
		db_info_svc 	*info_ptr;
		svcinfo_svc 	*svcinfo_ptr;
		db_poller_svc 	*poll_ptr;
		db_delupd_error *delupd_ptr;
		db_svcarray_net *dshost_ptr;
	}recev_ptr;

	void 	*recev_gen;
	CLIENT 	*new_client,
		*old_client,
		*cl_tcp;
	struct timeval 		old_tout;
	struct sockaddr_in 	serv_adr;
	int 			tcp_so;
#ifndef vxworks
	static struct hostent 	*ht;
#else
	static int 		host_addr;
#endif
#ifdef ALONE
	char *serv_name = ALONE_SERVER_HOST;
#endif /* ALONE */

/* Memorize old timeout */
   	clnt_control(*client,CLGET_TIMEOUT,(char *)&old_tout);

/* First change the retry to 1 second to verify that the server does not answer anymore */
	clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&api_retry_timeout);
	clnt_control(*client,CLSET_TIMEOUT,(char *)&timeout);

/* Try to do the NULLPROC call */
	if (db_null_proc_1(*client,error) == 0)
	{
/* (Re)set time-out value */
		clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
		clnt_control(*client,CLSET_TIMEOUT,(char *)&old_tout);
		 
/* Redo the call */
		switch (call_type)
		{
		case DB_GETRES : 
			recev_ptr.getres_ptr = db_getres_1((arr1 *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.getres_ptr;
			break;

		case DB_PUTRES : 
			recev_ptr.putres_ptr = db_putres_1((tab_putres *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.putres_ptr;
			break;

		case DB_DELRES : 
			recev_ptr.delres_ptr = db_delres_1((arr1 *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.delres_ptr;
			break;

		case DB_GETDEV : 
			recev_ptr.getdev_ptr = db_getdev_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.getdev_ptr;
			break;

		case DB_DEVEXP_2 : 
			recev_ptr.devexp_ptr = db_devexp_2((tab_dbdev_2 *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.devexp_ptr;
			break;

		case DB_DEVEXP_3 : 
			recev_ptr.devexp_ptr = db_devexp_3((tab_dbdev_3 *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.devexp_ptr;
			break;

		case DB_DEVIMP : 
			recev_ptr.devimp_ptr = db_devimp_1((arr1 *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.devimp_ptr;
			break;

		case DB_SVCUNR : 
			recev_ptr.svcunr_ptr = db_svcunr_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.svcunr_ptr;
			break;

		case DB_SVCCHK : 
			recev_ptr.svcchk_ptr = db_svcchk_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.svcchk_ptr;
			break;

		case DB_GETDEVEXP : 
			recev_ptr.getdevexp_ptr = db_getdevexp_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.getdevexp_ptr;
			break;

		case DB_CLODB : 
			recev_ptr.clodb_ptr = db_clodb_1(*client,error);
			recev_gen = (void *)recev_ptr.clodb_ptr;
			break;

		case DB_REOPENDB : 
			recev_ptr.reopendb_ptr = db_reopendb_1(*client,error);
			recev_gen = (void *)recev_ptr.reopendb_ptr;
			break;

		case DB_CMDQUERY : 
			recev_ptr.cmdquery_ptr = db_cmd_query_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.cmdquery_ptr;
			break;

		case DB_PSDEV_REG : 
			recev_ptr.psdevreg_ptr = db_psdev_reg_1((psdev_reg_x *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
			      
		case DB_GETDEVDOMAIN : 
			recev_ptr.browse_ptr = db_getdevdomain_1(*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETDEVFAMILY : 
			recev_ptr.browse_ptr = db_getdevfamily_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETDEVMEMBER : 
			recev_ptr.browse_ptr = db_getdevmember_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       			       			       
		case DB_GETRESDOMAIN : 
			recev_ptr.browse_ptr = db_getresdomain_1(*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETRESFAMILY : 
			recev_ptr.browse_ptr = db_getresfamily_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETRESMEMBER : 
			recev_ptr.browse_ptr = db_getresmember_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETRESRESO : 
			recev_ptr.browse_ptr = db_getresreso_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			     
		case DB_GETRESRESOVAL : 
			recev_ptr.browse_ptr = db_getresresoval_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			     			       			       			       			       
		case DB_GETSERVER :
			recev_ptr.browse_ptr = db_getdsserver_1(*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			    
		case DB_GETPERS : 
			recev_ptr.browse_ptr = db_getdspers_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			    			    
		case DB_GETHOST : 
			recev_ptr.browse_ptr = db_gethost_1(*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			  
		case DB_DEVINFO : 
			recev_ptr.devinfo_ptr = db_deviceinfo_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.devinfo_ptr;
			break;
			  
		case DB_DEVRES : 
			recev_ptr.browse_ptr = db_deviceres_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			 
		case DB_DEVDELALLRES : 
			recev_ptr.psdevreg_ptr = db_devicedeleteres_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
			       
		case DB_INFO : 
			recev_ptr.info_ptr = db_stat_1(*client,error);
		       	recev_gen = (void *)recev_ptr.info_ptr;
		       	break;
		       
		case DB_SVCUNREG : 
			recev_ptr.lg_ptr = db_servunreg_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			break;
			   
		case DB_SVCINFO : 
			recev_ptr.svcinfo_ptr = db_servinfo_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.svcinfo_ptr;
			break;
			    
		case DB_UPDDEV : 
			recev_ptr.psdevreg_ptr = db_upddev_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
			 
		case DB_UPDRES : 
			recev_ptr.psdevreg_ptr = db_updres_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
			 
		case DB_SECPASS : 
			recev_ptr.browse_ptr = db_secpass_1(*client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			  
		case DB_GETPOLLER : 
			recev_ptr.poll_ptr = db_getpoll_1((nam *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.poll_ptr;
			break;
			    
		case DB_DEL_UPDATE : 
			recev_ptr.delupd_ptr = db_delete_update_1((db_arr1_array *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.delupd_ptr;
			break;
			
		case DB_PSDEV_UNREG : 
			recev_ptr.psdevunreg_ptr = db_psdev_unreg_1((arr1 *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.psdevunreg_ptr;
			if (recev_ptr.psdevunreg_ptr->error_code == DbErr_DeviceNotDefined)
				recev_ptr.psdevunreg_ptr->error_code = 0;
			break;
			
		case DB_SVCDELETE : 
			recev_ptr.lg_ptr = db_servdelete_1((db_res *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			if (*(recev_ptr.lg_ptr) == DbErr_DeviceServerNotDefined)
				*(recev_ptr.lg_ptr) = 0;
			break;
			
		case DB_DEVDEL : 
			recev_ptr.lg_ptr = db_devicedelete_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			if (*(recev_ptr.lg_ptr) == DbErr_DeviceNotDefined)
				*(recev_ptr.lg_ptr) = 0;
			break;
			
		case DB_INITCACHE :
			recev_ptr.lg_ptr = db_initcache_1((char **)p_data,*client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			break;
			
		case DB_GETDSHOST :
			recev_ptr.dshost_ptr = db_getdsonhost_1((nam *)p_data,*client,error);
			recev_gen = (void *)recev_ptr.dshost_ptr;
			break;
		}
			
/* Pass data to caller and leave function */
		*pp_result = recev_gen;
		if (recev_gen != NULL)
		{
			*error = 0;
			return(0);
		}
		else
			return(-1);
	}
	else
	{
	
/* Let us say that the database server is dead. Ask for a new
   connection to the new database server. If the nethost index is 0 (default
   nethost), don't use the multi_nethost array. Set the API database_server
   and configuration flags for the right network to false in order to
   force the API to re-ask the manager for the new db server RPC parameters. */

#ifdef ALONE
		old_client = *client;
		if (connect_type == DB_UDP)
			new_client = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"udp");
		else
			new_client = clnt_create(serv_name,DB_SETUPPROG,DB_VERS_3,"tcp");
		if (new_client == NULL)
		{
			clnt_control(*client,CLSET_TIMEOUT,(char *)&old_tout);
			clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
			*error = DbErr_CannotCreateClientHandle;
			return(-1);
		}
		clnt_destroy(old_client);
		clnt_control(new_client,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(new_client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
#else
		if (connect_type == DB_UDP)
		{
			if (nethost_index == 0)
			{
				old_client = db_info.conf->clnt;
				config_flags.database_server = False;
				config_flags.configuration = False;
				if(db_import(error))
				{
					clnt_control(*client,CLSET_TIMEOUT,(char *)&timeout);
					clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&retry_timeout);
					config_flags.database_server = True;
					config_flags.database_server = True;
					db_info.conf->clnt = *client;
					return(-1);
				}
				new_client = db_info.conf->clnt;
				clnt_destroy(old_client);
			}
			else
			{
				old_client = multi_nethost[nethost_index].db_info->clnt;
				multi_nethost[nethost_index].config_flags.database_server = False;
				multi_nethost[nethost_index].config_flags.configuration = False;
				if (db_import_multi(multi_nethost[nethost_index].nethost,error))
				{
					clnt_control(*client,CLSET_TIMEOUT,(char *)&old_tout);
					clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
					multi_nethost[nethost_index].config_flags.database_server = True;
					multi_nethost[nethost_index].config_flags.configuration = True;
					multi_nethost[nethost_index].db_info->clnt = *client;
					return(-1);
				}
				new_client = multi_nethost[nethost_index].db_info->clnt;
				clnt_destroy(old_client);
			}
		}
		else
		{
#ifndef vxworks
			if (nethost_index == 0)
				ht = gethostbyname(db_info.conf->server_host);
			else
				ht = gethostbyname(multi_nethost[nethost_index].db_info->server_host);
			if (ht == NULL)
			{
				clnt_control(*client,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*error = DbErr_CannotCreateClientHandle;
				return(-1);
			}		
#else /* !vxworks */
			if (nethost_index == 0)
				host_addr = hostGetByName(db_info.conf->server_host);
			else
				host_addr = hostGetByName(multi_nethost[nethost_index].db_info.conf->server_host);
			if (host_addr == 0)
			{
				clnt_control(*client,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*error = DbErr_CannotCreateClientHandle;
				return(-1);
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
				clnt_control(*client,CLSET_TIMEOUT,(char *)&old_tout);
				clnt_control(*client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
				*error = DbErr_CannotCreateClientHandle;
				return(-1);
			}
			else
			{
				clnt_destroy(*client);
				new_client = cl_tcp;
			}
		}
		clnt_control(new_client,CLSET_TIMEOUT,(char *)&old_tout);
		clnt_control(new_client,CLSET_RETRY_TIMEOUT,(char *)&old_tout);
#endif /* ALONE */

/* Pass the new connection to caller */
		
		*client = new_client;
#ifdef ALONE
		if (connect_type == DB_UDP)
			cl = new_client;
#endif /* ALONE */

/* Now, redo the call using the new connection */

		switch (call_type)
		{
		case DB_GETRES : 
			recev_ptr.getres_ptr = db_getres_1((arr1 *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.getres_ptr;
			break;

		case DB_PUTRES : 
			recev_ptr.putres_ptr = db_putres_1((tab_putres *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.putres_ptr;
			break;

		case DB_DELRES : 
			recev_ptr.delres_ptr = db_delres_1((arr1 *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.delres_ptr;
			break;

		case DB_GETDEV : 
			recev_ptr.getdev_ptr = db_getdev_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.getdev_ptr;
			break;

		case DB_DEVEXP_2 : 
			recev_ptr.devexp_ptr = db_devexp_2((tab_dbdev_2 *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.devexp_ptr;
			break;

		case DB_DEVEXP_3 : 
			recev_ptr.devexp_ptr = db_devexp_3((tab_dbdev_3 *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.devexp_ptr;
			break;

		case DB_DEVIMP : 
			recev_ptr.devimp_ptr = db_devimp_1((arr1 *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.devimp_ptr;
			break;

		case DB_SVCUNR : 
			recev_ptr.svcunr_ptr = db_svcunr_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.svcunr_ptr;
			break;

		case DB_SVCCHK : 
			recev_ptr.svcchk_ptr = db_svcchk_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.svcchk_ptr;
			break;

		case DB_GETDEVEXP : 
			recev_ptr.getdevexp_ptr = db_getdevexp_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.getdevexp_ptr;
			break;

		case DB_CLODB : 
			recev_ptr.clodb_ptr = db_clodb_1(new_client,error);
			recev_gen = (void *)recev_ptr.clodb_ptr;
			break;

		case DB_REOPENDB : 
			recev_ptr.reopendb_ptr = db_reopendb_1(new_client,error);
			recev_gen = (void *)recev_ptr.reopendb_ptr;
			break;

		case DB_CMDQUERY : 
			recev_ptr.cmdquery_ptr = db_cmd_query_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.cmdquery_ptr;
			break;

		case DB_PSDEV_REG : 
			recev_ptr.psdevreg_ptr = db_psdev_reg_1((psdev_reg_x *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;

		case DB_PSDEV_UNREG : 
			recev_ptr.psdevunreg_ptr = db_psdev_unreg_1((arr1 *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.psdevunreg_ptr;
			break;
				      
		case DB_GETDEVDOMAIN : 
			recev_ptr.browse_ptr = db_getdevdomain_1(new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETDEVFAMILY : 
			recev_ptr.browse_ptr = db_getdevfamily_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETDEVMEMBER : 
			recev_ptr.browse_ptr = db_getdevmember_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       			       			       
		case DB_GETRESDOMAIN : 
			recev_ptr.browse_ptr = db_getresdomain_1(new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETRESFAMILY : 
			recev_ptr.browse_ptr = db_getresfamily_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETRESMEMBER : 
			recev_ptr.browse_ptr = db_getresmember_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			       
		case DB_GETRESRESO : 
			recev_ptr.browse_ptr = db_getresreso_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			     
		case DB_GETRESRESOVAL : 
			recev_ptr.browse_ptr = db_getresresoval_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			     			       			       			       			       
		case DB_GETSERVER : 
			recev_ptr.browse_ptr = db_getdsserver_1(new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			    
		case DB_GETPERS : 
			recev_ptr.browse_ptr = db_getdspers_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
			    			    
		case DB_GETHOST : 
			recev_ptr.browse_ptr = db_gethost_1(new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
				  
		case DB_DEVINFO : 
			recev_ptr.devinfo_ptr = db_deviceinfo_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.devinfo_ptr;
			break;
				  
		case DB_DEVRES : 
			recev_ptr.browse_ptr = db_deviceres_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
				 
		case DB_DEVDEL : 
			recev_ptr.lg_ptr = db_devicedelete_1((char **)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			break;
				 
		case DB_DEVDELALLRES : 
			recev_ptr.psdevreg_ptr = db_devicedeleteres_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
				       
		case DB_INFO : 
			recev_ptr.info_ptr = db_stat_1(new_client,error);
			recev_gen = (void *)recev_ptr.info_ptr;
			break;
			       
		case DB_SVCUNREG : 
			recev_ptr.lg_ptr = db_servunreg_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			break;
				   
		case DB_SVCINFO : 
			recev_ptr.svcinfo_ptr = db_servinfo_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.svcinfo_ptr;
			break;
				  
		case DB_SVCDELETE : 
			recev_ptr.lg_ptr = db_servdelete_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			break;
				    
		case DB_UPDDEV : 
			recev_ptr.psdevreg_ptr = db_upddev_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
				 
		case DB_UPDRES : 
			recev_ptr.psdevreg_ptr = db_updres_1((db_res *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.psdevreg_ptr;
			break;
				 
		case DB_SECPASS : 
			recev_ptr.browse_ptr = db_secpass_1(new_client,error);
			recev_gen = (void *)recev_ptr.browse_ptr;
			break;
				  
		case DB_GETPOLLER : 
			recev_ptr.poll_ptr = db_getpoll_1((nam *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.poll_ptr;
			break;
				    
		case DB_DEL_UPDATE : 
			recev_ptr.delupd_ptr = db_delete_update_1((db_arr1_array *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.delupd_ptr;
			break;
			
		case DB_INITCACHE : 
			recev_ptr.lg_ptr = db_initcache_1((nam *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.lg_ptr;
			break;
			
		case DB_GETDSHOST :
			recev_ptr.dshost_ptr = db_getdsonhost_1((nam *)p_data,new_client,error);
			recev_gen = (void *)recev_ptr.dshost_ptr;
			break;
		}

/* Pass data to caller and leave function */
		*pp_result = recev_gen;
		if (recev_gen != NULL)
		{
			*error = 0;
			return(0);
		}
		else
			return(-1);
	}
}


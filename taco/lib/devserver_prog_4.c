/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:	devserver_prog_4.c
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	Server side of the API.  
 *
 * Author(s);	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	Feb 1994
 *
 * Version:	$Revision: 1.1 $
 *
 * Date:		$Date: 2008-12-18 08:04:40 $
 *
 ********************************************************************-*/
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
/*
 * Include files and Static Routine definitions
 */

/*
 * C++ version
 */
#ifndef WIN32
#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#else
#	include <types.h>
#endif
#endif /* !WIN32 */

#include <API.h>
#include <private/ApiP.h>
#include <DevCmds.h>

#include <DevServer.h>
#include <DevServerP.h>
#include <DevSignal.h>
#include <Admin.h>
#include <DevErrors.h>
#if defined WIN32
#include <rpc/Pmap_pro.h>
#include <rpc/pmap_cln.h>
#include <process.h>
/* #define NODATETIMEPICK to avoid compiler error. (I hate MicroSoft!)*/
#define NODATETIMEPICK
#include <commctrl.h>
#undef NODATETIMEPICK
#else
#include <rpc/pmap_clnt.h>
#endif  /* WIN32 */

#include "taco_utils.h"


/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Type for global state flags for the current
 *  server or client status is defined in API.h
 */

/*
 *  Configuration flags
 */

#ifdef __cplusplus
extern "C" char 		*dev_error_stack;
#else
extern char 		    	*dev_error_stack;
#endif

/*
 *  Debug flag
 */

extern long 	debug_flag;

/**
 * @ingroup dsAPIintern
 * Entry point for received RPCs. Switches to the wished remote procedure.
 * 
 * @param rqstp   RPC request handle
 * @param transp  Service transport handle
 *
 */
void _WINAPI devserver_prog_4 (struct svc_req *rqstp, SVCXPRT *transp) 
{
	char	*help_ptr;
	int	pid = 0;

	union {
		_dev_import_in	rpc_dev_import_4_arg;
		_dev_free_in 	rpc_dev_free_4_arg;
		_server_data 	rpc_dev_putget_4_arg;
		_server_data 	rpc_dev_put_4_arg;
                _dev_query_in   rpc_dev_query_cmd_4_arg;
/*
 * RPC ADMIN service disabled temporarily, to be reimplemented later
 *
 * - andy 26nov96
 *
 *		long 		rpc_admin_import_4_arg;
 *		_server_admin 	rpc_admin_4_arg;
 */
		_server_data 	rpc_dev_putget_raw_4_arg;
		_server_data 	rpc_dev_put_asyn_4_arg;
	} argument;

	char *result;
	xdrproc_t xdr_argument,xdr_result;
#ifdef __cplusplus
	DevRpcLocalFunc local;
#else
	char *(*local)();
#endif

/*
 *  call the right server routine
 */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "devserver_prog_4(): called with rqstp->rq_proc %d\n", rqstp->rq_proc);
	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
			svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
			return;

        	case RPC_QUIT_SERVER:
			svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
#if defined (WIN32)
                	raise(SIGABRT);
#else  /* WIN32 */
#if !defined (vxworks)
                	pid = getpid ();
#else  /* !vxworks */
                	pid = taskIdSelf ();
#endif /* !vxworks */
                	kill (pid,SIGQUIT);
#endif /* WIN32 */
			return;

		case RPC_CHECK:
			help_ptr = &(config_flags->server_name[0]);
			svc_sendreply (transp, (xdrproc_t)xdr_wrapstring, (caddr_t) &help_ptr);
			return;

		case RPC_DEV_IMPORT:
			xdr_argument = (xdrproc_t)xdr__dev_import_in;
			xdr_result = (xdrproc_t)xdr__dev_import_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_import_4;
#else
			local = (char *(*)()) rpc_dev_import_4;
#endif
			break;

		case RPC_DEV_FREE:
			xdr_argument = (xdrproc_t)xdr__dev_free_in;
			xdr_result = (xdrproc_t)xdr__dev_free_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_free_4;
#else
			local = (char *(*)()) rpc_dev_free_4;
#endif
			break;

		case RPC_DEV_PUTGET:
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_data;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_4;
#else
			local = (char *(*)()) rpc_dev_putget_4;
#endif
			break;

		case RPC_DEV_PUT:
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_data;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_put_4;
#else
			local = (char *(*)()) rpc_dev_put_4;
#endif
			break;

		case RPC_DEV_CMD_QUERY:
			xdr_argument = (xdrproc_t)xdr__dev_query_in;
			xdr_result = (xdrproc_t)xdr__dev_query_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_cmd_query_4;
#else
			local = (char *(*)()) rpc_dev_cmd_query_4;
#endif
			break;
/* event query */
		case RPC_DEV_EVENT_QUERY:
        		xdr_argument = (xdrproc_t)xdr__dev_query_in;
        		xdr_result = (xdrproc_t)xdr__dev_queryevent_out;
#ifdef __cplusplus
        		local = (DevRpcLocalFunc) rpc_dev_event_query_4;
#else
        		local = (char *(*)()) rpc_dev_event_query_4;
#endif
        		break;
/* end event query */

/*
 * RPC ADMIN service disabled temporarily, to be reimplemented later
 *
 * - andy 26nov96
 *
 *		case RPC_ADMIN_IMPORT:
 *			xdr_argument = xdr_DevLong;
 *			xdr_result = xdr_DevLong;
 *			local = (char *(*)()) rpc_admin_import_4;
 *			break;
 *
 *		case RPC_ADMIN:
 *			xdr_argument = (xdrproc_t)xdr__server_admin;
 *			xdr_result = xdr_DevLong;
 *			local = (char *(*)()) rpc_admin_4;
 *			break;
 */
		case RPC_DEV_PUTGET_RAW:
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_raw_data;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_raw_4;
#else
			local = (char *(*)()) rpc_dev_putget_raw_4;
#endif
			break;

		case RPC_DEV_PUT_ASYN:
/*
 * Read incoming arguments and send the reply immediately without waiting
 * the execution of the function.
 */
			xdr_argument = (xdrproc_t)xdr__server_data;
			xdr_result = (xdrproc_t)xdr__client_data;
/*
 * Function only for the adminstration and security part of the asynchronous call.
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_put_asyn_4;
#else
			local = (char *(*)()) rpc_dev_put_asyn_4;
#endif
	        	break;

		case RPC_DEV_PING:
			xdr_argument = (xdrproc_t)xdr__dev_import_in;
			xdr_result = (xdrproc_t)xdr__dev_import_out;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_ping_4;
#else
			local = (char *(*)()) rpc_dev_ping_4;
#endif
			break;

		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));

	if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND, "svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call (*local) \n");
	result = (*local)(&argument, rqstp);
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call (*local)() %s\n", (result == NULL ? "failed" : "ok"));

	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call svc_sendreply()\n");
	if (result != NULL && !svc_sendreply(transp, xdr_result, (caddr_t)result)) 
	{
		dev_printerror (SEND, "svcerr_systemerr : server couldn't send reply arguments");
		svcerr_systemerr(transp);
	}
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call svc_sendreply() ok\n");

/*
 * If an asynchronous call was requested, execute now the command. After the answer was already send back to 
 * the client.
 */
	if ( rqstp->rq_proc == RPC_DEV_PUT_ASYN )
	{
		rpc_dev_put_asyn_cmd ((_server_data *)&argument);
	}

/*
 * If dev_free() was called AND the server is on OS9 AND tcp then give OS9 a hand in closing this end of 
 * the tcp socket. This fixes a bug in the OS9 (>3.x) implementation which led to the server blocking for 
 * a few seconds when closing a tcp connection.
 */
	dev_printdebug (DBG_TRACE | DBG_DEV_SVR_CLASS, "\nDevServerMain() : call svc_freeargs()\n");
	if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND, "svc_freeargs : server couldn't free arguments !!");
	}
	return;
}

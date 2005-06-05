
/*+*******************************************************************

 File       :  	ds_prog_vers3.c

 Project    : 	Device Servers with sun-rpc

 Description:   RPC entry points for the RPC version 1
		which was used up to library version 3.

 Author(s)  :	Jens Meyer
 		$Author: andy_gotz $

 Original   :	November 1994

 Version    :	$Revision: 1.5 $

 Date	    :	$Date: 2005-06-05 20:43:50 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

*******************************************************************-*/

#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <DevServerP.h>
#include <API_xdr_vers3.h>

/****************************************
 *          Globals                     *
 ****************************************/

/*
 *  Types for global state flags for the current
 *  server or client status and for Database Server
 *  information are defined in API.h
 */

/*
 *  Configuration flags
 */

extern configuration_flags      config_flags;


/*
 * Minimal access for clients of version 3.
 * Defined in DevServerMain.c and initialised with the
 * minimal access right given in the security database.
 */
#ifdef __cplusplus
extern "C"
#endif /* _cplusplus */
long minimal_access = WRITE_ACCESS;

void _WINAPI devserver_prog_1	PT_( (struct svc_req *rqstp,SVCXPRT *transp) );

/**@ingroup dsAPI
 * Entry point for received RPCs of version 1.
 *
 * Switches to the wished remote procedure.
 *
 * @param rqstp 	RPC request handle
 * @param transp        Service transport handle
 */ 
void _WINAPI devserver_prog_1 (struct svc_req *rqstp, SVCXPRT *transp) 
{
	union {
		_dev_import_in	rpc_dev_import_4_arg;
		_dev_free_in 	rpc_dev_free_4_arg;
		_server_data 	rpc_dev_putget_4_arg;
                _dev_query_in   rpc_dev_query_cmd_4_arg;
	} argument;

	char		*help_ptr;
	char 		*result;
	xdrproc_t 	xdr_argument,
			xdr_result;
#ifdef  __cplusplus
	DevRpcLocalFunc local;
#else
	char 		*(*local)();
#endif

/*
 *  call the right server routine
 */
	switch (rqstp->rq_proc) 
	{
		case NULLPROC:
#ifdef _UCC
			svc_sendreply(transp, (xdrproc_t)xdr_void, (caddr_t)NULL);
#else
			svc_sendreply(transp, (xdrproc_t)xdr_void, NULL);
#endif /* _UCC */
			return;
		case RPC_CHECK:
			help_ptr = &(config_flags.server_name[0]);
			svc_sendreply (transp, (xdrproc_t)xdr_wrapstring, (caddr_t) &help_ptr);
			return;
		case RPC_DEV_IMPORT:
			xdr_argument = (xdrproc_t)xdr__dev_import_in_3;
			xdr_result = (xdrproc_t)xdr__dev_import_out_3;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_import_4;
#else
			local = (char *(*)()) rpc_dev_import_4;
#endif
			break;

		case RPC_DEV_FREE:
			xdr_argument = (xdrproc_t)xdr__dev_free_in_3;
			xdr_result = (xdrproc_t)xdr__dev_free_out_3;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_free_4;
#else
			local = (char *(*)()) rpc_dev_free_4;
#endif
			break;
		case RPC_DEV_PUTGET:
			xdr_argument = (xdrproc_t)xdr__server_data_3;
			xdr_result = (xdrproc_t)xdr__client_data_3;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_4;
#else
			local = (char *(*)()) rpc_dev_putget_4;
#endif
			break;
		case RPC_DEV_PUT:
			xdr_argument = (xdrproc_t)xdr__server_data_3;
			xdr_result = (xdrproc_t)xdr__client_data_3;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_put_4;
#else
			local = (char *(*)()) rpc_dev_put_4;
#endif
			break;

        	case RPC_DEV_CMD_QUERY:
			xdr_argument = (xdrproc_t)xdr__dev_query_in_3;
			xdr_result = (xdrproc_t)xdr__dev_query_out_3;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_cmd_query_4;
#else
			local = (char *(*)()) rpc_dev_cmd_query_4;
#endif
			break;

		case RPC_DEV_PUTGET_RAW:
			xdr_argument = (xdrproc_t)xdr__server_data_3;
			xdr_result = (xdrproc_t)xdr__client_raw_data_3;
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_putget_raw_4;
#else
			local = (char *(*)()) rpc_dev_putget_raw_4;
#endif
			break;

		case RPC_DEV_PUT_ASYN:
/*
 * Read incoming arguments and send
 * the reply immediately without waiting
 * the execution of the function.
 */
			xdr_argument = (xdrproc_t)xdr__server_data_3;
			xdr_result = (xdrproc_t)xdr__client_data_3;
/*
 * Function only for the adminstration and security part
 * of the asynchronous call.
 */
#ifdef __cplusplus
			local = (DevRpcLocalFunc) rpc_dev_put_asyn_4;
#else
			local = (char *(*)()) rpc_dev_put_asyn_4;
#endif
	        	break;

		default:
			svcerr_noproc(transp);
			return;
	}

	memset(&argument, 0, sizeof(argument));
	if (!svc_getargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND,"%s", "svcerr_decode : server couldn't decode incoming arguments");
		svcerr_decode(transp);
		return;
	}

/*
 * Now switch again on the requested procedure and
 * set default values into the not initialised fields.
 * Version 4 has some more fields initialised.
 */
	switch (rqstp->rq_proc) 
	{
		case RPC_DEV_IMPORT:
	 		argument.rpc_dev_import_4_arg.access_right  = minimal_access;
	 		argument.rpc_dev_import_4_arg.client_id     = 0;
	 		argument.rpc_dev_import_4_arg.connection_id = 0;

	 		argument.rpc_dev_import_4_arg.var_argument.length   = 0;
	 		argument.rpc_dev_import_4_arg.var_argument.sequence = 0;
			break;
		case RPC_DEV_FREE:
	 		argument.rpc_dev_free_4_arg.access_right = minimal_access;
	 		argument.rpc_dev_free_4_arg.client_id    = 0;

	 		argument.rpc_dev_free_4_arg.var_argument.length   = 0;
	 		argument.rpc_dev_free_4_arg.var_argument.sequence = 0;
			break;
		case RPC_DEV_PUTGET:
		case RPC_DEV_PUT:
		case RPC_DEV_PUTGET_RAW:
		case RPC_DEV_PUT_ASYN:
			argument.rpc_dev_putget_4_arg.access_right = minimal_access;
			argument.rpc_dev_putget_4_arg.client_id    = 0;

			argument.rpc_dev_putget_4_arg.var_argument.length   = 0;
			argument.rpc_dev_putget_4_arg.var_argument.sequence = 0;
			break;

		case RPC_DEV_CMD_QUERY:
			argument.rpc_dev_query_cmd_4_arg.var_argument.length   = 0;
			argument.rpc_dev_query_cmd_4_arg.var_argument.sequence = 0;
			break;
	}

	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t)xdr_result, (caddr_t)result)) 
	{
		dev_printerror (SEND,"%s", "svcerr_systemerr : server couldn't send repply arguments");
		svcerr_systemerr(transp);
	}

/*
 * If an asynchronous call was requested, execute now the
 * command. After the answer was already send back to 
 * the client.
 */
	if ( rqstp->rq_proc == RPC_DEV_PUT_ASYN )
		rpc_dev_put_asyn_cmd ((_server_data *)&argument);

	if (!svc_freeargs(transp, xdr_argument, (caddr_t) &argument)) 
	{
		dev_printerror (SEND,"%s", "svc_freeargs : server couldn't free arguments !!");
		return;
	}
	return;
}

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
 * File:	dcwr_clnt.c
 *
 * Description:
 * 
 * Author(s):   Emmanuel Taurel
 *		$Author: andy_gotz $
 *
 * Original:    1992
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-01-22 21:21:43 $
 *
 ******************************************************************************/


#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>
#include <Admin.h>
#include <dc_xdr.h>

#ifndef NULL
#define NULL  (0)
#endif  /* NULL */

static struct timeval TIMEOUT = { 3, 0 };

dc_xdr_error *dc_open_1(dc_open_in *argp,CLIENT *clnt,long *perr)
{
	static dc_xdr_error 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_OPEN,(xdrproc_t)xdr_dc_open_in, (caddr_t)argp,(xdrproc_t)xdr_dc_xdr_error, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"dc_open clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

dc_xdr_error *dc_close_1(name_arr *argp,CLIENT *clnt,long *perr)
{
	static dc_xdr_error 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_CLOSE,(xdrproc_t)xdr_name_arr, (caddr_t)argp,(xdrproc_t)xdr_dc_xdr_error, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"dc_close clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

dc_xdr_error *dc_dataput_1(dev_datarr *argp,CLIENT *clnt,long *perr)
{
	static dc_xdr_error 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_DATAPUT,(xdrproc_t)xdr_dev_datarr, (caddr_t)argp,(xdrproc_t)xdr_dc_xdr_error, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"dc_dataput clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

dc_infox_back *dc_info_1(void *argp,CLIENT *clnt,long *perr)
{
	static dc_infox_back 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_INFO,(xdrproc_t)xdr_void, (char*)argp,(xdrproc_t)xdr_dc_infox_back, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"dc_info clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

dc_devallx_back *dc_devall_1(void *argp,CLIENT *clnt,long *perr)
{
	static dc_devallx_back 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_DEVALL,(xdrproc_t)xdr_void, (char*)argp,(xdrproc_t)xdr_dc_devallx_back, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"dc_devall clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

dc_devinfx_back *dc_devinfo_1(name *argp,CLIENT *clnt,long *perr)
{
	static dc_devinfx_back 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_DEVINFO,(xdrproc_t)xdr_name, (caddr_t)argp,(xdrproc_t)xdr_dc_devinfx_back, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"dc_devinfo clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



int dc_rpcwr_check_clnt_1(CLIENT *clnt,char **res,long *perr)
{
	enum clnt_stat clnt_stat;

	clnt_stat = clnt_call(clnt, RPC_CHECK,(xdrproc_t)xdr_void, NULL,(xdrproc_t)xdr_wrapstring,(caddr_t)res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS) 
	{
		dev_printdebug(DBG_ERROR,"rpc_check clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else
			*perr = DcErr_RPCreception;
		return (-1);
	}
	return (0);
}

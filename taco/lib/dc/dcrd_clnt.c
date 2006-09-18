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
 * File:	dcrd_clnt.c
 *
 * Description:
 *
 * Author(s):   Emmanuel Taurel
 *              $Author: jkrueger1 $
 * 
 * Original:    1993
 *
 * Version:     $Revision: 1.5 $
 *
 * Date:        $Date: 2006-09-18 22:13:30 $
 *
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <private/ApiP.h>
#include <DevErrors.h>
#include <Admin.h>

#ifndef OSK
#	if HAVE_TIME_H
#		include <time.h>
#	endif
#endif

#include "dc_xdr.h"

#ifndef NULL
#define NULL  0
#endif  /* NULL */

static struct timeval TIMEOUT = { 3, 0 };


xres_clnt *dc_devget_clnt_1(xdevget *argp, CLIENT *clnt, DevArgument argout, DevType argout_type, long *perr)
{
	static xres_clnt 	res;
	enum clnt_stat 		clnt_stat;

/* 
 * Clear only place for the error code coming from server, and initialize 
 * the remaining parameters of the "res" structure with the parameters
 * coming from the caller 
 */
	memset((char *)&res, 0, sizeof(res));
	res.xargout_type = argout_type;
	res.xargout = argout;

	clnt_stat = clnt_call(clnt, DC_DEVGET,(xdrproc_t)xdr_xdevget, (caddr_t)argp,(xdrproc_t)xdr_xres_clnt, (caddr_t)&res, TIMEOUT);

	if (clnt_stat != RPC_SUCCESS)
	{
		dev_printdebug(DBG_ERROR,"devget clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DevErr_DevArgTypeNotCorrectImplemented;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

xresv_clnt *dc_devgetv_clnt_1(xdevgetv *argp, CLIENT *clnt, xres_clnt *st_ptr, long *perr)
{
	static xresv_clnt 	res;
	enum clnt_stat 		clnt_stat;

	memset((char *)&res, 0, sizeof(res));
	res.xresa_clnt.xresv_clnt_val = st_ptr;

	clnt_stat = clnt_call(clnt, DC_DEVGETV,(xdrproc_t)xdr_xdevgetv, (caddr_t)argp,(xdrproc_t)xdr_xresv_clnt, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS)
	{
		dev_printdebug(DBG_ERROR,"devgetv clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DevErr_DevArgTypeNotCorrectImplemented;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

xresm_clnt *dc_devgetm_clnt_1(mpar *argp, CLIENT *clnt, mint *st_ptr, long *perr)
{
	static xresm_clnt 	res;
	enum clnt_stat 	clnt_stat;

	memset((char *)&res, 0, sizeof(res));
	res.x_clnt.x_clnt_val = st_ptr;

	clnt_stat = clnt_call(clnt, DC_DEVGETM,(xdrproc_t)xdr_mpar, (caddr_t)argp,(xdrproc_t)xdr_xresm_clnt, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS)
	{
		dev_printdebug(DBG_ERROR,"devgetm clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DevErr_DevArgTypeNotCorrectImplemented;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

outpar *dc_devdef_1(imppar *argp, CLIENT *clnt, long *perr)
{
	static outpar 	res;
	enum clnt_stat 	clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DC_DEVDEF,(xdrproc_t)xdr_imppar, (caddr_t)argp,(xdrproc_t)xdr_outpar, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS)
	{
		dev_printdebug(DBG_ERROR,"devdef clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DevErr_DevArgTypeNotCorrectImplemented;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

xres_hist_clnt *dc_devgeth_clnt_1(xdevgeth *argp, CLIENT *clnt, xresh_clnt *st_ptr, long *perr)
{
	static xres_hist_clnt	res;
	enum clnt_stat		clnt_stat;

	memset((char *)&res, 0, sizeof(res));

	res.xresb_clnt.xresh_clnt_val = st_ptr;

	clnt_stat = clnt_call(clnt, DC_DEVGET_H,(xdrproc_t)xdr_xdevgeth, (caddr_t)argp,(xdrproc_t)xdr_xres_hist_clnt, (caddr_t)&res, TIMEOUT);
	if (clnt_stat != RPC_SUCCESS)
	{
		dev_printdebug(DBG_ERROR,"devget_history clnt_call returned : %s\n",clnt_sperrno(clnt_stat));
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DcErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTRECV || clnt_stat == RPC_CANTSEND)
			*perr = DcErr_CantContactServer;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DevErr_DevArgTypeNotCorrectImplemented;
		else
			*perr = DcErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

int dc_rpc_check_clnt_1(CLIENT *clnt, char **res, long *perr)
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
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DevErr_DevArgTypeNotCorrectImplemented;
		else
			*perr = DcErr_RPCreception;
		return (-1);
	}
	return (0);
}

/*

 Author(s):     Emmanuel Taurel
                $Author: jkrueger1 $

 Original:      1993

 Version:       $Revision: 1.1 $

 Date:          $Date: 2003-03-18 16:16:19 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

 */

#include <API.h>
#include <ApiP.h>
#include <DevErrors.h>
#include <Admin.h>

#ifndef OSK
#include <time.h>
#endif

#include <dc_xdr.h>

#ifndef NULL
#define NULL  0
#endif  /* NULL */

static struct timeval TIMEOUT = { 3, 0 };


xres_clnt *
dc_devget_clnt_1(argp, clnt, argout, argout_type,perr)
	xdevget *argp;
	CLIENT *clnt;
	DevArgument argout;
	DevType argout_type;
	long *perr;
{
	static xres_clnt res;
	enum clnt_stat clnt_stat;

/* Clear only place for the error code coming from server, and initialize 
   the remaining parameters of the "res" structure with the parameters
   coming from the caller */

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



xresv_clnt *
dc_devgetv_clnt_1(argp, clnt, st_ptr,perr)
	xdevgetv *argp;
	CLIENT *clnt;
	xres_clnt *st_ptr;
	long *perr;
{
	static xresv_clnt res;
	enum clnt_stat clnt_stat;

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



xresm_clnt *
dc_devgetm_clnt_1(argp, clnt, st_ptr,perr)
	mpar *argp;
	CLIENT *clnt;
	mint *st_ptr;
	long *perr;
{
	static xresm_clnt res;
	enum clnt_stat clnt_stat;

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



outpar *
dc_devdef_1(argp, clnt,perr)
	imppar *argp;
	CLIENT *clnt;
	long *perr;
{
	static outpar res;
	enum clnt_stat clnt_stat;

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


xres_hist_clnt *
dc_devgeth_clnt_1(argp, clnt, st_ptr,perr)
	xdevgeth *argp;
	CLIENT *clnt;
	xresh_clnt *st_ptr;
	long *perr;
{
	static xres_hist_clnt res;
	enum clnt_stat clnt_stat;

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



int dc_rpc_check_clnt_1(clnt,res,perr)
	CLIENT *clnt;
	long *perr;
	char **res;
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

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
 * File       : setacc_clnt.c
 *
 * Project    : Static database
 *
 * Description: Application Programmers Interface
 *              Stub routines for calling remote procedures in static
 *	        database server.
 *	        This code was originally generated using "rpcgen"
 *
 * Author(s)  : Emmanuel Taurel
 *              $Author: jkrueger1 $
 *
 * Original   : January 1991
 *
 * Version    : $Revision: 1.5 $
 *
 * Date       : $Date: 2008-04-06 09:07:13 $
 *
 *-*******************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <macros.h>
#include "db_setup.h"
#include "db_xdr.h"

#if defined(WIN32)
#include <rpc.h>
#if 0
#include <nm_rpc.h>
#endif

#else
#include <rpc/rpc.h>
#endif

#if HAVE_TIME_H
#	include <time.h>
#endif
#include <DevErrors.h>


#ifndef NULL
#define NULL  0
#endif  /* NULL */


/* Added for the db_freedevexp function */
extern int func_ptr;
extern devexp_res *tab_clstu;
/* End of added definition */

static struct timeval TimeOut = { 3, 0 };

db_res
#if defined __STDC__ | defined __STDCPP__
* db_getres_1(arr1 *argp, CLIENT *clnt, DevLong *perr)
#else
*db_getres_1(argp, clnt, perr)
	arr1 *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETRES, (xdrproc_t)xdr_arr1, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


db_res
#if defined __STDC__ | defined __STDCPP__
* db_getdev_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
*db_getdev_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));


	clnt_stat = clnt_call(clnt, DB_GETDEV, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

/*
	clnt_stat = clnt_call(clnt, DB_GETDEV, (xdrproc_t)xdr_wrapstring, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));
 */
	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


int
#if defined __STDC__ | defined __STDCPP__
* db_devexp_2(tab_dbdev_2 *argp, CLIENT *clnt, DevLong *perr)
#else
*db_devexp_2(argp, clnt, perr)
	tab_dbdev_2 *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVEXP, (xdrproc_t)xdr_tab_dbdev_2, 
	                      (caddr_t)argp, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


int
#if defined __STDC__ | defined __STDCPP__
* db_devexp_3(tab_dbdev_3 *argp, CLIENT *clnt, DevLong *perr)
#else
*db_devexp_3(argp, clnt, perr)
	tab_dbdev_3 *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVEXP, (xdrproc_t)xdr_tab_dbdev_3, 
	                      (caddr_t)argp, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}




db_resimp
#if defined __STDC__ | defined __STDCPP__
* db_devimp_1(arr1 *argp, CLIENT *clnt, DevLong *perr)
#else
*db_devimp_1(argp, clnt, perr)
	arr1 *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_resimp res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVIMP, (xdrproc_t)xdr_arr1, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_resimp, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


int
#if defined __STDC__ | defined __STDCPP__
* db_svcunr_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
*db_svcunr_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_SVCUNR, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


svc_inf
#if defined __STDC__ | defined __STDCPP__
* db_svcchk_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
*db_svcchk_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static svc_inf res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_SVCCHK, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_svc_inf, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


/* Some changes have been made to this function:
	To give to the user the possibily to free the allocated memory,
	it is necessary to store the returned structure in an array.
	The memory needed for this array is allocated in the setacc_cli.c
        file. */

db_res
#if defined __STDC__ | defined __STDCPP__
* db_getdevexp_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
*db_getdevexp_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	enum clnt_stat clnt_stat;

	clnt_stat = clnt_call(clnt, DB_GETDEVEXP, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&(tab_clstu[func_ptr].res), 
	                      TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&(tab_clstu[func_ptr].res));
}


int
#if defined __STDC__ | defined __STDCPP__
* db_clodb_1(CLIENT *clnt, DevLong *perr)
#else
*db_clodb_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_CLODB, (xdrproc_t)xdr_void, 
	                      NULL, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


int
#if defined __STDC__ | defined __STDCPP__
* db_reopendb_1(CLIENT *clnt, DevLong *perr)
#else
*db_reopendb_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_REOPENDB, (xdrproc_t)xdr_void, 
	                      NULL, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


int
#if defined __STDC__ | defined __STDCPP__
* db_putres_1(tab_putres *argp, CLIENT *clnt, DevLong *perr)
#else
*db_putres_1(argp, clnt, perr)
	tab_putres *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_PUTRES, (xdrproc_t)xdr_tab_putres, 
                              (caddr_t)argp, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


int
#if defined __STDC__ | defined __STDCPP__
* db_delres_1(arr1 *argp, CLIENT *clnt, DevLong *perr)
#else
*db_delres_1(argp, clnt, perr)
	arr1 *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static int res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DELRES, (xdrproc_t)xdr_arr1, 
	                      (caddr_t)argp, (xdrproc_t)xdr_int, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


cmd_que
#if defined __STDC__ | defined __STDCPP__
* db_cmd_query_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
*db_cmd_query_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static cmd_que res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_CMDQUERY, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_cmd_que, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

/*event query function */
event_que
#if defined __STDC__ | defined __STDCPP__
* db_event_query_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
*db_event_query_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static event_que res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_EVENTQUERY, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_event_que, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}
/*end event query */


db_psdev_error
#if defined __STDC__ | defined __STDCPP__
* db_psdev_reg_1(psdev_reg_x *argp, CLIENT *clnt, DevLong *perr)
#else
*db_psdev_reg_1(argp, clnt, perr)
	ps_dev_reg_x *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_psdev_error res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_PSDEV_REG, (xdrproc_t)xdr_psdev_reg_x, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_psdev_error, 
	                      (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



db_psdev_error
#if defined __STDC__ | defined __STDCPP__
* db_psdev_unreg_1(arr1 *argp, CLIENT *clnt, DevLong *perr)
#else
*db_psdev_unreg_1(argp, clnt, perr)
	arr1 *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_psdev_error res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_PSDEV_UNREG, (xdrproc_t)xdr_arr1, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_psdev_error, 
	                      (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getdevdomain_1(CLIENT *clnt, DevLong *perr)
#else
db_res *db_getdevfamily_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETDEVDOMAIN, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getdevfamily_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getdevfamily_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETDEVFAMILY, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getdevmember_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getdevmember_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETDEVMEMBER, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getresdomain_1(CLIENT *clnt, DevLong *perr)
#else
db_res *db_getresdomain_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETRESDOMAIN, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getresfamily_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getresfamily_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETRESFAMILY, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getresmember_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getresmember_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETRESMEMBER, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getresreso_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getresreso_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETRESRESO, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getresresoval_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getresresoval_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETRESRESOVAL, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getdsserver_1(CLIENT *clnt, DevLong *perr)
#else
db_res *db_getdsserver_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETSERVER, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_getdspers_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_getdspers_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETPERS, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}

#if defined __STDC__ | defined __STDCPP__
db_res *db_gethost_1(CLIENT *clnt, DevLong *perr)
#else
db_res *db_gethost_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETHOST, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_devinfo_svc *db_deviceinfo_1(DevString *argp,CLIENT *clnt, DevLong *perr)
#else
db_devinfo_svc *db_deviceinfo_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_devinfo_svc res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVINFO, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_devinfo_svc, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_res *db_deviceres_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_res *db_deviceres_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVRES, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



#if defined __STDC__ | defined __STDCPP__
long *db_devicedelete_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
long *db_devicedelete_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static long res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVDEL, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_DevLong, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_psdev_error *db_devicedeleteres_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_psdev_error *db_devicedeleteres_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_psdev_error res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEVDELALLRES, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_psdev_error, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_info_svc *db_stat_1(CLIENT *clnt, DevLong *perr)
#else
db_info_svc *db_stat_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_info_svc res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_INFO, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_db_info_svc, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
long *db_servunreg_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
long *db_servunreg_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static long res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_SVCUNREG, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_DevLong, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



#if defined __STDC__ | defined __STDCPP__
svcinfo_svc *db_servinfo_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
svcinfo_svc *db_servinfo_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static svcinfo_svc res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_SVCINFO, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_svcinfo_svc, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
long *db_servdelete_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
long *db_servdelete_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static long res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_SVCDELETE, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_DevLong, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



#if defined __STDC__ | defined __STDCPP__
db_psdev_error *db_upddev_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_psdev_error *db_upddev_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_psdev_error res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_UPDDEV, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_psdev_error, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_psdev_error *db_updres_1(db_res *argp, CLIENT *clnt, DevLong *perr)
#else
db_psdev_error *db_updres_1(argp, clnt, perr)
	db_res *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_psdev_error res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_UPDRES, (xdrproc_t)xdr_db_res, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_psdev_error, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



#if defined __STDC__ | defined __STDCPP__
db_res *db_secpass_1(CLIENT *clnt, DevLong *perr)
#else
db_res *db_secpass_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_res res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_SECPASS, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_db_res, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_poller_svc *db_getpoll_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
db_poller_svc *db_getpoll_1(clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_poller_svc res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETPOLLER, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_poller_svc, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_delupd_error *db_delete_update_1(db_arr1_array *argp, CLIENT *clnt, DevLong *perr)
#else
db_delupd_error *db_delete_update_1(clnt, perr)
	db_arr1_array *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_delupd_error res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_DEL_UPDATE, (xdrproc_t)xdr_db_arr1_array, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_delupd_error, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}



#if defined __STDC__ | defined __STDCPP__
long *db_initcache_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
long *db_initcache_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static long res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_INITCACHE, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_DevLong, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
db_svcarray_net *db_getdsonhost_1(DevString *argp, CLIENT *clnt, DevLong *perr)
#else
db_svcarray_net *db_getdsonhost_1(argp, clnt, perr)
	DevString *argp;
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	static db_svcarray_net res;
	enum clnt_stat clnt_stat;

	memset((void *)&res, 0, sizeof(res));

	clnt_stat = clnt_call(clnt, DB_GETDSHOST, (xdrproc_t)xdr_nam, 
	                      (caddr_t)argp, (xdrproc_t)xdr_db_svcarray_net, 
		              (caddr_t)&res, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (NULL);
	}
	return (&res);
}


#if defined __STDC__ | defined __STDCPP__
long db_null_proc_1(CLIENT *clnt, DevLong *perr)
#else
long db_null_proc_1(clnt, perr)
	CLIENT *clnt;
	DevLong *perr;
#endif	/* __STDC__ */
{
	enum clnt_stat clnt_stat;

	clnt_stat = clnt_call(clnt,NULLPROC, (xdrproc_t)xdr_void, 
	                      (caddr_t)NULL, (xdrproc_t)xdr_void, 
		              (caddr_t)NULL, TIMEVAL(TimeOut));

	if (clnt_stat != RPC_SUCCESS)
	{
		if (clnt_stat == RPC_TIMEDOUT)
			*perr = DevErr_RPCTimedOut;
		else if (clnt_stat == RPC_CANTENCODEARGS)
			*perr = DbErr_CannotEncodeArguments;
		else if (clnt_stat == RPC_CANTDECODERES)
			*perr = DbErr_CannotDecodeResult;
		else
			*perr = DbErr_RPCreception;
		return (-1);
	}
	return (0);
}

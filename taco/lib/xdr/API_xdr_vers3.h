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
 * File:        API_xdr_vers3.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: XDR definitinos for all services in the
 *		applications programmers interface for the old
 *		version 3. This file is needed for compatibility
 *		reasons.
 *
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	November 1994
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef API_XDR_VERS3_H
#define API_XDR_VERS3_H

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char API_xdrh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/API_xdr_vers3.h,v 1.3 2005-07-25 13:05:45 jkrueger1 Exp $";
#endif /* _IDENT */


#include <macros.h>


/*
 *  xdr data structures used in the dev_putget(),
 *  dev_put() and dev_putget_raw() calls.
 */

struct _server_data_3 {
 	long ds_id;
	long cmd;
	long argin_type;
	long argout_type;
	DevArgument argin;
};
typedef struct _server_data_3 _server_data_3;


struct _client_data_3 {
  	long status;
	long error;
	long argout_type;
	DevArgument argout;
};
typedef struct _client_data_3 _client_data_3;



struct _client_raw_data_3 {
  	long status;
	long error;
	long ser_argout_type;
	long deser_argout_type;
	long xdr_length;
	DevArgument argout;
};
typedef struct _client_raw_data_3 _client_raw_data_3;




/*
 *  internal structures for device services
 */

struct _dev_import_in_3 {
        char *device_name;
};
typedef struct _dev_import_in_3 _dev_import_in_3;


struct _dev_import_out_3 {
        char server_name[LONG_NAME_SIZE];
	long ds_id;
	long status;
	long error;
};
typedef struct _dev_import_out_3 _dev_import_out_3;


struct _dev_free_in_3 {
	long ds_id;
};
typedef struct _dev_free_in_3 _dev_free_in_3;


struct _dev_free_out_3 {
	long status;
	long error;
};
typedef struct _dev_free_out_3 _dev_free_out_3;


/*
 *  structures for dev_cmd_query()
 */

struct _dev_query_in_3 {
  	long                ds_id;
};
typedef struct _dev_query_in_3 _dev_query_in_3;


struct _dev_query_out_3 {
  	u_int           length;
  	_dev_cmd_info   *sequence;
  	char            class_name [20];
  	long            error;
  	long            status;
};
typedef struct _dev_query_out_3 _dev_query_out_3;


struct _manager_data_3 {
        _register_data  msg_info;
        _register_data  db_info;
        long    status;
        long    error;
};
typedef struct _manager_data_3 _manager_data_3;

					 
					 
#ifdef __cplusplus
extern "C" {
#endif					 					 
bool_t _DLLFunc xdr__server_data_3 PT_((XDR *xdrs,_server_data_3 *objp));
bool_t _DLLFunc xdr__client_data_3 PT_((XDR *xdrs, _client_data_3 *objp));
bool_t _DLLFunc xdr__client_raw_data_3 PT_((XDR *xdrs, _client_raw_data_3 *objp));
bool_t _DLLFunc xdr__dev_import_in_3 PT_((XDR *xdrs, _dev_import_in_3 *objp));
bool_t _DLLFunc xdr__dev_import_out_3 PT_((XDR *xdrs, _dev_import_out_3 *objp));
bool_t _DLLFunc xdr__dev_free_in_3 PT_((XDR *xdrs, _dev_free_in_3 *objp));
bool_t _DLLFunc xdr__dev_free_out_3 PT_((XDR *xdrs, _dev_free_out_3 *objp));
bool_t _DLLFunc xdr__dev_query_in_3 PT_((XDR *xdrs, _dev_query_in_3 *objp));
bool_t _DLLFunc xdr__dev_query_out_3 PT_((XDR *xdrs, _dev_query_out_3 *objp));
#ifdef __cplusplus
#ifdef _UCC
bool_t _DLLFunc xdr__manager_data_3 (...); 
#else
bool_t _DLLFunc xdr__manager_data_3 PT_((XDR *xdrs, _manager_data_3 *objp));
#endif /* _UCC */
#else
bool_t _DLLFunc xdr__manager_data_3 PT_((XDR *xdrs, _manager_data_3 *objp));
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif
					
#endif /* _api_xdr_vers3_h */

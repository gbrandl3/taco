/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        API_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: XDR definitinos for all services in the
 *		applications programmers interface.
 *
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.7 $
 *
 * Date:	$Date: 2008-12-02 09:19:43 $
 *
 *******************************************************************-*/

#ifndef API_XDR_H
#define API_XDR_H

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char API_xdrh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/API_xdr.h,v 1.7 2008-12-02 09:19:43 jkrueger1 Exp $";
#endif /* _IDENT */

#include <dev_xdr.h>
#include <macros.h>

/*
 * Structures for variable argument passing for API calls.
 * Will avoid recompilation of the system when adding variables.
 */

#ifdef __cplusplus
extern "C" {
#endif					   

struct DevVarArgument {
	DevLong 	argument_type;
	DevArgument 	argument;
};
typedef struct DevVarArgument DevVarArgument;

struct DevVarArgumentArray {
        DevULong 	length;
        DevVarArgument  *sequence;
};
typedef struct DevVarArgumentArray DevVarArgumentArray;

/*
 *  xdr data structures used in the dev_putget(),
 *  dev_put() and dev_putget_raw() calls.
 */

struct _server_data {
 	DevLong 		ds_id;
	DevLong 		cmd;
	DevLong 		argin_type;
	DevLong 		argout_type;
	DevArgument 		argin;
 	DevLong 		access_right;
 	DevLong 		client_id;
	DevVarArgumentArray 	var_argument;
};
typedef struct _server_data _server_data;

struct _client_data {
  	DevLong 		status;
	DevLong 		error;
	DevLong 		argout_type;
	DevArgument 		argout;
	DevVarArgumentArray 	var_argument;
};
typedef struct _client_data _client_data;



struct _client_raw_data {
  	DevLong 		status;
	DevLong 		error;
	DevLong 		ser_argout_type;
	DevLong 		deser_argout_type;
	DevLong 		xdr_length;
	DevArgument 		argout;
	DevVarArgumentArray 	var_argument;
};
typedef struct _client_raw_data _client_raw_data;

/*
 *  internal structures for device services
 */

struct _dev_import_in {
        DevString 		device_name;
	DevLong 		access_right;
	DevLong 		client_id;
	DevLong 		connection_id;
	DevVarArgumentArray 	var_argument;
};
typedef struct _dev_import_in _dev_import_in;


struct _dev_import_out {
	DevChar 		server_name[LONG_NAME_SIZE];
	DevLong 		ds_id;
	DevLong 		status;
	DevLong 		error;
	DevVarArgumentArray 	var_argument;
};
typedef struct _dev_import_out _dev_import_out;


struct _dev_free_in {
	DevLong 		ds_id;
	DevLong 		access_right;
	DevLong 		client_id;
	DevVarArgumentArray 	var_argument;
};
typedef struct _dev_free_in _dev_free_in;


struct _dev_free_out {
	DevLong 		status;
	DevLong 		error;
	DevVarArgumentArray 	var_argument;
};
typedef struct _dev_free_out _dev_free_out;


struct _server_admin {
	DevLong cmd;
	DevLong debug_bits;
};
typedef struct _server_admin _server_admin;



/*
 *  structures for dev_cmd_query()
 */

struct _dev_query_in {
  	DevLong             ds_id;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_query_in _dev_query_in;


struct _dev_cmd_info {
	DevLong  cmd;
  	DevLong  in_type;
  	DevLong  out_type;
};
typedef struct _dev_cmd_info _dev_cmd_info;


struct _dev_event_info {
	DevLong  event;
  	DevLong  out_type;
};
typedef struct _dev_event_info _dev_event_info;

struct _dev_query_out {
  	DevULong           	length;
  	_dev_cmd_info   	*sequence;
  	char            	class_name [SHORT_NAME_SIZE];
  	DevLong         	error;
  	DevLong         	status;
	DevVarArgumentArray 	var_argument;
};
typedef struct _dev_query_out _dev_query_out;

/*
 * structure for query event
 */

struct _dev_queryevent_out {
  	DevULong           	length;
  	_dev_event_info 	*sequence;
  	DevChar            	class_name [SHORT_NAME_SIZE];
  	DevLong         	error;
  	DevLong         	status;
	DevVarArgumentArray 	var_argument;
};
typedef struct _dev_queryevent_out _dev_queryevent_out;

/*
 *  structures for message service
 */

struct _msg_data {
	DevString devserver_name;
  	DevString host_name;
	DevLong   prog_number;
  	DevString display;
	DevShort  type_of_message;
 	DevString message_buffer;
};
typedef struct _msg_data _msg_data;


struct _msg_out {
	DevLong status;
	DevLong error;
};
typedef struct _msg_out _msg_out;


/*
 *  structures for network manager services
 */

struct _register_data {
	DevString host_name;
	DevLong   prog_number;
	DevLong   vers_number;
};
typedef struct _register_data _register_data;


struct _manager_data {
	_register_data  	msg_info;
	_register_data  	db_info;
	DevLong    		status;
	DevLong    		error;
	DevLong			security;
	DevVarArgumentArray 	var_argument;
};
typedef struct _manager_data _manager_data;


struct _msg_manager_data {
	DevString    dshome;
	DevString    display;
};
typedef struct _msg_manager_data _msg_manager_data;					   
					   
bool_t _DLLFunc xdr_DevVarArgument PT_((XDR *xdrs, DevVarArgument *objp));
bool_t _DLLFunc xdr_DevVarArgumentArray PT_((XDR *xdrs, DevVarArgumentArray *objp));
bool_t _DLLFunc xdr__register_data PT_((XDR *xdrs, _register_data *objp));
bool_t _DLLFunc xdr__manager_data PT_((XDR *xdrs, _manager_data *objp));
bool_t _DLLFunc xdr__server_data PT_((XDR *xdrs, _server_data *objp));
bool_t _DLLFunc xdr__client_data PT_((XDR *xdrs, _client_data *objp));
bool_t _DLLFunc xdr__client_raw_data PT_((XDR *xdrs, _client_raw_data *objp));
bool_t _DLLFunc xdr__dev_import_in PT_((XDR *xdrs, _dev_import_in *objp));
bool_t _DLLFunc xdr__dev_import_out PT_((XDR *xdrs, _dev_import_out *objp));
bool_t _DLLFunc xdr__dev_free_in PT_((XDR *xdrs, _dev_free_in *objp));
bool_t _DLLFunc xdr__dev_free_out PT_((XDR *xdrs, _dev_free_out *objp));
bool_t _DLLFunc xdr__server_admin PT_((XDR *xdrs, _server_admin *objp));
bool_t _DLLFunc xdr__dev_query_in PT_((XDR *xdrs, _dev_query_in *objp));
bool_t _DLLFunc xdr__dev_cmd_info PT_((XDR *xdrs, _dev_cmd_info *objp));
bool_t _DLLFunc xdr__dev_query_out PT_((XDR *xdrs, _dev_query_out *objp));
bool_t _DLLFunc xdr__msg_data PT_((XDR *xdrs, _msg_data *objp));
bool_t _DLLFunc xdr__msg_out PT_((XDR *xdrs, _msg_out *objp));
bool_t _DLLFunc xdr__dev_event_info PT_((XDR *xdrs, _dev_event_info *objp));
bool_t _DLLFunc xdr__msg_manager_data PT_((XDR *xdrs, _msg_manager_data *objp));
bool_t _DLLFunc xdr__dev_queryevent_out PT_((XDR *xdrs, _dev_queryevent_out *objp));
/*
bool_t _DLLFunc xdr__asynch_client_data PT_((XDR *xdrs, _asynch_client_data *objp));
bool_t _DLLFunc xdr__asynch_client_raw_data PT_((XDR *xdrs, _asynch_client_raw_data *objp));
*/
#ifdef __cplusplus
}
#endif

#endif /* _api_xdr_h */

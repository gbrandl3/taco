/*+*******************************************************************

 File:          API_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   xdr definitinos for all services in the
		applications programmers interface.

 Author(s):	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.4 $

 Date:		 $Date: 2004-09-17 07:56:18 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _api_xdr_h
#define _api_xdr_h

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char API_xdrh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/xdr/API_xdr.h,v 1.4 2004-09-17 07:56:18 jkrueger1 Exp $";
#endif /* _IDENT */


#include <macros.h>

/*
 * Structures for variable argument passing for API calls.
 * Will avoid recompilation of the system when adding variables.
 */

#ifdef __cplusplus
extern "C" {
#endif					   

struct DevVarArgument {
	long 		argument_type;
	DevArgument 	argument;
};
typedef struct DevVarArgument DevVarArgument;

struct DevVarArgumentArray {
        u_int 		length;
        DevVarArgument  *sequence;
};
typedef struct DevVarArgumentArray DevVarArgumentArray;

/*
 *  xdr data structures used in the dev_putget(),
 *  dev_put() and dev_putget_raw() calls.
 */

struct _server_data {
 	long ds_id;
	long cmd;
	long argin_type;
	long argout_type;
	DevArgument argin;
 	long access_right;
 	long client_id;
	DevVarArgumentArray var_argument;
};
typedef struct _server_data _server_data;

struct _client_data {
  	long status;
	long error;
	long argout_type;
	DevArgument argout;
	DevVarArgumentArray var_argument;
};
typedef struct _client_data _client_data;



struct _client_raw_data {
  	long status;
	long error;
	long ser_argout_type;
	long deser_argout_type;
	long xdr_length;
	DevArgument argout;
	DevVarArgumentArray var_argument;
};
typedef struct _client_raw_data _client_raw_data;

/*
 *  internal structures for device services
 */

struct _dev_import_in {
        char *device_name;
	long access_right;
	long client_id;
	long connection_id;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_import_in _dev_import_in;


struct _dev_import_out {
	char server_name[LONG_NAME_SIZE];
	long ds_id;
	long status;
	long error;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_import_out _dev_import_out;


struct _dev_free_in {
	long ds_id;
	long access_right;
	long client_id;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_free_in _dev_free_in;


struct _dev_free_out {
	long status;
	long error;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_free_out _dev_free_out;


struct _server_admin {
	long cmd;
	long debug_bits;
};
typedef struct _server_admin _server_admin;



/*
 *  structures for dev_cmd_query()
 */

struct _dev_query_in {
  	long                ds_id;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_query_in _dev_query_in;


struct _dev_cmd_info {
	long  cmd;
  	long  in_type;
  	long  out_type;
};
typedef struct _dev_cmd_info _dev_cmd_info;


struct _dev_event_info {
	long  event;
  	long  out_type;
};
typedef struct _dev_event_info _dev_event_info;

struct _dev_query_out {
  	u_int           length;
  	_dev_cmd_info   *sequence;
  	char            class_name [SHORT_NAME_SIZE];
  	long            error;
  	long            status;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_query_out _dev_query_out;

/*
 * structure for query event
 */

struct _dev_queryevent_out {
  	u_int           length;
  	_dev_event_info   *sequence;
  	char            class_name [SHORT_NAME_SIZE];
  	long            error;
  	long            status;
	DevVarArgumentArray var_argument;
};
typedef struct _dev_queryevent_out _dev_queryevent_out;

/*
 *  structures for message service
 */

struct _msg_data {
	char *devserver_name;
  	char *host_name;
	long  prog_number;
  	char *display;
	short type_of_message;
 	char *message_buffer;
};
typedef struct _msg_data _msg_data;


struct _msg_out {
	long status;
	long error;
};
typedef struct _msg_out _msg_out;


/*
 *  structures for network manager services
 */

struct _register_data {
	char    *host_name;
	long    prog_number;
	long    vers_number;
};
typedef struct _register_data _register_data;


struct _manager_data {
	_register_data  msg_info;
	_register_data  db_info;
	long    status;
	long    error;
	long	security;
	DevVarArgumentArray var_argument;
};
typedef struct _manager_data _manager_data;


struct _msg_manager_data {
	char    *dshome;
	char    *display;
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

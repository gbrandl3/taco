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
 * File:   	asc_api.c
 *
 * Project:   	ASCII based interface to device server API
 *
 * Description: The code for dev_get_ascii, dev_put_ascii and dev_putget_ascii
 *
 * Author(s):   Faranguiss Poncet
 *		$Author: jkrueger1 $
 *
 * Original:   	November 1996
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:	$Date: 2006-09-18 22:18:41 $
 *
 *****************************************************************************/

/**
 * @defgroup asciiAPI ASCII based device API
 * @ingroup API
 *
 * The "ASCII based dev api" has been designed as a shared library
 * which can dynamically be loaded (using dynamic loader functions) in any C
 * program. It has been loaded and used inside 'veetest' application to access
 * the TACO control system. It will also be used in "xdevmenu" application to
 * access devices.
 * 
 * The aim of this library is to give an ASCII based interface to the device
 * server API. It means one can execute a command on a device server simply by
 * specifying the device name and the command name with two strings. He (she)
 * is returned the output result and the eventual errors in the form of string
 * as well. If the device server command requires an input argument, it is also
 * specified through a string.
 * 
 * This library has simply one C module in which the 3 functions of the interface
 * will be defined (asc_dev_get, asc_dev_put, asc_dev_putget).

 * The reason is that a shared library cannot be debugged (see "Programming with HPUX" doc) 
 * and the only way of
 * debugging is to debug the archived version and then rebuild it in shared mode.
 * So the archived version has only be used in the debugging phase. It will not
 * be distributed nor supported.

 * The TACO ASCII based API library is used to give the clients a string (C type)
 * based interface to the device server commands.
 * 
 * The name of the device, the command to be executed on that device, the input
 * and output parameters all are specified in strings.
 * 
 * This library uses the TACO type conversion library to do the necessary type
 * conversion to (output data) and from (input data) strings.
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <stdlib.h>
#include <string.h>

#include <API.h>
#include <tc_api.h>
#include <asc_api.h>

#ifdef __hpux
#include <dl.h>
#else
#include <dlfcn.h>
#endif /* __hpux */


#ifdef __hpux
shl_t _asc_TC_shl;
#else
void *_asc_TC_shl;
#endif /* __hpux */


#define ASC_TRUE           1
#define ASC_FALSE          0
#define ASC_MAX_DEV        20
#define ASC_EMPTY_DEV      "NO_DEVICE"
#define ASC_MAX_STR        5001


typedef struct
         {
	   char            dev_name[201];
	   devserver       ds;
	   DevVarCmdArray  varcmdarr;
         } _asc_dev_info_type;


static  _asc_dev_info_type    *asc_dev_array_ptr = NULL;
static  int                   _asc_nb_block_dev_array = 1;
static  int                   _asc_library_loaded = ASC_FALSE;


static long _asc_load_library()
{
	long		err;
	db_resource	res;
	char		*lib_path,
			*dev_name;
   
/* 
 * Try to find the path for the type conversion library in Data Base 
 */
	if ( !db_import(&err))
	{
#ifdef __hpux
		dev_name = "sys/libtc/hpux10.2";
#else
		dev_name = "sys/libtc/solaris";
#endif /* __hpux */

		lib_path = NULL;
#ifndef linux
		res.resource_name = "path";
		res.resource_adr = &lib_path;
		res.resource_type = D_STRING_TYPE;
		db_getresource(dev_name,&res,1,&err);
#endif

/* 
 * Did not find the libpath in static database so set it to default 
 */
		if (lib_path == (char *)NULL)
#ifdef __hpux
         		lib_path = TC_HPUX_DEF_PATH;
#else
         		lib_path = TC_SOLARIS_DEF_PATH;
#endif /* __hpux */
	}
	else
	{
/* 
 * Did not import the data base so set lib_path to default 
 */
#ifdef __hpux
		lib_path = TC_HPUX_DEF_PATH;
#else
		lib_path = TC_SOLARIS_DEF_PATH;
#endif /* __hpux */
	}

/*
 * Load the type conversion shared library 
 */

#ifdef __hpux
	_asc_TC_shl = shl_load(lib_path, BIND_IMMEDIATE | DYNAMIC_PATH, 0);
#else
	_asc_TC_shl = dlopen(lib_path, RTLD_NOW);
#endif /* __hpux */
	if (_asc_TC_shl == NULL)
	{
		fprintf(stderr,"_asc_load_library : Cannot load the type conversion shared library(lib_path=%s)",lib_path);
		return(-1);
	}

	return(0);
}

static int _asc_lookup_dev(char  *dev_name)
{
	int    	ind_dev = 0, 
		dev_found = ASC_FALSE;

	while ( (dev_found == ASC_FALSE) && (ind_dev < (ASC_MAX_DEV * _asc_nb_block_dev_array)) )
	{
		if ( strcasecmp(asc_dev_array_ptr[ind_dev].dev_name, dev_name) == 0 )
			dev_found = ASC_TRUE;
		else
			ind_dev++;
	}

	if (dev_found == ASC_TRUE)
		return(ind_dev);
	else
	{
		ind_dev =0;
		dev_found = ASC_FALSE;

		while ( (dev_found == ASC_FALSE) && (ind_dev < (ASC_MAX_DEV * _asc_nb_block_dev_array)) )
		{
			if ( strcasecmp(asc_dev_array_ptr[ind_dev].dev_name, dev_name) == 0 )
				dev_found = ASC_TRUE;
			else if ( strcasecmp(asc_dev_array_ptr[ind_dev].dev_name, ASC_EMPTY_DEV)==0)
				dev_found = ASC_TRUE;
			else
				ind_dev++;
		}
		return(ind_dev);
	}
}

static long  _asc_dev_array_realloc()
{
	_asc_dev_info_type  	*dev_array_safe;
	int                 	new_size, 
				index_dev;

	new_size = ((_asc_nb_block_dev_array + 1) * ASC_MAX_DEV) * sizeof(_asc_dev_info_type);
	dev_array_safe = asc_dev_array_ptr;

	asc_dev_array_ptr = (_asc_dev_info_type *) realloc(asc_dev_array_ptr,new_size);
	if (asc_dev_array_ptr == NULL)
	{
		asc_dev_array_ptr = dev_array_safe;
		return(-1);
	}
	else
	{
		for ( index_dev=(ASC_MAX_DEV * _asc_nb_block_dev_array);
            		index_dev < (ASC_MAX_DEV * (_asc_nb_block_dev_array+1));
            		index_dev++)
			strcpy(asc_dev_array_ptr[index_dev].dev_name, ASC_EMPTY_DEV);
		_asc_nb_block_dev_array++;
		return(0);
	}
}

static long _asc_lookup_cmd(int index_dev, const char *cmd_name, long *cmd_code, long *in_type, long *out_type)
{
	int             ind_cmd, 
			cmd_found;
	DevVarCmdArray  cmd_list;

	cmd_found = ASC_FALSE;
	ind_cmd = 0;
	cmd_list = asc_dev_array_ptr[index_dev].varcmdarr;

	while ( (cmd_found == ASC_FALSE) && (ind_cmd < cmd_list.length) )
	{
		if ( strcasecmp(cmd_list.sequence[ind_cmd].cmd_name, cmd_name) == 0)
			cmd_found = ASC_TRUE;
		else
			ind_cmd++;
	}

	if (cmd_found == ASC_TRUE)
	{
		*cmd_code = cmd_list.sequence[ind_cmd].cmd;
		*in_type = cmd_list.sequence[ind_cmd].in_type;
		*out_type = cmd_list.sequence[ind_cmd].out_type;
		return(0);
	}
	else
		return(-1);
}

static long _asc_dev_connect(char *dev_name, const char *cmd_name, devserver *ds_ptr, long  *cmd_code,
				       long  *in_type, long *out_type, char  **str_err )
{
	int		index_dev, 
			ind_cmd,
			cmd_found;
	long		status, 
			error;
	devserver	ds;
	DevVarCmdArray	cmd_arr;
	char		*error_str;

	if (asc_dev_array_ptr == NULL)
	{
		asc_dev_array_ptr = (_asc_dev_info_type *) malloc (sizeof(_asc_dev_info_type) * ASC_MAX_DEV);
		for (index_dev=0; index_dev < ASC_MAX_DEV; index_dev++)
			strcpy(asc_dev_array_ptr[index_dev].dev_name, ASC_EMPTY_DEV);
	}

	index_dev = _asc_lookup_dev(dev_name);
	if (index_dev >= (ASC_MAX_DEV * _asc_nb_block_dev_array))
	{
		status = _asc_dev_array_realloc();
		if (status == -1)
		{
			error_str = "Too many devices already imported. Cannot realloc device array.";
			*str_err = (char *) malloc(strlen(error_str) + 1);
			strcpy(*str_err, error_str);
			return(DS_NOTOK);
		}
		else
			index_dev = _asc_lookup_dev(dev_name);
	}

	if (strcasecmp(asc_dev_array_ptr[index_dev].dev_name, dev_name) == 0 )
	{ 
/* 
 * device already in the device array 
 */
		*ds_ptr = asc_dev_array_ptr[index_dev].ds;
		status = _asc_lookup_cmd(index_dev, cmd_name,cmd_code,in_type,out_type);

		if (status == DS_NOTOK)
		{
			error_str = "Cannot find the command for this device.";
			*str_err = (char *) malloc(strlen(error_str) + 1);
			strcpy(*str_err, error_str);
			return(DS_NOTOK);
		}
	}
	else 
	{
/* 
 * device is not yet in the dev array so should do dev_import 
 */
		status = dev_import(dev_name,0,&ds,&error);
		if (status == DS_NOTOK)
		{
			status = dev_import(dev_name,0,&ds,&error);
			if (status == DS_NOTOK)
			{
				error_str = dev_error_str (error);
				*str_err = error_str;
				return(DS_NOTOK);
			}
		}

		status = dev_cmd_query(ds,&cmd_arr,&error);
		if (status == DS_NOTOK)
		{
			error_str = dev_error_str (error);
			*str_err = error_str;
			return (DS_NOTOK);
		}

		strcpy(asc_dev_array_ptr[index_dev].dev_name, dev_name);
		asc_dev_array_ptr[index_dev].ds = ds;
		*ds_ptr = ds;
		asc_dev_array_ptr[index_dev].varcmdarr = cmd_arr;

		status = _asc_lookup_cmd(index_dev, cmd_name,cmd_code,in_type,out_type);
		if (status == DS_NOTOK)
		{
			error_str = "Cannot find the command for this device.";
			*str_err = (char *) malloc(strlen(error_str) + 1);
			strcpy(*str_err, error_str);
			return(DS_NOTOK);
		}
	} 
	return(0);
}

static long _asc_convert_outtype ( const char *type_name, const void *data_to_convert, char *res_function, char *err_str)
{
	char  func_name[201];
	long  (*p_conv_func)();

	snprintf(func_name, sizeof(func_name), "tc_%s_to_str", type_name);

#ifdef __hpux
	if (shl_findsym(&_asc_TC_shl, func_name, TYPE_PROCEDURE, &p_conv_func) == -1)
#else
	p_conv_func = (long (*)()) dlsym(_asc_TC_shl, func_name);
	if (p_conv_func == NULL)
#endif /* __hpux */
	{
		sprintf(err_str, "_asc_convert_outtype : cannot resolve symbol = %s", func_name);
		return(DS_NOTOK);
	}

	if ( (*p_conv_func)(data_to_convert, res_function) != 0 )
	{
		strcpy(err_str, "_asc_convert_outtype : error returned by conversion function.");
		return(DS_NOTOK);
	}
	return(DS_OK);
}

static long _asc_convert_intype ( const char *type_name, const char *str_in, void *res_convert, char *err_str )
{
	char  func_name[201];
	long  (*p_conv_func)();

	snprintf(func_name, sizeof(func_name), "tc_str_to_%s", type_name);

#ifdef __hpux
	if (shl_findsym(&_asc_TC_shl, func_name, TYPE_PROCEDURE, &p_conv_func) == -1)
#else
	p_conv_func = (long (*)()) dlsym(_asc_TC_shl, func_name);
	if (p_conv_func == NULL)
#endif /* __hpux */
	{
		sprintf(err_str, "_asc_convert_intype : cannot resolve symbol = %s", func_name);
		return(DS_NOTOK);
	}

	if ( (*p_conv_func)(str_in, res_convert) != 0 )
	{
		strcpy(err_str, "_asc_convert_intype : error returned by conversion function.");
		return(DS_NOTOK);
	}
	return(DS_OK);
}

/**@ingroup asciiAPI
 *
 * @param dev_name
 * @param cmd_name
 * @param out_arg
 * @param str_err
 *
 * @return DS_OK or DS_NOTOK
 *
 * @see dev_put_ascii
 */
long dev_get_ascii(char *dev_name, char *cmd_name, char **out_arg, char **str_err)
{

	devserver                        ds;
	char                             *error_str;
	long                             status,error;
	void                             *out_data;
	long                             cmd;
	long                             in_type;
	long                             out_type;
        char                             type_to_convert[101];
        char                             conv_err_str[301];
        char                             str_local[ASC_MAX_STR];
        char                             *res_string;


	void                             *d_void;
        DevBoolean                       d_boolean;
	DevUShort                        d_ushort;
	DevShort                         d_short;
	DevULong                         d_ulong;
	DevLong                          d_long;
	DevFloat                         d_float;
	DevDouble                        d_double;
	DevString                        d_string;
	DevIntFloat                      d_intfloat;
	DevFloatReadPoint                d_flrp;
	DevStateFloatReadPoint           d_stflrp;
	DevLongReadPoint                 d_lnrp;
	DevDoubleReadPoint               d_dblrp;
	DevVarCharArray                  d_char_arr;
        DevVarStringArray                d_string_arr;
        DevVarUShortArray                d_ushort_arr;
        DevVarShortArray                 d_short_arr;
        DevVarULongArray                 d_ulong_arr;
        DevVarLongArray                  d_long_arr;
        DevVarFloatArray                 d_float_arr;
        DevVarDoubleArray                d_dbl_arr;
        DevVarFloatReadPointArray        d_flrp_arr;
        DevVarStateFloatReadPointArray   d_stflrp_arr;
        DevVarLongReadPointArray         d_lnrp_arr;



        status = 0;

	if (_asc_library_loaded == ASC_FALSE)
           status = _asc_load_library();

        if (status == 0)
           _asc_library_loaded = ASC_TRUE;
        else
        {
           strcpy(*str_err, "dev_get_ascii : Cannot load the shared libraries.");
	   return(-1);
        }
           
	if (dev_name == NULL)
	{
           strcpy(*str_err, "dev_get_ascii : Device Name is not specified.");
	   return(-1);
	}

        status = _asc_dev_connect(dev_name, cmd_name, &ds, &cmd, &in_type,
						      &out_type, &error_str);

	if (status == -1)
	{
	   strcpy(*str_err, error_str);
	   if (error_str != NULL) free(error_str);
	   return(-1);
	}

        switch (out_type)
        {
               case (D_VOID_TYPE):
                        out_data = (void *) &d_void;
                        strcpy(type_to_convert, "DevVoid");
                        break;
               case (D_BOOLEAN_TYPE):
			d_boolean = (DevBoolean) 0;
                        out_data = (void *) &d_boolean;
                        strcpy(type_to_convert, "DevBoolean");
                        break;
               case (D_USHORT_TYPE):
			d_ushort = 0;
                        out_data = (void *) &d_ushort;
                        strcpy(type_to_convert, "DevUShort");
                        break;
               case (D_SHORT_TYPE):
			d_short = 0;
                        out_data = (void *) &d_short;
                        strcpy(type_to_convert, "DevShort");
                        break;
               case (D_ULONG_TYPE):
			d_ulong = 0;
                        out_data = (void *) &d_ulong;
                        strcpy(type_to_convert, "DevULong");
                        break;
               case (D_LONG_TYPE):
			d_long = 0;
                        out_data = (void *) &d_long;
                        strcpy(type_to_convert, "DevLong");
                        break;
               case (D_FLOAT_TYPE):
			d_float = 0.0;
                        out_data = (void *) &d_float;
                        strcpy(type_to_convert, "DevFloat");
                        break;
               case (D_DOUBLE_TYPE):
			d_double = 0.0;
                        out_data = (void *) &d_double;
                        strcpy(type_to_convert, "DevDouble");
                        break;
               case (D_STRING_TYPE):
			d_string = NULL;
                        out_data = (void *) &d_string;
                        strcpy(type_to_convert, "DevString");
                        break;
               case (D_INT_FLOAT_TYPE):
			d_intfloat.state = 0;
                        d_intfloat.value = 0.0;
                        out_data = (void *) &d_intfloat;
                        strcpy(type_to_convert, "DevIntFloat");
                        break;
               case (D_FLOAT_READPOINT):
			d_flrp.set = 0.0;
                        d_flrp.read = 0.0;
                        out_data = (void *) &d_flrp;
                        strcpy(type_to_convert, "DevFloatReadPoint");
                        break;
               case (D_STATE_FLOAT_READPOINT):
			d_stflrp.state = 0;
			d_stflrp.set = 0.0;
			d_stflrp.read = 0.0;
                        out_data = (void *) &d_stflrp;
                        strcpy(type_to_convert, "DevStateFloatReadPoint");
                        break;
               case (D_LONG_READPOINT):
			d_lnrp.set = 0;
			d_lnrp.read = 0;
                        out_data = (void *) &d_lnrp;
                        strcpy(type_to_convert, "DevLongReadPoint");
                        break;
               case (D_DOUBLE_READPOINT):
			d_dblrp.set = 0.0;
			d_dblrp.read = 0.0;
                        out_data = (void *) &d_dblrp;
                        strcpy(type_to_convert, "DevDoubleReadPoint");
                        break;
               case (D_VAR_CHARARR):
			d_char_arr.length = 0;
                        d_char_arr.sequence = NULL;
                        out_data = (void *) &d_char_arr;
                        strcpy(type_to_convert, "DevVarCharArray");
                        break;
               case (D_VAR_STRINGARR):
			d_string_arr.length = 0;
                        d_string_arr.sequence = NULL;
                        out_data = (void *) &d_string_arr;
                        strcpy(type_to_convert, "DevVarStringArray");
                        break;
               case (D_VAR_USHORTARR):
			d_ushort_arr.length = 0;
                        d_ushort_arr.sequence = NULL;
                        out_data = (void *) &d_ushort_arr;
                        strcpy(type_to_convert, "DevVarUShortArray");
                        break;
               case (D_VAR_SHORTARR):
			d_short_arr.length = 0;
                        d_short_arr.sequence = NULL;
                        out_data = (void *) &d_short_arr;
                        strcpy(type_to_convert, "DevVarShortArray");
                        break;
               case (D_VAR_ULONGARR):
			d_ulong_arr.length = 0;
                        d_ulong_arr.sequence = NULL;
                        out_data = (void *) &d_ulong_arr;
                        strcpy(type_to_convert, "DevVarULongArray");
                        break;
               case (D_VAR_LONGARR):
			d_long_arr.length = 0;
                        d_long_arr.sequence = NULL;
                        out_data = (void *) &d_long_arr;
                        strcpy(type_to_convert, "DevVarLongArray");
                        break;
               case (D_VAR_FLOATARR):
			d_float_arr.length = 0;
                        d_float_arr.sequence = NULL;
                        out_data = (void *) &d_float_arr;
                        strcpy(type_to_convert, "DevVarFloatArray");
                        break;
               case (D_VAR_DOUBLEARR):
			d_dbl_arr.length = 0;
                        d_dbl_arr.sequence = NULL;
                        out_data = (void *) &d_dbl_arr;
                        strcpy(type_to_convert, "DevVarDoubleArray");
                        break;
               case (D_VAR_FRPARR):
			d_flrp_arr.length = 0;
                        d_flrp_arr.sequence = NULL;
                        out_data = (void *) &d_flrp_arr;
                        strcpy(type_to_convert, "DevVarFloatReadPointArray");
                        break;
               case (D_VAR_SFRPARR):
			d_stflrp_arr.length = 0;
                        d_stflrp_arr.sequence = NULL;
                        out_data = (void *) &d_stflrp_arr;
                        strcpy(type_to_convert, "DevVarStateFloatReadPointArray");
                        break;
               case (D_VAR_LRPARR):
			d_lnrp_arr.length = 0;
                        d_lnrp_arr.sequence = NULL;
                        out_data = (void *) &d_lnrp_arr;
                        strcpy(type_to_convert, "DevVarLongReadPointArray");
                        break;
		default:
	                strcpy(*str_err, "dev_get_ascii : output data type not implemented !!");
	                return(-1);
			break;
        }

	status = dev_putget(ds, cmd, NULL, D_VOID_TYPE, 
			    (void*) out_data, out_type, &error);
	if (status == -1)
	{
	   status = dev_putget(ds, cmd, NULL, D_VOID_TYPE, 
			           (void *) out_data, out_type, &error);
           if (status == -1)
           {
	      error_str = dev_error_str (error);
	      strcpy(*str_err, error_str);
	      if (error_str != NULL) free(error_str);
	      return(-1);
	   }
	}

        status = _asc_convert_outtype (type_to_convert, out_data, 
                                                        (char *) str_local,
                                                        conv_err_str );

        if (status == 0)
        {
	   strcpy(*str_err, "dev_get_ascii succeeded.");
	   strcpy(*out_arg, str_local);
           switch (out_type)
           {
               case (D_STRING_TYPE):
               case (D_VAR_CHARARR):
               case (D_VAR_STRINGARR):
               case (D_VAR_USHORTARR):
               case (D_VAR_SHORTARR):
               case (D_VAR_ULONGARR):
               case (D_VAR_LONGARR):
               case (D_VAR_FLOATARR):
               case (D_VAR_DOUBLEARR):
               case (D_VAR_FRPARR):
               case (D_VAR_SFRPARR):
               case (D_VAR_LRPARR): status = dev_xdrfree(out_type, out_data,
                                                                   &error);
                                    break;

               default: ;
           };

           if (status != 0)
           {
              strcat(*str_err, "dev_get_ascii : cannot free memory allocated");
              strcat(*str_err, " by dev_putget. ");
              strcat(*str_err, "dev_get_ascii : dev_xdrfree call failed.");
           };
	   return(0);
        }
        else
        {
           strcpy(*str_err, "dev_get_ascii : output argument type conversion failed.");
           strcat(*str_err, conv_err_str);
	   return(-1);
        }
}
 


/**@ingroup asciiAPI
 *
 * @param dev_name
 * @param cmd_name
 * @param in_arg
 * @param str_err
 *
 * @return DS_OK or DS_NOTOK
 *
 * @see dev_putget_ascii
 * @see dev_put
 */

long dev_put_ascii(char *dev_name, char *cmd_name, char *in_arg, char **str_err)
{
	devserver                        ds;
	char                             *error_str;
	long                             status,error;
	void                             *in_data;
	long                             cmd;
	long                             in_type;
	long                             out_type;
        char                             type_to_convert[101];
        char                             conv_err_str[301];
        char                             str_local[ASC_MAX_STR];
        char                             *res_string;
	int                              ind;


	DevBoolean                       d_boolean;
	DevUShort                        d_ushort;
	DevShort                         d_short;
	DevULong                         d_ulong;
	DevLong                          d_long;
	DevFloat                         d_float;
	DevDouble                        d_double;
	DevString                        d_string;
	DevIntFloat                      d_intfloat;
	DevFloatReadPoint                d_flrp;
	DevStateFloatReadPoint           d_stflrp;
	DevLongReadPoint                 d_lnrp;
	DevDoubleReadPoint               d_dblrp;
	DevVarCharArray                  d_char_arr;
        DevVarStringArray                d_string_arr;
        DevVarUShortArray                d_ushort_arr;
        DevVarShortArray                 d_short_arr;
        DevVarULongArray                 d_ulong_arr;
        DevVarLongArray                  d_long_arr;
        DevVarFloatArray                 d_float_arr;
        DevVarDoubleArray                d_dbl_arr;
/***************************************************
        DevVarFloatReadPointArray        d_flrp_arr;
        DevVarStateFloatReadPointArray   d_stflrp_arr;
        DevVarLongReadPointArray         d_lnrp_arr;
****************************************************/
        DevMulMove                       d_mulmove;
        DevMotorLong                     d_motor_l;
	DevMotorFloat                    d_motor_f;



        status = 0;

	if (_asc_library_loaded == ASC_FALSE)
           status = _asc_load_library();

        if (status == 0)
           _asc_library_loaded = ASC_TRUE;
        else
        {
           strcpy(*str_err, "dev_put_ascii : Cannot load the shared libraries.");
	   return(-1);
        }
           
	if (dev_name == NULL)
	{
           strcpy(*str_err, "dev_put_ascii : Device Name is not specified.");
	   return(-1);
	}

        status = _asc_dev_connect(dev_name, cmd_name, &ds, &cmd, &in_type,
						      &out_type, &error_str);

	if (status == -1)
	{
	   strcpy(*str_err, error_str);
	   if (error_str != NULL) free(error_str);
	   return(-1);
	}

        switch (in_type)
        {
               case (D_VOID_TYPE):
                        in_data = (void *) NULL;
                        strcpy(type_to_convert, "DevVoid");
                        break;
               case (D_BOOLEAN_TYPE):
                        d_boolean = (DevBoolean) 0;
                        in_data = (void *) &d_boolean;
                        strcpy(type_to_convert, "DevBoolean");
                        break;
               case (D_USHORT_TYPE):
			d_ushort = 0;
                        in_data = (void *) &d_ushort;
                        strcpy(type_to_convert, "DevUShort");
                        break;
               case (D_SHORT_TYPE):
			d_short = 0;
                        in_data = (void *) &d_short;
                        strcpy(type_to_convert, "DevShort");
                        break;
               case (D_ULONG_TYPE):
			d_ulong = 0;
                        in_data = (void *) &d_ulong;
                        strcpy(type_to_convert, "DevULong");
                        break;
               case (D_LONG_TYPE):
			d_long = 0;
                        in_data = (void *) &d_long;
                        strcpy(type_to_convert, "DevLong");
                        break;
               case (D_FLOAT_TYPE):
			d_float = 0.0;
                        in_data = (void *) &d_float;
                        strcpy(type_to_convert, "DevFloat");
                        break;
               case (D_DOUBLE_TYPE):
			d_double = 0.0;
                        in_data = (void *) &d_double;
                        strcpy(type_to_convert, "DevDouble");
                        break;
               case (D_STRING_TYPE):
			d_string = (DevString) malloc(ASC_MAX_STR);
                        in_data = (void *) &d_string;
                        strcpy(type_to_convert, "DevString");
                        break;
               case (D_INT_FLOAT_TYPE):
			d_intfloat.state = 0;
                        d_intfloat.value = 0.0;
                        in_data = (void *) &d_intfloat;
                        strcpy(type_to_convert, "DevIntFloat");
                        break;
               case (D_FLOAT_READPOINT):
			d_flrp.set = 0.0;
                        d_flrp.read = 0.0;
                        in_data = (void *) &d_flrp;
                        strcpy(type_to_convert, "DevFloatReadPoint");
                        break;
               case (D_STATE_FLOAT_READPOINT):
			d_stflrp.state = 0;
			d_stflrp.set = 0.0;
			d_stflrp.read = 0.0;
                        in_data = (void *) &d_stflrp;
                        strcpy(type_to_convert, "DevStateFloatReadPoint");
                        break;
               case (D_LONG_READPOINT):
			d_lnrp.set = 0;
			d_lnrp.read = 0;
                        in_data = (void *) &d_lnrp;
                        strcpy(type_to_convert, "DevLongReadPoint");
                        break;
               case (D_DOUBLE_READPOINT):
			d_dblrp.set = 0.0;
			d_dblrp.read = 0.0;
                        in_data = (void *) &d_dblrp;
                        strcpy(type_to_convert, "DevDoubleReadPoint");
                        break;
               case (D_VAR_CHARARR):
			d_char_arr.length = 0;
                        d_char_arr.sequence = NULL;
                        in_data = (void *) &d_char_arr;
                        strcpy(type_to_convert, "DevVarCharArray");
                        break;
               case (D_VAR_STRINGARR):
			d_string_arr.length = 0;
                        d_string_arr.sequence = NULL;
                        in_data = (void *) &d_string_arr;
                        strcpy(type_to_convert, "DevVarStringArray");
                        break;
               case (D_VAR_USHORTARR):
			d_ushort_arr.length = 0;
                        d_ushort_arr.sequence = NULL;
                        in_data = (void *) &d_ushort_arr;
                        strcpy(type_to_convert, "DevVarUShortArray");
                        break;
               case (D_VAR_SHORTARR):
			d_short_arr.length = 0;
                        d_short_arr.sequence = NULL;
                        in_data = (void *) &d_short_arr;
                        strcpy(type_to_convert, "DevVarShortArray");
                        break;
               case (D_VAR_ULONGARR):
			d_ulong_arr.length = 0;
                        d_ulong_arr.sequence = NULL;
                        in_data = (void *) &d_ulong_arr;
                        strcpy(type_to_convert, "DevVarULongArray");
                        break;
               case (D_VAR_LONGARR):
			d_long_arr.length = 0;
                        d_long_arr.sequence = NULL;
                        in_data = (void *) &d_long_arr;
                        strcpy(type_to_convert, "DevVarLongArray");
                        break;
               case (D_VAR_FLOATARR):
			d_float_arr.length = 0;
                        d_float_arr.sequence = NULL;
                        in_data = (void *) &d_float_arr;
                        strcpy(type_to_convert, "DevVarFloatArray");
                        break;
               case (D_VAR_DOUBLEARR):
			d_dbl_arr.length = 0;
                        d_dbl_arr.sequence = NULL;
                        in_data = (void *) &d_dbl_arr;
                        strcpy(type_to_convert, "DevVarDoubleArray");
                        break;
/*********************************************************************
               case (D_VAR_FRPARR):
			d_flrp_arr.length = 0;
                        d_flrp_arr.sequence = NULL;
                        in_data = (void *) &d_flrp_arr;
                        strcpy(type_to_convert, "DevVarFloatReadPointArray");
                        break;
               case (D_VAR_SFRPARR):
			d_stflrp_arr.length = 0;
                        d_stflrp_arr.sequence = NULL;
                        in_data = (void *) &d_stflrp_arr;
                        strcpy(type_to_convert, "DevVarStateFloatReadPointArray");
                        break;
               case (D_VAR_LRPARR):
			d_lnrp_arr.length = 0;
                        d_lnrp_arr.sequence = NULL;
                        in_data = (void *) &d_lnrp_arr;
                        strcpy(type_to_convert, "DevVarLongReadPointArray");
                        break;
***************************************************************************/

                case (D_MULMOVE_TYPE):
		        for (ind=0; ind < 8; ind++)
			{
			   d_mulmove.action[ind] = 0;
			   d_mulmove.delay[ind] = 0;
			   d_mulmove.position[ind] = 0.0;
			}
                        in_data = (void *) &d_mulmove;
                        strcpy(type_to_convert, "DevMulMove");
                        break;
	        
                case (D_MOTOR_LONG):
			d_motor_l.axisnum = 0;
                        d_motor_l.value = 0;
                        in_data = (void *) &d_motor_l;
                        strcpy(type_to_convert, "DevMotorLong");
                        break;
	        
                case (D_MOTOR_FLOAT):
			d_motor_l.axisnum = 0;
                        d_motor_l.value = 0.0;
                        in_data = (void *) &d_motor_f;
                        strcpy(type_to_convert, "DevMotorFloat");
                        break;

			
		default:
	                strcpy(*str_err, "dev_put_ascii : input data type not implemented !!");
	                return(-1);
			break;
        }

        status = _asc_convert_intype (type_to_convert, in_arg, in_data, conv_err_str);

        if (status != 0)
        {
           strcpy(*str_err, "dev_put_ascii : input argument type conversion failed.");
           strcat(*str_err, conv_err_str);
	   return(-1);
        }


	status = dev_putget(ds, cmd, (void*) in_data, in_type,
                                     NULL, D_VOID_TYPE, &error);
	if (status == -1)
	{
	   status = dev_putget(ds, cmd, (void*) in_data, in_type,
                                        NULL, D_VOID_TYPE, &error);
           if (status == -1)
           {
	      error_str = dev_error_str (error);
	      strcpy(*str_err, error_str);
	      if (error_str != NULL) free(error_str);
	      return(-1);
	   }
	}

        if (status == 0)
        {
	   strcpy(*str_err, "dev_put_ascii succeeded.");

/*
 * Free the memory allocated by the conversion function for the input argument of the dev_putget.
 */
           switch (in_type)
           {
               case (D_STRING_TYPE):   if (d_string != NULL)
                                       {
                                          free(d_string);
                                          d_string = NULL;
                                       }
                                       break;
               case (D_VAR_CHARARR):   if (d_char_arr.sequence != NULL)
                                       {
                                          free(d_char_arr.sequence);
                                          d_char_arr.sequence = NULL;
                                          d_char_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_STRINGARR): if (d_string_arr.sequence != NULL)
                                       {
                                          free(d_string_arr.sequence);
                                          d_string_arr.sequence = NULL;
                                          d_string_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_USHORTARR): if (d_ushort_arr.sequence != NULL)
                                       {
                                          free(d_ushort_arr.sequence);
                                          d_ushort_arr.sequence = NULL;
                                          d_ushort_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_SHORTARR):  if (d_short_arr.sequence != NULL)
                                       {
                                          free(d_short_arr.sequence);
                                          d_short_arr.sequence = NULL;
                                          d_short_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_ULONGARR):  if (d_ulong_arr.sequence != NULL)
                                       {
                                          free(d_ulong_arr.sequence);
                                          d_ulong_arr.sequence = NULL;
                                          d_ulong_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_LONGARR):   if (d_long_arr.sequence != NULL)
                                       {
                                          free(d_long_arr.sequence);
                                          d_long_arr.sequence = NULL;
                                          d_long_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_FLOATARR):  if (d_float_arr.sequence != NULL)
                                       {
                                          free(d_float_arr.sequence);
                                          d_float_arr.sequence = NULL;
                                          d_float_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_DOUBLEARR): if (d_dbl_arr.sequence != NULL)
                                       {
                                          free(d_dbl_arr.sequence);
                                          d_dbl_arr.sequence = NULL;
                                          d_dbl_arr.length = 0;
                                       }
                                       break;
/***************************************************************************
               case (D_VAR_FRPARR):    if (d_flrp_arr.sequence != NULL)
                                       {
                                          free(d_flrp_arr.sequence);
                                          d_flrp_arr.sequence = NULL;
                                          d_flrp_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_SFRPARR):   if (d_stflrp_arr.sequence != NULL)
                                       {
                                          free(d_stflrp_arr.sequence);
                                          d_stflrp_arr.sequence = NULL;
                                          d_stflrp_arr.length = 0;
                                       }
                                       break;
               case (D_VAR_LRPARR):    if (d_lnrp_arr.sequence != NULL)
                                       {
                                          free(d_lnrp_arr.sequence);
                                          d_lnrp_arr.sequence = NULL;
                                          d_lnrp_arr.length = 0;
                                       }
                                       break;
*************************************************************************/

               default: ;
           }; /*** switch ***/

	} 
        return(0);
}
 

/**@ingroup asciiAPI
 *
 * @param dev_name
 * @param cmd_name
 * @param in_arg
 * @param out_arg
 * @param str_err
 *
 * @return DS_OK or DS_NOTOK
 *
 * @see dev_put_ascii
 * @see dev_putget
 */
long dev_putget_ascii(char *dev_name, char *cmd_name, char *in_arg,
                                    char **out_arg, char **str_err)
{
	devserver			ds;
	char				*error_str;
	long				status = 0,
					error;
	void				*in_data, 
					*out_data;
	long				cmd;
	long				in_type;
	long				out_type;
        char				type_to_convert[101];
        char				conv_err_str[301];
        char				str_local[ASC_MAX_STR];
        char				*res_string;
	int				ind;


        DevBoolean			d_in_boolean;
	DevUShort			d_in_ushort;
	DevShort			d_in_short;
	DevULong			d_in_ulong;
	DevLong				d_in_long;
	DevFloat			d_in_float;
	DevDouble			d_in_double;
	DevString			d_in_string;
	DevIntFloat			d_in_intfloat;
	DevFloatReadPoint		d_in_flrp;
	DevStateFloatReadPoint		d_in_stflrp;
	DevLongReadPoint		d_in_lnrp;
	DevDoubleReadPoint		d_in_dblrp;
	DevVarCharArray			d_in_char_arr;
        DevVarStringArray		d_in_string_arr;
        DevVarUShortArray		d_in_ushort_arr;
        DevVarShortArray		d_in_short_arr;
        DevVarULongArray		d_in_ulong_arr;
        DevVarLongArray			d_in_long_arr;
        DevVarFloatArray		d_in_float_arr;
        DevVarDoubleArray		d_in_dbl_arr;
/***************************************************
        DevVarFloatReadPointArray	d_in_flrp_arr;
        DevVarStateFloatReadPointArray	d_in_stflrp_arr;
        DevVarLongReadPointArray	d_in_lnrp_arr;
****************************************************/
        DevMulMove			d_in_mulmove;
        DevMotorLong			d_in_motor_l;
	DevMotorFloat			d_in_motor_f;

	void				*d_out_void;
        DevBoolean			d_out_boolean;
	DevUShort			d_out_ushort;
	DevShort			d_out_short;
	DevULong			d_out_ulong;
	DevLong				d_out_long;
	DevFloat			d_out_float;
	DevDouble			d_out_double;
	DevString			d_out_string;
	DevIntFloat			d_out_intfloat;
	DevFloatReadPoint		d_out_flrp;
	DevStateFloatReadPoint		d_out_stflrp;
	DevLongReadPoint		d_out_lnrp;
	DevDoubleReadPoint		d_out_dblrp;
	DevVarCharArray			d_out_char_arr;
        DevVarStringArray		d_out_string_arr;
        DevVarUShortArray		d_out_ushort_arr;
        DevVarShortArray		d_out_short_arr;
        DevVarULongArray		d_out_ulong_arr;
        DevVarLongArray			d_out_long_arr;
        DevVarFloatArray		d_out_float_arr;
        DevVarDoubleArray		d_out_dbl_arr;
        DevVarFloatReadPointArray	d_out_flrp_arr;
        DevVarStateFloatReadPointArray	d_out_stflrp_arr;
        DevVarLongReadPointArray	d_out_lnrp_arr;

	if (_asc_library_loaded == ASC_FALSE)
        	status = _asc_load_library();

        if (status == 0)
		_asc_library_loaded = ASC_TRUE;
        else
        {
		strcpy(*str_err, "dev_putget_ascii : Cannot load the shared libraries.");
		return(DS_NOTOK);
        }
           
	if (dev_name == NULL)
	{
		strcpy(*str_err, "dev_putget_ascii : Device Name is not specified.");
		return(DS_NOTOK);
	}

        status = _asc_dev_connect(dev_name, cmd_name, &ds, &cmd, &in_type, &out_type, &error_str);
	if (status == -1)
	{
		strcpy(*str_err, error_str);
		if (error_str != NULL) free(error_str);
		return(DS_NOTOK);
	}

        switch (in_type)
        {
		case (D_VOID_TYPE):
                        in_data = (void *) NULL;
                        strcpy(type_to_convert, "DevVoid");
                        break;
                        break;
               case (D_BOOLEAN_TYPE):
                        d_in_boolean = (DevBoolean) 0;
                        in_data = (void *) &d_in_boolean;
                        strcpy(type_to_convert, "DevBoolean");
                        break;
               case (D_USHORT_TYPE):
			d_in_ushort = 0;
                        in_data = (void *) &d_in_ushort;
                        strcpy(type_to_convert, "DevUShort");
                        break;
               case (D_SHORT_TYPE):
			d_in_short = 0;
                        in_data = (void *) &d_in_short;
                        strcpy(type_to_convert, "DevShort");
                        break;
               case (D_ULONG_TYPE):
			d_in_ulong = 0;
                        in_data = (void *) &d_in_ulong;
                        strcpy(type_to_convert, "DevULong");
                        break;
               case (D_LONG_TYPE):
			d_in_long = 0;
                        in_data = (void *) &d_in_long;
                        strcpy(type_to_convert, "DevLong");
                        break;
               case (D_FLOAT_TYPE):
			d_in_float = 0.0;
                        in_data = (void *) &d_in_float;
                        strcpy(type_to_convert, "DevFloat");
                        break;
               case (D_DOUBLE_TYPE):
			d_in_double = 0.0;
                        in_data = (void *) &d_in_double;
                        strcpy(type_to_convert, "DevDouble");
                        break;
               case (D_STRING_TYPE):
			d_in_string = (DevString) malloc(ASC_MAX_STR);
                        in_data = (void *) &d_in_string;
                        strcpy(type_to_convert, "DevString");
                        break;
               case (D_INT_FLOAT_TYPE):
			d_in_intfloat.state = 0;
                        d_in_intfloat.value = 0.0;
                        in_data = (void *) &d_in_intfloat;
                        strcpy(type_to_convert, "DevIntFloat");
                        break;
               case (D_FLOAT_READPOINT):
			d_in_flrp.set = 0.0;
                        d_in_flrp.read = 0.0;
                        in_data = (void *) &d_in_flrp;
                        strcpy(type_to_convert, "DevFloatReadPoint");
                        break;
               case (D_STATE_FLOAT_READPOINT):
			d_in_stflrp.state = 0;
			d_in_stflrp.set = 0.0;
			d_in_stflrp.read = 0.0;
                        in_data = (void *) &d_in_stflrp;
                        strcpy(type_to_convert, "DevStateFloatReadPoint");
                        break;
               case (D_LONG_READPOINT):
			d_in_lnrp.set = 0;
			d_in_lnrp.read = 0;
                        in_data = (void *) &d_in_lnrp;
                        strcpy(type_to_convert, "DevLongReadPoint");
                        break;
               case (D_DOUBLE_READPOINT):
			d_in_dblrp.set = 0.0;
			d_in_dblrp.read = 0.0;
                        in_data = (void *) &d_in_dblrp;
                        strcpy(type_to_convert, "DevDoubleReadPoint");
                        break;
               case (D_VAR_CHARARR):
			d_in_char_arr.length = 0;
                        d_in_char_arr.sequence = NULL;
                        in_data = (void *) &d_in_char_arr;
                        strcpy(type_to_convert, "DevVarCharArray");
                        break;
               case (D_VAR_STRINGARR):
			d_in_string_arr.length = 0;
                        d_in_string_arr.sequence = NULL;
                        in_data = (void *) &d_in_string_arr;
                        strcpy(type_to_convert, "DevVarStringArray");
                        break;
               case (D_VAR_USHORTARR):
			d_in_ushort_arr.length = 0;
                        d_in_ushort_arr.sequence = NULL;
                        in_data = (void *) &d_in_ushort_arr;
                        strcpy(type_to_convert, "DevVarUShortArray");
                        break;
               case (D_VAR_SHORTARR):
			d_in_short_arr.length = 0;
                        d_in_short_arr.sequence = NULL;
                        in_data = (void *) &d_in_short_arr;
                        strcpy(type_to_convert, "DevVarShortArray");
                        break;
               case (D_VAR_ULONGARR):
			d_in_ulong_arr.length = 0;
                        d_in_ulong_arr.sequence = NULL;
                        in_data = (void *) &d_in_ulong_arr;
                        strcpy(type_to_convert, "DevVarULongArray");
                        break;
               case (D_VAR_LONGARR):
			d_in_long_arr.length = 0;
                        d_in_long_arr.sequence = NULL;
                        in_data = (void *) &d_in_long_arr;
                        strcpy(type_to_convert, "DevVarLongArray");
                        break;
               case (D_VAR_FLOATARR):
			d_in_float_arr.length = 0;
                        d_in_float_arr.sequence = NULL;
                        in_data = (void *) &d_in_float_arr;
                        strcpy(type_to_convert, "DevVarFloatArray");
                        break;
               case (D_VAR_DOUBLEARR):
			d_in_dbl_arr.length = 0;
                        d_in_dbl_arr.sequence = NULL;
                        in_data = (void *) &d_in_dbl_arr;
                        strcpy(type_to_convert, "DevVarDoubleArray");
                        break;
/*********************************************************************
               case (D_VAR_FRPARR):
			d_in_flrp_arr.length = 0;
                        d_in_flrp_arr.sequence = NULL;
                        in_data = (void *) &d_in_flrp_arr;
                        strcpy(type_to_convert, "DevVarFloatReadPointArray");
                        break;
               case (D_VAR_SFRPARR):
			d_in_stflrp_arr.length = 0;
                        d_in_stflrp_arr.sequence = NULL;
                        in_data = (void *) &d_in_stflrp_arr;
                        strcpy(type_to_convert, "DevVarStateFloatReadPointArray");
                        break;
               case (D_VAR_LRPARR):
			d_in_lnrp_arr.length = 0;
                        d_in_lnrp_arr.sequence = NULL;
                        in_data = (void *) &d_in_lnrp_arr;
                        strcpy(type_to_convert, "DevVarLongReadPointArray");
                        break;
***************************************************************************/

                case (D_MULMOVE_TYPE):
		        for (ind=0; ind < 8; ind++)
			{
			   d_in_mulmove.action[ind] = 0;
			   d_in_mulmove.delay[ind] = 0;
			   d_in_mulmove.position[ind] = 0.0;
			}
                        in_data = (void *) &d_in_mulmove;
                        strcpy(type_to_convert, "DevMulMove");
                        break;

                case (D_MOTOR_LONG):
			d_in_motor_l.axisnum = 0;
                        d_in_motor_l.value = 0;
                        in_data = (void *) &d_in_motor_l;
                        strcpy(type_to_convert, "DevMotorLong");
                        break;
	        
                case (D_MOTOR_FLOAT):
			d_in_motor_l.axisnum = 0;
                        d_in_motor_l.value = 0.0;
                        in_data = (void *) &d_in_motor_f;
                        strcpy(type_to_convert, "DevMotorFloat");
                        break;


		default:
	                strcpy(*str_err, "dev_putget_ascii : input data type not implemented !!");
	                return(-1);
			break;
        }

        status = _asc_convert_intype (type_to_convert, in_arg, in_data, conv_err_str);

        if (status != 0)
        {
		strcpy(*str_err, "dev_putget_ascii : input argument type conversion failed.");
		strcat(*str_err, conv_err_str);
		return(-1);
        }

        switch (out_type)
        {
               case (D_VOID_TYPE):
                        out_data = (void *) &d_out_void;
                        strcpy(type_to_convert, "DevVoid");
                        break;
               case (D_BOOLEAN_TYPE):
			d_out_boolean = (DevBoolean) 0;
                        out_data = (void *) &d_out_boolean;
                        strcpy(type_to_convert, "DevBoolean");
                        break;
               case (D_USHORT_TYPE):
			d_out_ushort = 0;
                        out_data = (void *) &d_out_ushort;
                        strcpy(type_to_convert, "DevUShort");
                        break;
               case (D_SHORT_TYPE):
			d_out_short = 0;
                        out_data = (void *) &d_out_short;
                        strcpy(type_to_convert, "DevShort");
                        break;
               case (D_ULONG_TYPE):
			d_out_ulong = 0;
                        out_data = (void *) &d_out_ulong;
                        strcpy(type_to_convert, "DevULong");
                        break;
               case (D_LONG_TYPE):
			d_out_long = 0;
                        out_data = (void *) &d_out_long;
                        strcpy(type_to_convert, "DevLong");
                        break;
               case (D_FLOAT_TYPE):
			d_out_float = 0.0;
                        out_data = (void *) &d_out_float;
                        strcpy(type_to_convert, "DevFloat");
                        break;
               case (D_DOUBLE_TYPE):
			d_out_double = 0.0;
                        out_data = (void *) &d_out_double;
                        strcpy(type_to_convert, "DevDouble");
                        break;
               case (D_STRING_TYPE):
			d_out_string = NULL;
                        out_data = (void *) &d_out_string;
                        strcpy(type_to_convert, "DevString");
                        break;
               case (D_INT_FLOAT_TYPE):
			d_out_intfloat.state = 0;
                        d_out_intfloat.value = 0.0;
                        out_data = (void *) &d_out_intfloat;
                        strcpy(type_to_convert, "DevIntFloat");
                        break;
               case (D_FLOAT_READPOINT):
			d_out_flrp.set = 0.0;
                        d_out_flrp.read = 0.0;
                        out_data = (void *) &d_out_flrp;
                        strcpy(type_to_convert, "DevFloatReadPoint");
                        break;
               case (D_STATE_FLOAT_READPOINT):
			d_out_stflrp.state = 0;
			d_out_stflrp.set = 0.0;
			d_out_stflrp.read = 0.0;
                        out_data = (void *) &d_out_stflrp;
                        strcpy(type_to_convert, "DevStateFloatReadPoint");
                        break;
               case (D_LONG_READPOINT):
			d_out_lnrp.set = 0;
			d_out_lnrp.read = 0;
                        out_data = (void *) &d_out_lnrp;
                        strcpy(type_to_convert, "DevLongReadPoint");
                        break;
               case (D_DOUBLE_READPOINT):
			d_out_dblrp.set = 0.0;
			d_out_dblrp.read = 0.0;
                        out_data = (void *) &d_out_dblrp;
                        strcpy(type_to_convert, "DevDoubleReadPoint");
                        break;
               case (D_VAR_CHARARR):
			d_out_char_arr.length = 0;
                        d_out_char_arr.sequence = NULL;
                        out_data = (void *) &d_out_char_arr;
                        strcpy(type_to_convert, "DevVarCharArray");
                        break;
               case (D_VAR_STRINGARR):
			d_out_string_arr.length = 0;
                        d_out_string_arr.sequence = NULL;
                        out_data = (void *) &d_out_string_arr;
                        strcpy(type_to_convert, "DevVarStringArray");
                        break;
               case (D_VAR_USHORTARR):
			d_out_ushort_arr.length = 0;
                        d_out_ushort_arr.sequence = NULL;
                        out_data = (void *) &d_out_ushort_arr;
                        strcpy(type_to_convert, "DevVarUShortArray");
                        break;
               case (D_VAR_SHORTARR):
			d_out_short_arr.length = 0;
                        d_out_short_arr.sequence = NULL;
                        out_data = (void *) &d_out_short_arr;
                        strcpy(type_to_convert, "DevVarShortArray");
                        break;
               case (D_VAR_ULONGARR):
			d_out_ulong_arr.length = 0;
                        d_out_ulong_arr.sequence = NULL;
                        out_data = (void *) &d_out_ulong_arr;
                        strcpy(type_to_convert, "DevVarULongArray");
                        break;
               case (D_VAR_LONGARR):
			d_out_long_arr.length = 0;
                        d_out_long_arr.sequence = NULL;
                        out_data = (void *) &d_out_long_arr;
                        strcpy(type_to_convert, "DevVarLongArray");
                        break;
               case (D_VAR_FLOATARR):
			d_out_float_arr.length = 0;
                        d_out_float_arr.sequence = NULL;
                        out_data = (void *) &d_out_float_arr;
                        strcpy(type_to_convert, "DevVarFloatArray");
                        break;
               case (D_VAR_DOUBLEARR):
			d_out_dbl_arr.length = 0;
                        d_out_dbl_arr.sequence = NULL;
                        out_data = (void *) &d_out_dbl_arr;
                        strcpy(type_to_convert, "DevVarDoubleArray");
                        break;
               case (D_VAR_FRPARR):
			d_out_flrp_arr.length = 0;
                        d_out_flrp_arr.sequence = NULL;
                        out_data = (void *) &d_out_flrp_arr;
                        strcpy(type_to_convert, "DevVarFloatReadPointArray");
                        break;
               case (D_VAR_SFRPARR):
			d_out_stflrp_arr.length = 0;
                        d_out_stflrp_arr.sequence = NULL;
                        out_data = (void *) &d_out_stflrp_arr;
                        strcpy(type_to_convert, "DevVarStateFloatReadPointArray");
                        break;
               case (D_VAR_LRPARR):
			d_out_lnrp_arr.length = 0;
                        d_out_lnrp_arr.sequence = NULL;
                        out_data = (void *) &d_out_lnrp_arr;
                        strcpy(type_to_convert, "DevVarLongReadPointArray");
                        break;
		default:
	                strcpy(*str_err, "dev_putget_ascii : output data type not implemented !!");
	                return(-1);
			break;
        }

	status = dev_putget(ds, cmd, (void*) in_data, in_type, (void*) out_data, out_type, &error);
	if (status == -1)
	{
		status = dev_putget(ds, cmd, (void*) in_data, in_type, (void*) out_data, out_type, &error);
		if (status == -1)
		{
			error_str = dev_error_str (error);
			strcpy(*str_err, error_str);
			if (error_str != NULL) 
				free(error_str);
			return(-1);
		}
	}

        status = _asc_convert_outtype (type_to_convert, out_data, (char *) str_local, conv_err_str );

        if (status == 0)
        {
		strcpy(*str_err, "dev_putget_ascii succeeded.");
		strcpy(*out_arg, str_local);
		switch (out_type)
		{
			case (D_STRING_TYPE):
			case (D_VAR_CHARARR):
			case (D_VAR_STRINGARR):
			case (D_VAR_USHORTARR):
			case (D_VAR_SHORTARR):
			case (D_VAR_ULONGARR):
			case (D_VAR_LONGARR):
			case (D_VAR_FLOATARR):
			case (D_VAR_DOUBLEARR):
			case (D_VAR_FRPARR):
			case (D_VAR_SFRPARR):
			case (D_VAR_LRPARR): 
				status = dev_xdrfree(out_type, out_data, &error);
				if (status != 0)
				{
					strcat(*str_err, "dev_putget_ascii : cannot free memory allocated");
					strcat(*str_err, " by dev_putget. ");
					strcat(*str_err, "dev_putget_ascii : dev_xdrfree call failed.");
				}
				break;
			default: ;
		};
        }
        else
        {
		strcpy(*str_err, "dev_putget_ascii : output argument type conversion failed.");
		strcat(*str_err, conv_err_str);
		return(-1);
        }

/*
 * Free the memory allocated by the conversion function  
 * for the input argument of the dev_putget.             
 */
        switch (in_type)
        {
               case (D_STRING_TYPE):   
			if (d_in_string != NULL)
			{
				free(d_in_string);
				d_in_string = NULL;
			}
			break;
               case (D_VAR_CHARARR):   
			if (d_in_char_arr.sequence != NULL)
			{
				free(d_in_char_arr.sequence);
				d_in_char_arr.sequence = NULL;
				d_in_char_arr.length = 0;
			}
			break;
               case (D_VAR_STRINGARR): 
			if (d_in_string_arr.sequence != NULL)
			{
				free(d_in_string_arr.sequence);
				d_in_string_arr.sequence = NULL;
				d_in_string_arr.length = 0;
			}
			break;
               case (D_VAR_USHORTARR): 
			if (d_in_ushort_arr.sequence != NULL)
			{
				free(d_in_ushort_arr.sequence);
				d_in_ushort_arr.sequence = NULL;
				d_in_ushort_arr.length = 0;
			}
			break;
               case (D_VAR_SHORTARR):  
			if (d_in_short_arr.sequence != NULL)
			{
				free(d_in_short_arr.sequence);
				d_in_short_arr.sequence = NULL;
				d_in_short_arr.length = 0;
			}
			break;
               case (D_VAR_ULONGARR):  
			if (d_in_ulong_arr.sequence != NULL)
			{
				free(d_in_ulong_arr.sequence);
				d_in_ulong_arr.sequence = NULL;
				d_in_ulong_arr.length = 0;
			}
			break;
               case (D_VAR_LONGARR):   
			if (d_in_long_arr.sequence != NULL)
			{
				free(d_in_long_arr.sequence);
				d_in_long_arr.sequence = NULL;
				d_in_long_arr.length = 0;
			}
			break;
               case (D_VAR_FLOATARR):  
			if (d_in_float_arr.sequence != NULL)
			{
				free(d_in_float_arr.sequence);
				d_in_float_arr.sequence = NULL;
				d_in_float_arr.length = 0;
			}
			break;
               case (D_VAR_DOUBLEARR): 
			if (d_in_dbl_arr.sequence != NULL)
			{
				free(d_in_dbl_arr.sequence);
				d_in_dbl_arr.sequence = NULL;
				d_in_dbl_arr.length = 0;
			}
			break;
/***************************************************************************
               case (D_VAR_FRPARR):    
			if (d_in_flrp_arr.sequence != NULL)
			{
				free(d_in_flrp_arr.sequence);
				d_in_flrp_arr.sequence = NULL;
				d_in_flrp_arr.length = 0;
			}
			break;
               case (D_VAR_SFRPARR):   
			if (d_in_stflrp_arr.sequence != NULL)
			{
				free(d_in_stflrp_arr.sequence);
				d_in_stflrp_arr.sequence = NULL;
				d_in_stflrp_arr.length = 0;
			}
			break;
               case (D_VAR_LRPARR):    
			if (d_in_lnrp_arr.sequence != NULL)
			{
				free(d_in_lnrp_arr.sequence);
				d_in_lnrp_arr.sequence = NULL;
				d_in_lnrp_arr.length = 0;
			}
			break;
*************************************************************************/
               default: ;
        }; 
        return(0);
}
 
/**@ingroup asciiAPI
 *
 * @param dev_name
 * @param str_err
 *
 * @return DS_OK or DS_NOTOK
 */
long dev_free_ascii(char *dev_name, char **str_err)
{
	long          	status, 
			error;
	int             ind_dev, 
			dev_found, 
			ind_cmd;
	DevVarCmdArray  cmd_arr;

	if (dev_name == NULL)
	{
		strcpy(*str_err, "dev_free_ascii : Device Name is not specified.");
		return(DS_NOTOK);
	}

	if (asc_dev_array_ptr == NULL)
	{
		strcpy(*str_err, "dev_free_ascii : No device imported yet.");
		return(DS_NOTOK);
	}

	ind_dev = 0;
	dev_found = ASC_FALSE;

	while ( (dev_found == ASC_FALSE) && (ind_dev < (ASC_MAX_DEV * _asc_nb_block_dev_array)) )
	{
		if ( strcasecmp(asc_dev_array_ptr[ind_dev].dev_name, dev_name) == 0 )
			dev_found = ASC_TRUE;
		else
			ind_dev++;
	}

	if (dev_found == ASC_FALSE)
	{
		strcpy(*str_err, "dev_free_ascii : Device is not specified imported.");
		return(DS_NOTOK);
	}

	if (asc_dev_array_ptr[ind_dev].varcmdarr.sequence != NULL)
	{
		cmd_arr = asc_dev_array_ptr[ind_dev].varcmdarr;
		for (ind_cmd=0; ind_cmd<cmd_arr.length; ind_cmd++)
		{
			if ( cmd_arr.sequence[ind_cmd].in_name != NULL )
				free ( cmd_arr.sequence[ind_cmd].in_name );
			if ( cmd_arr.sequence[ind_cmd].out_name != NULL )
				free ( cmd_arr.sequence[ind_cmd].out_name );
	
			cmd_arr.sequence[ind_cmd].in_name = NULL;
			cmd_arr.sequence[ind_cmd].out_name = NULL;
		}
		free ( cmd_arr.sequence );
	}

	asc_dev_array_ptr[ind_dev].varcmdarr.length = 0;
	asc_dev_array_ptr[ind_dev].varcmdarr.sequence = NULL;

	strcpy(asc_dev_array_ptr[ind_dev].dev_name, ASC_EMPTY_DEV);

	status = 0;
	status = dev_free (asc_dev_array_ptr[ind_dev].ds, &error);
	if (status != 0)
	{
		strcpy(*str_err, "dev_free_ascii : dev_free on the device failed.");
		return(DS_NOTOK);
	}
	return(DS_OK);
}



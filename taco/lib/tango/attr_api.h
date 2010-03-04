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
 * File       :	attr_api.h
 *
 * Project    :	Interface to TACO signals and TANGO attributes
 *
 * Description:	
 *	
 * Author(s)  :	Jens Meyer
 * 		$Author: jensmeyer $
 *
 * Original   :	September 2002
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2010-03-04 12:18:20 $
 *
 *********************************************************************/ 

#ifndef	ATTR_API_H
#define	ATTR_API_H

#include <tango.h>
#include "config.h"
#include <API.h>
#include <ApiP.h>
#include <MDSSignal.h>


/*
 * Missing declarations in API library  (DevSec.h> !!!!
 */
 
extern "C" void free_var_str_array (DevVarStringArray *str_array);

// defines to translate TANGO enum types

#define READ_ATTR				"read"
#define WRITE_ATTR	  			"write"
#define READ_WRITE_ATTR	 			"read_write"
#define READ_WITH_WRITE_ATTR			"read_with_write"

#define SCALAR_FORMAT				"scalar"
#define SPECTRUM_FORMAT 			"spectrum"
#define IMAGE_FORMAT				"image"

#define QUALITY_VALID				0
#define QUALITY_INVALID				(-1)
#define QUALITY_ALARM				1
#define QUALITY_WARNING				2
#define QUALITY_CHANGING			3

#define SOURCE_DEVICE				0
#define SOURCE_CACHE  				1
#define SOURCE_CACHE_DEVICE			2									 

/*
 * Attribute access class
 */
 
class AttrAccess 
{
	public:
   	AttrAccess (char *full_attr_name, long access, DevLong *error);
   	~AttrAccess ();

		long  read_attr  (DevArgument argout, DevType argout_type, DevLong *error);
		long  read_attr_state (DevArgument argout, DevType argout_type, DevLong *error);
		long  read_set_attr  (DevArgument argout, DevType argout_type, DevLong *error);
		long  write_attr (DevArgument argin,  DevType argin_type,  DevLong *error);
		long  attr_cmd_query (DevVarCmdArray *attr_cmd_query_array,DevLong *error);
		long  read_state (DevArgument argout, DevType argout_type, DevLong *error);
		long  read_status (DevArgument argout, DevType argout_type, DevLong *error);
		long	abort	(DevLong *error);	
		long  read_attr_config (DevArgument argout, DevType argout_type, 
									   DevLong *error);
		long  read_attr_mode  (DevArgument argout, DevType argout_type, DevLong *error);
		long  write_attr_mode (DevArgument argin,  DevType argin_type,  DevLong *error);
		long  timeout(long request, struct timeval *dev_timeout, DevLong *error);
		
	protected:
		long	search_attr_name (DevLong *error);
		long	search_tango_attr_name (DevLong *error);
		long	create_attr_access (DevLong *error);
		long	get_tango_data_type (long taco_data_type);
		long	get_taco_data_type (long tango_data_type);
		long	get_taco_array_data_type (long tango_data_type);
		long    check_requested_data_type (long request_type, long attr_type, 
												   DevLong *error);
		long    convert_data (long data_type, void *data_ptr,
								  long conv_data_type, void *conv_data_ptr, DevLong *error);
		long	to_taco_sequence(Tango::DeviceAttribute , DevArgument , 
								    DevType , DevType,long , DevLong *);	
		long	to_xdr_sequence(DevArgument , DevArgument , DevType , DevLong *);
		
#ifdef DevState
#undef DevState
#endif		
		short   tango_to_taco_state (Tango::DevState);
		Tango::DevState taco_to_tango_state (short);	
#ifndef DevState
#define DevState	4
#endif		
			
		char	attr_name[80];
		char	device_name[80];
		char 	signal_name[80];
		
		Tango::DeviceProxy	*tango_obj;
		devserver             taco_obj;
		
		Tango::AttributeInfoEx		attr_config;
		Tango::AttrQuality			quality_last_read;
		short	tango_device;
		short   is_boolean;
		short   is_uchar;
		short   is_state;
		short   is_64;
		
		long	taco_signal_index;
		long	taco_access;
		long	taco_write_cmd;
		long	taco_write_type;



};

#endif

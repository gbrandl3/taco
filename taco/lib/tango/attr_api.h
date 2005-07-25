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
 * 		$Author: jkrueger1 $
 *
 * Original   :	September 2002
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2005-07-25 13:00:43 $
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
								 									 

/*
 * Attribute access class
 */
 
class AttrAccess 
{
	public:
   	AttrAccess (char *full_attr_name, long access, long *error);
   	~AttrAccess ();

		long  read_attr  (DevArgument argout, DevType argout_type, long *error);
		long  write_attr (DevArgument argin,  DevType argin_type,  long *error);
		long  attr_cmd_query (DevVarCmdArray *attr_cmd_query_array,long *error);
		long  read_state (DevArgument argout, DevType argout_type, long *error);
		long  read_status (DevArgument argout, DevType argout_type, long *error);
		long	abort	(long *error);	
		long  read_attr_config (DevVarStringArray *attr_config_array,long *error);			
	protected:
		long	search_attr_name (long *error);
		long	search_tango_attr_name (long *error);
		long	create_attr_access (long *error);
		long	get_tango_data_type (long taco_data_type);
		long	get_taco_data_type (long tango_data_type);
		long  check_requested_data_type (long request_type, long attr_type, 
												   long *error);
		long  convert_data (long data_type, void *data_ptr,
								  long conv_data_type, void *conv_data_ptr, long *error);															
	
		char	attr_name[80];
		char	device_name[80];
		char 	signal_name[80];
		
		Tango::DeviceProxy	*tango_obj;
		devserver             taco_obj;
		
		Tango::AttributeInfo	attr_config;
		short	tango_device;
		
		long	taco_signal_index;
		long	taco_access;
		long	taco_write_cmd;
		long	taco_write_type;

		long	to_taco_sequence(Tango::DeviceAttribute , DevArgument , DevType , DevType, long *);	
		long	to_xdr_sequence(DevArgument , DevArgument , DevType , long *);	

};

#endif

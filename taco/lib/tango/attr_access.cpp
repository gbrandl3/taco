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
 * File       :	attribute_access.cpp
 *
 * Project    :	Interface to TACO signals and TANGO attributes
 *
 * Description:	
 *	
 * Author(s)  :	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original   :	September2002
 *
 * Version    :	$Revision: 1.13 $
 *
 * Date       : $Date: 2009-09-25 11:55:08 $
 *
 *********************************************************************/ 

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <attr_api.h>

// copy from tango_api.cpp, because declared as static function
static long tango_dev_error_string(Tango::DevFailed tango_exception);

#ifdef DevState
#undef DevState
#endif
#ifdef RUNNING
#undef RUNNING
#endif
#ifdef WRITE
#undef WRITE
#endif
/**
 * Constructor for the attribute access class.
 * 
 * Takes a four field attribute name as input.
 * Verifies whether it is TACO or a TANGO attribute
 * and opens a connection to the underlying device.
 *					 
 * @param full_attr_name name of Taco or TANGO attribute
 * @param access Taco security access
 * @param error pointer to take the Taco error code
 */

AttrAccess::AttrAccess (char *full_attr_name, long access, DevLong *error)
{
	char	*str_ptr;
	short	count;

	*error 				= 0;
	tango_device 		= False;
	taco_signal_index = (-1);
	quality_last_read = Tango::ATTR_VALID;
	
	sprintf (attr_name, "%s", full_attr_name);
	
	/*
	 * test for a valid four field attribute name
	 */
	
	count = 0; 
	str_ptr = attr_name;
	
	while ( (str_ptr = strchr (str_ptr, '/')) != NULL )
		{
		count ++;
		str_ptr++;
		}
		
	if ( count < 3 )
		{
		*error = DevErr_ValueOutOfBounds;
		return;
		}
	
	/*
	 * Check for the tango: device name extension
	 */
	 
	if (strncasecmp(attr_name, "tango:", 6) == 0)
		{
		sprintf (device_name, "%s", (attr_name+6));
		tango_device = True;
		}
	else
		{
		sprintf (device_name, "%s", attr_name);
		}
		 
	/*
	 * Split in device and signal name
	 */
	 	
	str_ptr = strrchr (device_name, '/');
	*str_ptr = '\0';
	
	str_ptr = strrchr (attr_name, '/');
	str_ptr++;
	sprintf (signal_name, "%s", str_ptr);
	
	/*
	 * Store requested access TACO right
	 */
	 
	taco_access = access;

	/*
	 * Search in the TACO and TANGO database to find
	 * the requested attribute
	 */
	 
	if ( tango_device == True )
		{
		if ( search_tango_attr_name (error) == DS_NOTOK )
			{
			printf ("Attribute not found! Error = %d\n", *error);
			return;
			}		
		}
	else
		{
		if ( search_attr_name (error) == DS_NOTOK )
			{
			printf ("Attribute not found! Error = %d\n", *error);
			return;
			}		
		}
		
	
	/*
	 * Connect to the device of the requested attribute
	 */
	 
	if ( create_attr_access (error) == DS_NOTOK )
		{
		printf ("Cannot create access! Error = %d\n", *error);
		return;
		}
		
	//printf ("Attribute access is now initialised!\n");
			
	return;
}



/**
 * Destructor for the attribute access class.
 *
 * Closes the connection to an attribute and cleans-up the memory.
 */
AttrAccess::~AttrAccess()
{
	DevLong	error;
	
	/* 
	 * close access to the underlying device
	 */
	 
	 if ( tango_device == False )
	{
		if ( taco_dev_free (taco_obj, &error) == DS_NOTOK )
		{
			printf ("Cannot free access to %s :\n%s\n", attr_name, dev_error_str (error));
			return;
		}
			
		// printf ("Freed TACO attribute : %s\n", attr_name);
	}
	else
	{
		try
		{
   			delete tango_obj;
			
			// printf ("Deleted TANGO attribute : %s\n", attr_name);
		}
		
		catch (Tango::DevFailed &e)
		{	
			tango_dev_error_string (e);
//			printf ("Cannot free access to %s :\n%s\n", attr_name, (char *)e);
			printf ("Cannot free access to %s :\n", attr_name);
		}
	}
}



/**
 * Maps the read and write functionality of attributes to Taco commands.
 *
 * A read only attribute can execute only DevRead and a read/write attribute 
 * the commands DevRead and DevWrite.
 * The attribute configuration is returned by the command DevReadConfig.
 * The command DevReadSetValue and DevIOState can be used only for Tango
 * attributes. DevReadSetValue returns the read and the set point of an attribute
 * as an array. DevIOState returns the quality of the last read attribute.
 *					 
 * @param varcmdarr Pointer to command array.
 * @param error pointer to take the Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::attr_cmd_query (DevVarCmdArray *attr_cmd_query_array, DevLong *error)
{
	DevCmdInfo  *cmd_info;
	unsigned int 	cmd_counter;
	
	*error = 0;
	
	/*
	 * Get the number of commands to return
	 */
	cmd_counter = 5;
	if ( attr_config.writable != Tango::READ )
		{
		cmd_counter++;
		}
	if ( tango_device == True )
		{
		cmd_counter = cmd_counter +4;
		}
	
	/*
 	 * Allocate memory for a sequence of DevCmdInfo structures
 	 * returned with attr_cmd_query_array.
 	 */
	
	cmd_info = (DevCmdInfo *) malloc
	    		  (cmd_counter * sizeof (DevCmdInfo));
	if ( cmd_info == NULL )
	{
		*error  = DevErr_InsufficientMemory;
		return (DS_NOTOK);
	}	 
	
	
	/*
	 * Command DevRead to read attribute values
	 */
	 
	cmd_counter = 0; 
	cmd_info[cmd_counter].cmd = 28;
	sprintf (cmd_info[cmd_counter].cmd_name, "DevRead");
	cmd_info[cmd_counter].in_name = NULL;
	cmd_info[cmd_counter].in_type = D_VOID_TYPE;
	cmd_info[cmd_counter].out_name = NULL;
	cmd_info[cmd_counter].out_type = get_taco_data_type (attr_config.data_type);
	
	/*
	 * Command DevAbort to stop a moving device via the attribute handle
	 */
	 
	cmd_counter++;
	cmd_info[cmd_counter].cmd = 33;
	sprintf (cmd_info[cmd_counter].cmd_name, "DevAbort");
	cmd_info[cmd_counter].in_name = NULL;
	cmd_info[cmd_counter].in_type = D_VOID_TYPE;
	cmd_info[cmd_counter].out_name = NULL;
	cmd_info[cmd_counter].out_type = D_VOID_TYPE;	
	
	/*
	 * Command DevState to read the device state via a attribute handle
	 */
	 
	cmd_counter++;
	cmd_info[cmd_counter].cmd = 4;
	sprintf (cmd_info[cmd_counter].cmd_name, "DevState");
	cmd_info[cmd_counter].in_name = NULL;
	cmd_info[cmd_counter].in_type = D_VOID_TYPE;
	cmd_info[cmd_counter].out_name = NULL;
	cmd_info[cmd_counter].out_type = D_SHORT_TYPE;	

	/*
	 * Command DevStatus to read the device status via a attribute handle
	 */
	 
	cmd_counter++;
	cmd_info[cmd_counter].cmd = 8;
	sprintf (cmd_info[cmd_counter].cmd_name, "DevStatus");
	cmd_info[cmd_counter].in_name = NULL;
	cmd_info[cmd_counter].in_type = D_VOID_TYPE;
	cmd_info[cmd_counter].out_name = NULL;
	cmd_info[cmd_counter].out_type = D_STRING_TYPE;	
	
	/*
	  * Command DevReadConfig to read the attribute properties
	  */
	  
	cmd_counter++;
	cmd_info[cmd_counter].cmd = 605;
	sprintf (cmd_info[cmd_counter].cmd_name, "DevReadConfig");
	cmd_info[cmd_counter].in_name = NULL;
	cmd_info[cmd_counter].in_type = D_VOID_TYPE;
	cmd_info[cmd_counter].out_name = NULL;
	cmd_info[cmd_counter].out_type = D_VAR_STRINGARR;


	/* Is this an Tango attribute? For Taco signals no setpoint information available */
	if ( tango_device == True )
	 	{	
		/*
	  		* Command DevReadSetValue to read the device read and set point as an array
	  		*/
		cmd_counter++;
		cmd_info[cmd_counter].cmd = 39;
		sprintf (cmd_info[cmd_counter].cmd_name, "DevReadSetValue");
		cmd_info[cmd_counter].in_name  = NULL;
		cmd_info[cmd_counter].in_type  = D_VOID_TYPE;
		cmd_info[cmd_counter].out_name = NULL;
		cmd_info[cmd_counter].out_type = get_taco_array_data_type (attr_config.data_type);

		/*
			*
			*/
		cmd_counter++;
		cmd_info[cmd_counter].cmd = 150;
		sprintf (cmd_info[cmd_counter].cmd_name, "DevIOState");
		cmd_info[cmd_counter].in_name  = NULL;
		cmd_info[cmd_counter].in_type  = D_VOID_TYPE;
		cmd_info[cmd_counter].out_name = NULL;
		cmd_info[cmd_counter].out_type = D_SHORT_TYPE;

		/*
	  		* Command DevReadMode to read the tango data source
	  		*/
		cmd_counter++;
		cmd_info[cmd_counter].cmd = 250;
		sprintf (cmd_info[cmd_counter].cmd_name, "DevReadMode");
		cmd_info[cmd_counter].in_name  = NULL;
		cmd_info[cmd_counter].in_type  = D_VOID_TYPE;
		cmd_info[cmd_counter].out_name = NULL;
		cmd_info[cmd_counter].out_type = D_SHORT_TYPE;

		/*
	  		* Command DevRetMode to set the tango data source
	  		*/
		cmd_counter++;
		cmd_info[cmd_counter].cmd = 249;
		sprintf (cmd_info[cmd_counter].cmd_name, "DevSetMode");
		cmd_info[cmd_counter].in_name  = NULL;
		cmd_info[cmd_counter].in_type  = D_SHORT_TYPE;
		cmd_info[cmd_counter].out_name = NULL;
		cmd_info[cmd_counter].out_type = D_VOID_TYPE;
		}
		
	/*
	 * Command DevWrite to write attribute values
	 */
	 
	/* Is the attribute writable? */
	
	if ( attr_config.writable != Tango::READ )
		{
		cmd_counter++;
		cmd_info[cmd_counter].cmd = 29;
		sprintf (cmd_info[cmd_counter].cmd_name, "DevWrite");
		cmd_info[cmd_counter].in_name = NULL;
		if ( tango_device == False )
			{
			cmd_info[cmd_counter].in_type = get_taco_data_type (taco_write_type);
			}
		else
			{
			cmd_info[cmd_counter].in_type = get_taco_data_type (attr_config.data_type);
			}

		cmd_info[cmd_counter].out_name = NULL;
		cmd_info[cmd_counter].out_type = D_VOID_TYPE;		
		}	
	 
	attr_cmd_query_array->length   = ++cmd_counter; 	
	attr_cmd_query_array->sequence = cmd_info;	
	return (DS_OK);
}



/**
 *  Returns the Tango attribute properties or Taco signal configuration
 *	 values as a string array.
 *	 All values which are in the Taco signal definition are returned in the
 *  order. Some extra fields are added for Tango spectrum and image attributes.
 *
 * @param argout - Taco string array with attribute properties
 * @param argout_type - D_VAR_STRINGARR
 * @param error pointer to take the Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::read_attr_config (DevArgument argout, 
											 DevType argout_type, DevLong *error)
{
	DevVarStringArray 	property_array;
	char						*property_ptrs[20];
	char 						data_type[80];
	char						max_dim_x[80];
	char						max_dim_y[80];
	
	*error = 0;
	
	property_ptrs[0] = (char *)"18";
	property_ptrs[1] = (char *)attr_config.name.c_str();
	property_ptrs[2] = (char *)attr_config.label.c_str();
	property_ptrs[3] = (char *)attr_config.unit.c_str();
	property_ptrs[4] = (char *)attr_config.format.c_str();
	property_ptrs[5] = (char *)attr_config.description.c_str();
	property_ptrs[6] = (char *)attr_config.max_value.c_str();
	property_ptrs[7] = (char *)attr_config.min_value.c_str();
	property_ptrs[8] = (char *)attr_config.alarms.max_alarm.c_str();
	property_ptrs[9] = (char *)attr_config.alarms.min_alarm.c_str();
	property_ptrs[10] =(char *)attr_config.alarms.delta_val.c_str();;
	property_ptrs[11] =(char *)attr_config.alarms.delta_t.c_str();;
	property_ptrs[12] =(char *)attr_config.standard_unit.c_str();
	
	// added tango display unit parameter 16.9.2009
	property_ptrs[18] =(char *)attr_config.display_unit.c_str();
	
	// translate enumeration type
	switch (attr_config.writable)
		{
		case Tango::READ:
			property_ptrs[13] = (char *)READ_ATTR;
			break;
		case Tango::WRITE:
			property_ptrs[13] = (char *)WRITE_ATTR;
			break;
		case Tango::READ_WRITE:
			property_ptrs[13] = (char *)READ_WRITE_ATTR;
			break;
		case Tango::READ_WITH_WRITE:
			property_ptrs[13] = (char *)READ_WITH_WRITE_ATTR;
			break;		
		}
		
	sprintf (data_type, "%d", attr_config.data_type);
	property_ptrs[14] = data_type;

	// translate enumeration type
	switch (attr_config.data_format)
		{
		case Tango::SCALAR:
			property_ptrs[15] = (char *)SCALAR_FORMAT;
			break;
		case Tango::SPECTRUM:
			property_ptrs[15] = (char *)SPECTRUM_FORMAT;
			break;
		case Tango::IMAGE:
			property_ptrs[15] = (char *)IMAGE_FORMAT;
			break;
		}	
	
	sprintf (max_dim_x, "%d", attr_config.max_dim_x);
	property_ptrs[16] = max_dim_x;
	sprintf (max_dim_y, "%d", attr_config.max_dim_y);
	property_ptrs[17] = max_dim_y;
	
	property_array.length = 19;
	property_array.sequence = property_ptrs;
	
	//	Allocate and fill XDR argout
	
	if ( to_xdr_sequence(&property_array, argout, D_VAR_STRINGARR, error) == DS_NOTOK )
		{
		return DS_NOTOK;
		}
	
	return (DS_OK);
}

/**
 * Writes the input argument to the attribute.
 *
 * The input value is casted if necessary and written via the TACO command or the
 * TANGO write_attribute() methode.
 *					 
 * @param argin Input argument
 * @param argin_type Type of input argument
 * @param error pointer to take the Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::write_attr (DevArgument argin, DevType argin_type, DevLong *error)
{
	long							input_type;
	Tango::DeviceAttribute	attr_values;
	double						double_value;
	float							float_value;
	long							long_value;
	short							short_value;
	unsigned short					ushort_value;
	string						string_value;
	char							*c_str_ptr;
	void							*data_ptr;
	
	*error = 0;
	
	/*
	 * Check for a writabe attribute
	 */
	 
	if ( attr_config.writable == Tango::READ )
		{
		*error = DevErr_CommandNotImplemented;
		return (DS_NOTOK);
		}
	 
	/*
	 * Switch Taco to Tango data type definitions
	 */
	 
	input_type = get_tango_data_type (argin_type);
	
	/*
	 * Test whether the requested data type can be casted to the attribute
	 * data type.
	 */
	 
	if ( check_requested_data_type (attr_config.data_type, input_type, error)
	     == DS_NOTOK )
		{
		return (DS_NOTOK);
		}
	
	switch (attr_config.data_type)
		{
		case Tango::DEV_DOUBLE:
			data_ptr = (void *)&double_value;
			break;
			
		case Tango::DEV_FLOAT:
			data_ptr = (void *)&float_value;
			break;
			
		case Tango::DEV_LONG:
			data_ptr = (void *)&long_value;
			break;
			
		case Tango::DEV_SHORT:
			data_ptr = (void *)&short_value;
			break;
			
		case Tango::DEV_USHORT:
			data_ptr = (void *)&ushort_value;
			break;
			
		case Tango::DEV_BOOLEAN:
			data_ptr = (void *)&short_value;
			break;
			
		case Tango::DEV_UCHAR:
			data_ptr = (void *)&short_value;
			break;
			
		case Tango::DEV_STRING:
			data_ptr = (void *)&(c_str_ptr);
			break;
		}
							
	
	if ( tango_device == False )
	 	{
		/*
		 * Convert input data if necessary to the attribute data type
		 */
			 
		if ( convert_data (input_type, argin, 
							    taco_write_type, data_ptr, error) == DS_NOTOK )
			{
			return (DS_NOTOK);
			}
		
		/*
		 * Call the TACO command to set the attribute value
		 */	
		 
		if ( taco_dev_put (taco_obj, taco_write_cmd, data_ptr, taco_write_type, error)
					     == DS_NOTOK )
			{
			return (DS_NOTOK);
			}	
		}
	else
		{
		try
			{
			 			
			/*
			 * Fill the TANGO attribute data object and write 
			 * the attribute value.
			 */
			 
			attr_values.set_name(signal_name);

			switch (attr_config.data_type)
				{
				case Tango::DEV_DOUBLE:
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					attr_values << double_value;
					tango_obj->write_attribute (attr_values);
					break;
					
				case Tango::DEV_FLOAT:
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					attr_values << *((float *)data_ptr);
					tango_obj->write_attribute (attr_values);
					break;
					
				case Tango::DEV_LONG:
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					attr_values << *((long *)data_ptr);
					tango_obj->write_attribute (attr_values);
					break;
					
				case Tango::DEV_SHORT:
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					attr_values << *((short *)data_ptr);
					tango_obj->write_attribute (attr_values);
					break;
					
				case Tango::DEV_USHORT:
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					attr_values << *((unsigned short *)data_ptr);
					tango_obj->write_attribute (attr_values);
					break;
					
				case Tango::DEV_BOOLEAN:
				{
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					bool b_value = (bool)(*((short *)data_ptr));
					attr_values << b_value;
					tango_obj->write_attribute (attr_values);
				}
				break;
					
				case Tango::DEV_UCHAR:
				{
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					unsigned char uc_value = (unsigned char)(*((short *)data_ptr));
					attr_values << uc_value;
					tango_obj->write_attribute (attr_values);
				}
				break;
					
				case Tango::DEV_STRING:
				{
					if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
					{
						return (DS_NOTOK);
					}
					string_value = *((char **)data_ptr);
					attr_values << string_value;
					tango_obj->write_attribute (attr_values);
				}
				break;
					
				case Tango::DEVVAR_SHORTARRAY:
				{
					if( input_type!=attr_config.data_type )
					{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
					}
				    DevVarShortArray *s_arr = (DevVarShortArray *)argin;
					if(is_boolean) {
					  vector<bool> b_vec;
					  for(int i=0;i<s_arr->length;i++)
						b_vec.push_back((bool)(s_arr->sequence[i]));
					  Tango::DeviceAttribute	attr_b(signal_name,b_vec);
					  tango_obj->write_attribute (attr_b);					
					} else if(is_uchar) {
					  vector<unsigned char> uc_vec;
					  for(int i=0;i<s_arr->length;i++)
						uc_vec.push_back((unsigned char)(s_arr->sequence[i]));
					  Tango::DeviceAttribute	attr_uc(signal_name,uc_vec);
					  tango_obj->write_attribute (attr_uc);										
					} else {
					  vector<short> s_vec;
					  for(int i=0;i<s_arr->length;i++)
						s_vec.push_back(s_arr->sequence[i]);
					  Tango::DeviceAttribute	attr_s(signal_name,s_vec);
					  tango_obj->write_attribute (attr_s);
					}
				}
			    break;
				
				case Tango::DEVVAR_USHORTARRAY:
				{
					if( input_type!=attr_config.data_type )
					{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
					}
					DevVarUShortArray *us_arr = (DevVarUShortArray *)argin;
					vector<unsigned short> us_vec;
					for(int i=0;i<us_arr->length;i++)
						us_vec.push_back(us_arr->sequence[i]);
					Tango::DeviceAttribute	attr_us(signal_name,us_vec);
					tango_obj->write_attribute (attr_us);					
				}
			    break;
				
				case Tango::DEVVAR_LONGARRAY:
				{
					if( input_type!=attr_config.data_type )
					{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
					}
					DevVarLongArray *l_arr = (DevVarLongArray *)argin;
					vector<long> l_vec;
					for(int i=0;i<l_arr->length;i++)
						l_vec.push_back(l_arr->sequence[i]);
					Tango::DeviceAttribute	attr_l(signal_name,l_vec);
					tango_obj->write_attribute (attr_l);					
				}
			    break;
				
				case Tango::DEVVAR_FLOATARRAY:
				{
					if( input_type!=attr_config.data_type )
					{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
					}
					DevVarFloatArray *f_arr = (DevVarFloatArray *)argin;
					vector<float> f_vec;
					for(int i=0;i<f_arr->length;i++)
						f_vec.push_back(f_arr->sequence[i]);
					Tango::DeviceAttribute	attr_f(signal_name,f_vec);
					tango_obj->write_attribute (attr_f);					
				}
			    break;
				
				case Tango::DEVVAR_DOUBLEARRAY:
				{
					if( input_type!=attr_config.data_type )
					{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
					}
					DevVarDoubleArray *d_arr = (DevVarDoubleArray *)argin;
					vector<double> d_vec;
					for(int i=0;i<d_arr->length;i++)
						d_vec.push_back(d_arr->sequence[i]);
					Tango::DeviceAttribute	attr_d(signal_name,d_vec);
					tango_obj->write_attribute (attr_d);					
				}
			    break;
				
				case Tango::DEVVAR_STRINGARRAY:
				{
					if( input_type!=attr_config.data_type )
					{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
					}
					DevVarStringArray *str_arr = (DevVarStringArray *)argin;
					vector<string> str_vec;
					for(int i=0;i<str_arr->length;i++)
						str_vec.push_back(str_arr->sequence[i]);
					Tango::DeviceAttribute	attr_str(signal_name,str_vec);
					tango_obj->write_attribute (attr_str);					
				}
			    break;
				
				}
			}
		
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         		tango_dev_error_string (e);
         		*error = DevErr_TangoAccessFailed;
			
         		return(DS_NOTOK);				
			}		
		}
	
	return (DS_OK);
}


/**
 * Reads the read and set point of an attribute as an array.
 * This can be used only for Tango attributes. Taco attributes don`t
 * have an associated set point.
 *					 
 * @param argout read and set point array
 * @param argout_type Requested type for output argument
 * @param error Taco error code
 * 
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::read_set_attr (DevArgument argout, DevType argout_type, DevLong *error)
{
	Tango::DeviceAttribute	attr_values;
	long					request_type;
	long					outgoing_type;

	*error = 0;
	
	/*
	 * Switch Taco to Tango data type definitions
	 */
	 
	request_type = get_tango_data_type (argout_type);
	 
	
	/*
	 * Read the attribute value from the device
	 */
	
	if ( tango_device == False )
	 	{
		// No set point handling implemented for Taco signals
		
		*error = DevErr_CommandNotImplemented;
		return (DS_NOTOK);
		}		
		
	else
		{
		/* 
		 * Read from a TANGO device
		 */
		try
			{
			attr_values = tango_obj->read_attribute (signal_name);
			
			// Check the validity of data.
			// If not valid return an error
			
			quality_last_read = attr_values.get_quality();
			if ( quality_last_read == Tango::ATTR_INVALID )
				{
				*error = DevErr_TangoAttributeValueIsNotValid;
				return (DS_NOTOK);
				}
			
			//	Switch on TANGO argout
			switch (attr_config.data_type)
				{
				case Tango::DEV_DOUBLE:
					outgoing_type = Tango::DEVVAR_DOUBLEARRAY;
					break;
					
				case Tango::DEV_FLOAT:
					outgoing_type = Tango::DEVVAR_FLOATARRAY;
					break;

				case Tango::DEV_LONG:
					outgoing_type = Tango::DEVVAR_LONGARRAY;
					break;

				case Tango::DEV_SHORT:
					outgoing_type = Tango::DEVVAR_SHORTARRAY;
					break;
					
				case Tango::DEV_USHORT:
					outgoing_type = Tango::DEVVAR_USHORTARRAY;
					break;
					
				case Tango::DEV_BOOLEAN:
					outgoing_type = Tango::DEVVAR_SHORTARRAY;
					break;
					
				case Tango::DEV_UCHAR:
					outgoing_type = Tango::DEVVAR_SHORTARRAY;
					break;

				case Tango::DEV_STRING:
					outgoing_type = Tango::DEVVAR_STRINGARRAY;
					break;

				case Tango::DEVVAR_SHORTARRAY:
				case Tango::DEVVAR_USHORTARRAY:
				case Tango::DEVVAR_LONGARRAY:
				case Tango::DEVVAR_FLOATARRAY:
				case Tango::DEVVAR_DOUBLEARRAY:
				case Tango::DEVVAR_STRINGARRAY:
					outgoing_type = attr_config.data_type;
					break;										
				}
			
			// check the correct data type
			
			if ( request_type != outgoing_type )
				{
				*error = DevErr_CannotConvertAttributeDataType;
				return (DS_NOTOK);
				}	
				
			// extract the sequence of read and setpoint
			return to_taco_sequence (attr_values, argout, 
										   outgoing_type, argout_type, true, error);
			}
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);				
			}			 
		}


	return (DS_OK);
}



/**
 * Reads the value of an attribute.
 *
 * The output data is casted to the requested data type if possible.
 *
 * The TACO command DevReadSigValues or the TANGO methode read_attribute() are used to 
 * read the value.
 *					 
 * @param argout Output argument
 * @param argout_type Requested type for output argument
 * @param error Taco error code
 * 
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::read_attr (DevArgument argout, DevType argout_type, DevLong *error)
{
	Tango::DeviceAttribute	attr_values;
	double					double_value;
	float					float_value;
	long					long_value;
	short					short_value;
	unsigned short			ushort_value;
	string					string_value;
	char					*c_str_ptr;
	long					request_type;
	void					*data_ptr;

	*error = 0;
	
	/*
	 * Switch Taco to Tango data type definitions
	 */
	 
	request_type = get_tango_data_type (argout_type);
	 
	/*
	 * Test whether the attribute data type can be casted to the requested
	 * data type.
	 */	
	 
	if ( check_requested_data_type (request_type, attr_config.data_type, error)
	     == DS_NOTOK )
		{
		return (DS_NOTOK);
		}	 
	
	/*
	 * Read the attribute value from the device
	 */
	
	if ( tango_device == False )
	 	{
		/*
		 * Read from a TACO device
		 */
		 
		switch (attr_config.data_type)
			{
			case Tango::DEV_DOUBLE:
				DevVarDoubleArray 	double_arr;
				double_arr.length 	= 0;
				double_arr.sequence 	= NULL;
				
				if ( taco_dev_putget (taco_obj, DevReadSigValues, NULL, D_VOID_TYPE,
					  				 &double_arr, D_VAR_DOUBLEARR, error) == DS_NOTOK )
					{
					return (DS_NOTOK);
					}
				
				double_value = double_arr.sequence[taco_signal_index];				
				dev_xdrfree (D_VAR_DOUBLEARR, &double_arr, error);	
				break;
				
			case Tango::DEV_FLOAT:
				DevVarFloatArray 		float_arr;
				float_arr.length 		= 0;
				float_arr.sequence 	= NULL;
				
				if ( taco_dev_putget (taco_obj, DevReadSigValues, NULL, D_VOID_TYPE,
					  				 &float_arr, D_VAR_FLOATARR, error) == DS_NOTOK )
					{
					return (DS_NOTOK);
					}
				
				float_value = float_arr.sequence[taco_signal_index];				
				dev_xdrfree (D_VAR_FLOATARR, &float_arr, error);	
				break;
				
			case Tango::DEV_LONG:
				DevVarLongArray 		long_arr;
				long_arr.length 		= 0;
				long_arr.sequence 	= NULL;
				
				if ( taco_dev_putget (taco_obj, DevReadSigValues, NULL, D_VOID_TYPE,
					  				 &long_arr, D_VAR_LONGARR, error) == DS_NOTOK )
					{
					return (DS_NOTOK);
					}
				
				long_value = long_arr.sequence[taco_signal_index];				
				dev_xdrfree (D_VAR_LONGARR, &long_arr, error);	
				break;								
			}		
		}
	else
		{
		/* 
		 * Read from a TANGO device
		 */
		try
			{
			attr_values = tango_obj->read_attribute (signal_name);
			
			// Check the validity of data.
			// If not valid return an error
			
			quality_last_read = attr_values.get_quality();
			if ( quality_last_read == Tango::ATTR_INVALID )
				{
				*error = DevErr_TangoAttributeValueIsNotValid;
				return (DS_NOTOK);
				}
			
			//	Switch on TANGO argout
			switch (attr_config.data_type)
				{
				case Tango::DEV_DOUBLE:
					attr_values >> double_value;
					break;
					
				case Tango::DEV_FLOAT:
					attr_values >> float_value;
					break;

				case Tango::DEV_LONG:
					attr_values >> long_value;
					break;

				case Tango::DEV_SHORT:
					attr_values >> short_value;
					break;
					
				case Tango::DEV_USHORT:
					attr_values >> ushort_value;
					break;
					
				case Tango::DEV_BOOLEAN:
					{
				    bool bool_value;
					attr_values >> bool_value;
					short_value = (short)bool_value;
					}
					break;
					
				case Tango::DEV_UCHAR:
					{
				    unsigned char uc_value;
					attr_values >> uc_value;
					short_value = (short)uc_value;
					}
					break;

				case Tango::DEV_STRING:
					attr_values >> string_value;
					break;

				case Tango::DEVVAR_SHORTARRAY:
				case Tango::DEVVAR_USHORTARRAY:
				case Tango::DEVVAR_LONGARRAY:
				case Tango::DEVVAR_FLOATARRAY:
				case Tango::DEVVAR_DOUBLEARRAY:
				case Tango::DEVVAR_STRINGARRAY:
					if ( attr_config.data_type != request_type )
						{
						*error = DevErr_CannotConvertAttributeDataType;
						return (DS_NOTOK);
						}
					else
						{
						return to_taco_sequence(attr_values, argout,
								attr_config.data_type, argout_type, false, error);
						}
					break;										
				}
			}
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);				
			}			 
		}


	/*
	 * Cast the read data to void
	 */
		
	switch (attr_config.data_type)
		{
		case Tango::DEV_DOUBLE:
			data_ptr = (void *)&double_value;
			break;
			
		case Tango::DEV_FLOAT:
			data_ptr = (void *)&float_value;
			break;
			
		case Tango::DEV_LONG:
			data_ptr = (void *)&long_value;
			break;
			
		case Tango::DEV_SHORT:
		case Tango::DEV_BOOLEAN:
		case Tango::DEV_UCHAR:
			data_ptr = (void *)&short_value;
			break;
			
		case Tango::DEV_USHORT:
			data_ptr = (void *)&ushort_value;
			break;
			
		case Tango::DEV_STRING:
			c_str_ptr = (char *)string_value.c_str();
			data_ptr = (void *)&(c_str_ptr);
			break;
		}
		
	/*
	 * Convert the attribute value to the requested data type if necessary
	 */
	 	
	 if (attr_config.data_type==Tango::DEV_DOUBLE	||
	 	attr_config.data_type==Tango::DEV_FLOAT		||
	 	attr_config.data_type==Tango::DEV_LONG		||
	 	attr_config.data_type==Tango::DEV_SHORT		||
	 	attr_config.data_type==Tango::DEV_USHORT	||
	 	attr_config.data_type==Tango::DEV_BOOLEAN	||
	 	attr_config.data_type==Tango::DEV_UCHAR		||
	 	attr_config.data_type==Tango::DEV_STRING)
		{
			if ( convert_data (attr_config.data_type, data_ptr, 
							 request_type, argout, error) == DS_NOTOK )
				return (DS_NOTOK);
		}

	return (DS_OK);
}


/**
 * Reads the quality factor of the last read value of an attribute.
 * Only Tango attributes have a quality value.
 * The value is returnd as a short value:
 *	ATTR_VALID    = 0
 * ATTR_INVALID  = -1
 *	ATTR_ALARM    = 1
 * ATTR_WARNING  = 2
 * ATTR_CHANGING = 3
 *      				 
 * @param argout - quality of attribute value
 * @param argout_type D_SHORT_TYPE
 * @param error Taco error code
 * 
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::read_attr_state (DevArgument argout, DevType argout_type, DevLong *error)
{
	short	*quality = (short *)argout;
	
	/*
	 * Read the attribute quality value for the last attribute read
	 */
	
	if ( tango_device == False )
	 	{
		// No quality handling implemented for Taco signals
		
		*error = DevErr_CommandNotImplemented;
		return (DS_NOTOK);
		}
			
	switch (quality_last_read)
		{
		case Tango::ATTR_VALID:
			*quality = QUALITY_VALID;
			break;
		case Tango::ATTR_INVALID:
			*quality = QUALITY_INVALID;
			break;
		case Tango::ATTR_ALARM:
			*quality = QUALITY_ALARM;
			break;
		case Tango::ATTR_WARNING:
			*quality = QUALITY_WARNING;
			break;
		case Tango::ATTR_CHANGING:
			*quality = QUALITY_CHANGING;
			break;
		}
	
	return (DS_OK);
}


/**
 * Convert a Tango sequence to the request TACO sequence
 *
 * @param attribute
 * @param argout Tango data returned.
 * @param tango_type
 * @param taco_type
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::to_taco_sequence(Tango::DeviceAttribute attribute, 
                                 DevArgument argout,
				 DevType tango_type,
				 DevType taco_type,
				 long get_setpoint,
				 DevLong *error)
{
	vector<unsigned char>	uc_vect;
	vector<short>	s_vect;
	vector<bool>	b_vect;
	vector<unsigned short>	us_vect;
	vector<long>	l_vect;
	vector<float>	f_vect;
	vector<double>	d_vect;
	vector<string>	str_vect;
	
	int		nb_data;
	short	status;

	//	Tango attribute to vector
	switch(tango_type)
	{
	case Tango::DEVVAR_SHORTARRAY:
		if(is_boolean) {
			attribute >> b_vect;
			if(get_setpoint)
			  nb_data = b_vect.size();
			else
			  nb_data = attribute.get_nb_read();
		} else if(is_uchar) {
			attribute >> uc_vect;
			if(get_setpoint)
			  nb_data = uc_vect.size();
			else
			  nb_data = attribute.get_nb_read();		
		} else {
			attribute >> s_vect;
			if(get_setpoint)
			  nb_data = s_vect.size();
			else
			  nb_data = attribute.get_nb_read();		
		}
		break;										
		
	case Tango::DEVVAR_USHORTARRAY:
		attribute >> us_vect;
		if(get_setpoint)
		  nb_data = us_vect.size();
		else
		  nb_data = attribute.get_nb_read();
		break;										

	case Tango::DEVVAR_LONGARRAY:
		attribute >> l_vect;
		if(get_setpoint)
		  nb_data = l_vect.size();
		else
		  nb_data = attribute.get_nb_read();
		break;										

	case Tango::DEVVAR_DOUBLEARRAY:
		attribute >> d_vect;
		if(get_setpoint)
		  nb_data = d_vect.size();
		else
		  nb_data = attribute.get_nb_read();
		break;										
		
	case Tango::DEVVAR_FLOATARRAY:
		attribute >> f_vect;
		if(get_setpoint)
		  nb_data = f_vect.size();
		else
		  nb_data = attribute.get_nb_read();
		break;
		
	case Tango::DEVVAR_STRINGARRAY:
		attribute >> str_vect;
		if(get_setpoint)
		  nb_data = str_vect.size();
		else
		  nb_data = attribute.get_nb_read();
		break;										
	}
	//	fill Taco argout
	switch(taco_type)
	{
	case D_VAR_SHORTARR:
		DevVarShortArray	sa;
		sa.length = nb_data;
		sa.sequence = new short[nb_data];
		for (int i=0 ; i<nb_data ; i++)
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
			    if(is_boolean)
				  sa.sequence[i] = (short)b_vect[i];
				else if(is_uchar)
				  sa.sequence[i] = (short)uc_vect[i];
				else
				  sa.sequence[i] = (short)s_vect[i];
				break;
			case Tango::DEVVAR_USHORTARRAY:
				sa.sequence[i] = (short)us_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				sa.sequence[i] = (short)l_vect[i];
				break;
			case Tango::DEVVAR_FLOATARRAY:
				sa.sequence[i] = (short)f_vect[i];
				break;
			case Tango::DEVVAR_DOUBLEARRAY:
				sa.sequence[i] = (short)d_vect[i];
				break;
			}
		//	Allocate and fill XDR argout
		status = to_xdr_sequence(&sa, argout, taco_type, error);
		delete sa.sequence;
		return status;
		break;
		
	case D_VAR_USHORTARR:
		DevVarUShortArray	usa;
		usa.length = nb_data;
		usa.sequence = new unsigned short[nb_data];
		for (int i=0 ; i<nb_data ; i++)
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
			    if(is_boolean)
				  usa.sequence[i] = (unsigned short)b_vect[i];
				else if(is_uchar)
				  usa.sequence[i] = (unsigned short)uc_vect[i];				
				else
				  usa.sequence[i] = (unsigned short)s_vect[i];
				break;
			case Tango::DEVVAR_USHORTARRAY:
				usa.sequence[i] = (unsigned short)us_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				usa.sequence[i] = (unsigned short)l_vect[i];
				break;
			case Tango::DEVVAR_FLOATARRAY:
				usa.sequence[i] = (unsigned short)f_vect[i];
				break;
			case Tango::DEVVAR_DOUBLEARRAY:
				usa.sequence[i] = (unsigned short)d_vect[i];
				break;
			}
		//	Allocate and fill XDR argout
		status = to_xdr_sequence(&usa, argout, taco_type, error);
		delete usa.sequence;
		return status;
		break;

	case D_VAR_LONGARR:
		DevVarLongArray	la;
		la.length = nb_data;
		la.sequence = new DevLong[nb_data];
		for (int i=0 ; i<nb_data ; i++)
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
			    if(is_boolean)
				  la.sequence[i] = (long)b_vect[i];
				else if(is_uchar)
				  la.sequence[i] = (long)uc_vect[i];
				else
				  la.sequence[i] = (long)s_vect[i];
				break;
			case Tango::DEVVAR_USHORTARRAY:
				la.sequence[i] = (long)us_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				la.sequence[i] = (long)l_vect[i];
				break;
			case Tango::DEVVAR_FLOATARRAY:
				la.sequence[i] = (long)f_vect[i];
				break;
			case Tango::DEVVAR_DOUBLEARRAY:
				la.sequence[i] = (long)d_vect[i];
				break;
			}
		//	Allocate and fill XDR argout
		status = to_xdr_sequence(&la, argout, taco_type, error);
		delete la.sequence;
		return status;
		break;

	case D_VAR_FLOATARR:
		DevVarFloatArray fa;
		fa.length = nb_data;
		fa.sequence = new float[nb_data];
		for (int i=0 ; i<nb_data ; i++)
		{
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
			    if(is_boolean)
				  fa.sequence[i] = (float)b_vect[i];
				else if(is_uchar)
				  fa.sequence[i] = (float)uc_vect[i];
				else
				  fa.sequence[i] = (float)s_vect[i];				
				break;
			case Tango::DEVVAR_USHORTARRAY:
				fa.sequence[i] = (float)us_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				fa.sequence[i] = (float)l_vect[i];
				break;
			case Tango::DEVVAR_FLOATARRAY:
				fa.sequence[i] = (float)f_vect[i];
				break;
			case Tango::DEVVAR_DOUBLEARRAY:
				fa.sequence[i] = (float)d_vect[i];
				break;
			}
		}
		//	Allocate and fill XDR argout
		status = to_xdr_sequence(&fa, argout, taco_type, error);
		delete fa.sequence;
		return status;
		break;										
		
	case D_VAR_DOUBLEARR:
		DevVarDoubleArray	da;
		da.length = nb_data;
		da.sequence = new double[nb_data];
		for (int i=0 ; i<nb_data ; i++)
		{
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
			    if(is_boolean)
				  da.sequence[i] = (double)b_vect[i];
				else if(is_uchar)
				  da.sequence[i] = (double)uc_vect[i];
				else
				  da.sequence[i] = (double)s_vect[i];				
				break;
			case Tango::DEVVAR_USHORTARRAY:
				da.sequence[i] = (double)us_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				da.sequence[i] = (double)l_vect[i];
				break;
			case Tango::DEVVAR_FLOATARRAY:
				da.sequence[i] = (double)f_vect[i];
				break;
			case Tango::DEVVAR_DOUBLEARRAY:
				da.sequence[i] = (double)d_vect[i];
				break;
			}
		}
		//	Allocate and fill XDR argout
		status = to_xdr_sequence(&da, argout, taco_type, error);
		delete da.sequence;
		return status;
		break;										
		
	case D_VAR_STRINGARR:
	    if(tango_type != Tango::DEVVAR_STRINGARRAY) {
			*error = DevErr_CannotConvertAttributeDataType;
			return (DS_NOTOK);
		}
		DevVarStringArray stra;
		stra.length = nb_data;
		stra.sequence = (char **)malloc(nb_data * sizeof(char *));
		for (int i=0 ; i<nb_data ; i++)
		{
		  int lgth = str_vect[i].length();
		  stra.sequence[i] = (char *)malloc(lgth+1);
		  strcpy(stra.sequence[i],str_vect[i].c_str());		  
		}
		//	Allocate and fill XDR argout
		status = to_xdr_sequence(&stra, argout, taco_type, error);
		for(int i=0;i<nb_data;i++) free(stra.sequence[i]);
		free(stra.sequence);
		return status;
		break;										
	}
}

/**
 * Encode the outgoing arguments into XDR format.
 *
 * Decode the arguments afterwards, to get the same memory
 * allocation by XDR as the dev_putget() working with RPCs.
 *
 * @param argin	data read.
 * @param argout data returned
 * @param data_type argin and argout type.  
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::to_xdr_sequence(DevArgument argin, 
                                 DevArgument argout,
				 DevType data_type,
				 DevLong *error)
{
	DevDataListEntry type_info;
	XDR 	xdrs_en;
	XDR 	xdrs_de;
	char* 	buf;
	long 	buf_size;
	/*
	* Get the XDR data type.
	*/
	*error = 0;
	if (xdr_get_type (data_type, &type_info, error) == DS_NOTOK)
	{
		return (DS_NOTOK);
	}

	/*
	* Calculate the size of the buffer needed for the
	* outgoing arguments.
	*/
	if (type_info.xdr_length != NULL)
	{
		buf_size = type_info.xdr_length(argin);
		if ( buf_size == (_Int)DS_NOTOK)
		{
			*error = DevErr_XDRLengthCalculationFailed;
			return (DS_NOTOK);
		}
		buf_size += 32;
	}
	else
	{
		/* If no length calculation is implemented, allocate
		 * 8KB as for RPC/UDP.
		 */
		buf_size = 8000;
	}

	/*
	 * Allocate the data buffer.
	 */
	if ( (buf = (char *)malloc(buf_size)) == NULL)
	{
		*error  = DevErr_InsufficientMemory;
		return (DS_NOTOK);
	}
	/*
	* Create XDR handles and serialize and deserialize
	* the arguments. This is done to have the same
	* memory allocation as dev_putget() using RPCs.
	*/
#ifdef _UCC
	xdrmem_create(&xdrs_en, (caddr_t)buf, (_Int)buf_size, XDR_ENCODE);
	xdrmem_create(&xdrs_de, (caddr_t)buf, (_Int)buf_size, XDR_DECODE);
#else
	xdrmem_create(&xdrs_en, buf, (_Int)buf_size, XDR_ENCODE);
	xdrmem_create(&xdrs_de, buf, (_Int)buf_size, XDR_DECODE);
#endif /* _UCC */


	if (!xdr_pointer(&xdrs_en, (char **)&(argin),
					(_Int)type_info.size,
					(xdrproc_t)type_info.xdr))
	{
		*error = DevErr_CannotDecodeArguments;
		return (DS_NOTOK);
	}

	if (!xdr_pointer(&xdrs_de, (char **)&(argout),
					(_Int)type_info.size,
		 			(xdrproc_t)type_info.xdr))
	{
		*error = DevErr_CannotDecodeArguments;
		return (DS_NOTOK);
	}

	free (buf);

	return (DS_OK);
}


/**
 * Opens a connection to the device of the attribute and initialises the attribute 
 * configuration structure.
 *
 * The TANGO attribute_config structure is used to store the values. All necessary 
 * information for TACO is read and also written into the TANGO attribute config structure. 
 *					 
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::create_attr_access (DevLong *error)
{
	DevVarCmdArray     cmd_array;
	DevVarStringArray	 signal_config;
	char					 write_cmd_name[80];
	char					 *str_ptr;
	short					 i;
	
	
	*error = 0;
	is_boolean = false;
	is_uchar = false;
	
	/* 
	 * create the connection to the device
	 */
	 
	 if ( tango_device == False )
	 	{
		if ( taco_dev_import (device_name, taco_access, &taco_obj, error) == DS_NOTOK )
			{
			return (DS_NOTOK);
			}
			
		// printf ("Imported TACO device : %s\n", device_name);
		}
	else
		{
		try
			{
   				tango_obj = new Tango::DeviceProxy (device_name);
			
			/*
		 	 * DO NOT Set the data source to direct device access
			 * 
			 * use default of CACHE then DEVICE - ag+jm 26jan2006
		 	 */
		 
				//tango_obj->set_source(Tango::DEV);
			
			
			}
		
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);				
			}
		}

	/*
	 * Query the command list of the Taco device
	 */		
	 
	if ( tango_device == False )
		{
		cmd_array.length = 0;
   	cmd_array.sequence = NULL;

		if ( taco_dev_cmd_query (taco_obj, &cmd_array, error ) == DS_NOTOK )
	 		{
			return (DS_NOTOK);
			}
	
		/*
	 	 * Find data data type of the attribute
	 	 */
			
		for (i=0; i< cmd_array.length; i++)
			{
			if ( strcasecmp (cmd_array.sequence[i].cmd_name,"DevReadSigValues") == 0 )
				{				
				switch (cmd_array.sequence[i].out_type)
					{
					case D_VAR_LONGARR:
						attr_config.data_type = Tango::DEV_LONG;
						break;					
					case D_VAR_FLOATARR:
						attr_config.data_type = Tango::DEV_FLOAT;
						break;
					case D_VAR_DOUBLEARR:
						attr_config.data_type = Tango::DEV_DOUBLE;
						break;
					}
							
				attr_config.data_format = Tango::SCALAR;
				attr_config.max_dim_x	= 0;
				attr_config.max_dim_y 	= 0;
				attr_config.name			= signal_name;					
				}
			}
					
		/*
		 * Find the command to write the TACO attribute
		 */
		 
		if ( strncasecmp (signal_name, "set-", 4) == 0 )
			{
			str_ptr = signal_name;
			str_ptr = str_ptr + 4;
			sprintf (write_cmd_name, "DevSet%s", str_ptr);
			}
		else
			{
			sprintf (write_cmd_name, "DevSet%s", signal_name);
			}
			
		attr_config.writable = Tango::READ;
		attr_config.writable_attr_name = write_cmd_name;
					
		for (i=0; i< cmd_array.length; i++)
			{
			if ( strcasecmp (cmd_array.sequence[i].cmd_name, write_cmd_name) == 0 )
				{
				attr_config.writable = Tango::READ_WRITE;
				
				/*
				 * Get the command number
				 */
				 
				taco_write_cmd = cmd_array.sequence[i].cmd;
				
				/*
				 * Get the input data type
				 */
				 
				taco_write_type = get_tango_data_type (cmd_array.sequence[i].in_type);
				}
			}
			
		/*
		 * Read the attribute configuration values
		 */
		 
		if ( dev_get_sig_config_from_name (device_name, signal_name, 
			  &signal_config, error) == DS_NOTOK )
			{
			return (DS_NOTOK);
			}
		
		/* 
		 * Fill the TANGO attribute_config structure
		 */
		 	
		attr_config.label = signal_config.sequence[1];
		attr_config.unit  = signal_config.sequence[2];
		attr_config.format = signal_config.sequence[3];
		attr_config.description = signal_config.sequence[4];
		attr_config.max_value = signal_config.sequence[5];
		attr_config.min_value = signal_config.sequence[6];
		attr_config.standard_unit = signal_config.sequence[11];
		attr_config.display_unit  = "1"; /* Taco does not support the display unit */

		attr_config.alarms.max_alarm = signal_config.sequence[7];
		attr_config.alarms.min_alarm = signal_config.sequence[8];		
		attr_config.alarms.delta_val = signal_config.sequence[9];
		attr_config.alarms.delta_t   = signal_config.sequence[10];
		}
		
	else
	 	{
		/*
		 * Read attribute configuration for a TANGO device
		 */
		
		try
			{ 			
			string str(signal_name);
			attr_config = tango_obj->get_attribute_config (str);		
			}
		catch (Tango::DevFailed &e)
			{
			//
	   	// recover TANGO error string and save it in the
	   	// global error string so that
			// it can be printed out with dev_error_str()
			//
			
      	tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);									
			}
			//	Check if scalar or sequence( for spectrum or image)
			switch (attr_config.data_type)
			{
			case Tango::DEV_BOOLEAN:
			    is_boolean = true;
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_SHORTARRAY;
					break;
			case Tango::DEV_UCHAR:
			    is_uchar = true;
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_SHORTARRAY;
					break;
			case Tango::DEV_SHORT:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_SHORTARRAY;
					break;
			case Tango::DEV_USHORT:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_USHORTARRAY;
					break;
			case Tango::DEV_LONG:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_LONGARRAY;
					break;
			case Tango::DEV_FLOAT:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_FLOATARRAY;
					break;
			case Tango::DEV_DOUBLE:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_DOUBLEARRAY;
					break;
			case Tango::DEV_STRING:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_STRINGARRAY;
					break;
			}
		
		}
	//cout << "Attribute : " << attr_name << endl;
	//cout << "Name      : " << attr_config.name << endl;
	//cout << "Data Type : " << attr_config.data_format	<< " " \
	//							  << attr_config.data_type << endl;
	//cout << "Writable  : " << attr_config.writable_attr_name	<< " " \
	//		     				  << attr_config.writable << endl;	
	//cout << "Label     : " << attr_config.label	<< endl;
	//cout << "Unit      : " << attr_config.unit << endl;
	//cout << "Format    : " << attr_config.format << endl;
	//cout << "Descr     : " << attr_config.description	<< endl;
	//cout << "Max Value : " << attr_config.max_value << endl;
	//cout << "Min Value : " << attr_config.min_value << endl;
	//cout << "High Alarm: " << attr_config.max_alarm	<< endl;
	//cout << "Low Alarm : " << attr_config.min_alarm	<< endl;
	//cout << "Std. Unit : " << attr_config.standard_unit	<< endl;
	//cout << "Disp Unit : " << attr_config.display_unit	<< endl;				
}

/**
 * Converts a data of a given type to another data type.
 *
 * @param data_type type of data to convert
 * @param data_ptr data to convert
 * @param conv_data_type requested data type
 * @param conv_data_ptr	converted data
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::convert_data (long data_type, void *data_ptr,
				 long conv_data_type, void *conv_data_ptr, 
				 DevLong *error)
{
	static char		c_string_value[80];
	char				**str_ptr;
	char				*c_str_ptr;
	double 			*double_ptr;
	float				*float_ptr;
	long				*long_ptr;
	short				*short_ptr;	
	unsigned short		*ushort_ptr;	
	
	*error = 0;
	cout.precision(10);
	
	switch (data_type)
		{
		case Tango::DEV_STRING:
			str_ptr = (char **)data_ptr;
			// cout << "Read value = " << *str_ptr << endl;
			
			*((char **)conv_data_ptr) = *str_ptr;
			break;
		
		case Tango::DEV_DOUBLE:
			double_ptr = (double *)data_ptr;
			// cout << "Value = " << *double_ptr << endl;
			
			switch (conv_data_type)
				{
				case Tango::DEV_DOUBLE:
					*((double *)conv_data_ptr) = *double_ptr;
					break;
					
				case Tango::DEV_STRING:
					sprintf (c_string_value, "%f", *double_ptr);
					c_str_ptr = &c_string_value[0];
					*((char **)conv_data_ptr) = c_str_ptr;
					break;					
				}
			break;
			
			
		case Tango::DEV_FLOAT:
			float_ptr = (float *)data_ptr;
			// cout << "Value = " << *float_ptr << endl;
			
			switch (conv_data_type)
				{
				case Tango::DEV_FLOAT:
					*((float *)conv_data_ptr) = *float_ptr;
					break;
					
				case Tango::DEV_DOUBLE:
					*((double *)conv_data_ptr) = *float_ptr;
					break;
					
				case Tango::DEV_STRING:
					sprintf (c_string_value, "%f", *float_ptr);
					c_str_ptr = &c_string_value[0];
					*((char **)conv_data_ptr) = c_str_ptr;
					break;											
				}
			break;

		case Tango::DEV_LONG:
			long_ptr = (long *)data_ptr;
			// cout << "Value = " << *long_ptr << endl;
			
			switch (conv_data_type)
				{					
				case Tango::DEV_LONG:
					*((long *)conv_data_ptr) = *long_ptr;
					break;					
					
				case Tango::DEV_FLOAT:
					*((float *)conv_data_ptr) = *long_ptr;
					break;					
					
				case Tango::DEV_DOUBLE:
					*((double *)conv_data_ptr) = *long_ptr;
					break;
					
				case Tango::DEV_STRING:
					sprintf (c_string_value, "%ld", *long_ptr);
					c_str_ptr = &c_string_value[0];
					*((char **)conv_data_ptr) = c_str_ptr;
					break;														
				}
			break;
			
		case Tango::DEV_SHORT:
		case Tango::DEV_BOOLEAN:
		case Tango::DEV_UCHAR:
			short_ptr = (short *)data_ptr;
			// cout << "Read value = " << *short_ptr << endl;
			
			switch (conv_data_type)
				{
				case Tango::DEV_SHORT:
				case Tango::DEV_BOOLEAN:
				case Tango::DEV_UCHAR:
					*((short *)conv_data_ptr) = *short_ptr;
					break;
					
				case Tango::DEV_LONG:
					*((long *)conv_data_ptr) = *short_ptr;
					break;					
					
				case Tango::DEV_FLOAT:
					*((float *)conv_data_ptr) = *short_ptr;
					break;					
					
				case Tango::DEV_DOUBLE:
					*((double *)conv_data_ptr) = *short_ptr;
					break;
					
				case Tango::DEV_STRING:
					sprintf (c_string_value, "%d", *short_ptr);
					c_str_ptr = &c_string_value[0];
					*((char **)conv_data_ptr) = c_str_ptr;
					break;														
				}
			break;			

		case Tango::DEV_USHORT:
			ushort_ptr = (unsigned short *)data_ptr;
			// cout << "Read value = " << *ushort_ptr << endl;
			
			switch (conv_data_type)
				{
				case Tango::DEV_USHORT:
					*((unsigned short *)conv_data_ptr) = *ushort_ptr;
					break;
					
				case Tango::DEV_LONG:
					*((long *)conv_data_ptr) = *ushort_ptr;
					break;					
					
				case Tango::DEV_FLOAT:
					*((float *)conv_data_ptr) = *ushort_ptr;
					break;					
					
				case Tango::DEV_DOUBLE:
					*((double *)conv_data_ptr) = *ushort_ptr;
					break;
					
				case Tango::DEV_STRING:
					sprintf (c_string_value, "%u", *ushort_ptr);
					c_str_ptr = &c_string_value[0];
					*((char **)conv_data_ptr) = c_str_ptr;
					break;														
				}
			break;			
			
		default:
			*error = DevErr_CannotConvertAttributeDataType;
			return (DS_NOTOK);
			break;												
		}
	
	return (DS_OK);
}	


/**
 * Checks whether the attribute data type can be casted into the requested data type.
 *					 
 * @param request_type requested data type
 * @param attr_type attribute data type
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::check_requested_data_type (long request_type, long attr_type, DevLong *error)
{
	*error = 0;
	if ( request_type != attr_type )
		{
		switch (request_type)
			{
			case Tango::DEV_SHORT:
				if( attr_type != Tango::DEV_BOOLEAN &&
				    attr_type != Tango::DEV_UCHAR ) {
				  *error = DevErr_CannotConvertAttributeDataType;
				  return (DS_NOTOK);
				}
				break;
				
			case Tango::DEV_USHORT:
				if( attr_type != Tango::DEV_USHORT ) {
				  *error = DevErr_CannotConvertAttributeDataType;
				  return (DS_NOTOK);
				}
				break;
				
			case Tango::DEV_BOOLEAN:
				if( attr_type != Tango::DEV_SHORT) {
				  *error = DevErr_CannotConvertAttributeDataType;
				  return (DS_NOTOK);
				}
				break;
				
			case Tango::DEV_UCHAR:
				if( attr_type != Tango::DEV_SHORT ) {
				  *error = DevErr_CannotConvertAttributeDataType;
				  return (DS_NOTOK);
				}
				break;
				
			case Tango::DEV_LONG:
				if ( attr_type != Tango::DEV_SHORT )
					{
					*error = DevErr_CannotConvertAttributeDataType;
					return (DS_NOTOK);
					}
				break;
				
			case Tango::DEV_FLOAT:
				if ( attr_type == Tango::DEV_STRING ||
					  attr_type == Tango::DEV_DOUBLE )
					{
					*error = DevErr_CannotConvertAttributeDataType;
					return (DS_NOTOK);
					}
				break;				
				
			case Tango::DEV_DOUBLE:
				if ( attr_type == Tango::DEV_STRING )
					{
					*error = DevErr_CannotConvertAttributeDataType;
					return (DS_NOTOK);
					}
				break;
				
			case Tango::DEVVAR_SHORTARRAY:
			case Tango::DEVVAR_LONGARRAY:
			case Tango::DEVVAR_DOUBLEARRAY:
				if ( attr_type == Tango::DEV_STRING )
					{
					*error = DevErr_CannotConvertAttributeDataType;
					return (DS_NOTOK);
					}
				break;

			case Tango::DEV_STRING:
				break;			

			default:
				*error = DevErr_CannotConvertAttributeDataType;
				return (DS_NOTOK);
				break;	
			}
		}
		
	return (DS_OK);	
}



/**
 * Converts TACO data type to TANGO data type 
 *					 
 * @param taco_data_type TACO data type
 *
 * @return TANGO data type
 */
long	AttrAccess::get_tango_data_type (long taco_data_type)
{
	switch (taco_data_type)
		{
		case D_STRING_TYPE:
			return (Tango::DEV_STRING);
			break;
						
		case D_SHORT_TYPE:
			return (Tango::DEV_SHORT);
			break;

		case D_USHORT_TYPE:
			return (Tango::DEV_USHORT);
			break;
			
		case D_LONG_TYPE:
			return (Tango::DEV_LONG);
			break;

		case D_FLOAT_TYPE:
			return (Tango::DEV_FLOAT);
			break;			
			
		case D_DOUBLE_TYPE:
			return (Tango::DEV_DOUBLE);
			break;

		case D_VAR_STRINGARR:
			return (Tango::DEVVAR_STRINGARRAY);
			break;

		case D_VAR_SHORTARR:
			return (Tango::DEVVAR_SHORTARRAY);
			break;
			
		case D_VAR_USHORTARR:
			return (Tango::DEVVAR_USHORTARRAY);
			break;

		case D_VAR_LONGARR:
			return (Tango::DEVVAR_LONGARRAY);
			break;

		case D_VAR_FLOATARR:
			return (Tango::DEVVAR_FLOATARRAY);
			break;

		case D_VAR_DOUBLEARR:
			return (Tango::DEVVAR_DOUBLEARRAY);
			break;

		default:
			return (Tango::DEV_VOID);
			break;
		}
}

/**
 * Converts TANGO data type to TACO data type 
 *					 
 * @param tango_data_type TANGO data type
 * 
 * @return TACO data type
 */
long	AttrAccess::get_taco_data_type (long tango_data_type)
{
	switch (tango_data_type)
		{
		case Tango::DEV_STRING:
			return (D_STRING_TYPE);
			break;
			
		case Tango::DEV_SHORT:
			return (D_SHORT_TYPE);
			break;
			
		case Tango::DEV_USHORT:
			return (D_USHORT_TYPE);
			break;
			
		case Tango::DEV_BOOLEAN:
			return (D_SHORT_TYPE);
			break;
			
		case Tango::DEV_UCHAR:
			return (D_SHORT_TYPE);
			break;
			
		case Tango::DEV_LONG:
			return (D_LONG_TYPE);
			break;

		case Tango::DEV_FLOAT:
			return (D_FLOAT_TYPE);
			break;			
			
		case Tango::DEV_DOUBLE:
			return (D_DOUBLE_TYPE);
			break;
			
		case Tango::DEVVAR_SHORTARRAY:
			return D_VAR_SHORTARR;
			break;
		case Tango::DEVVAR_USHORTARRAY:
			return D_VAR_USHORTARR;
			break;
		case Tango::DEVVAR_LONGARRAY:
			return D_VAR_LONGARR;
			break;
		case Tango::DEVVAR_FLOATARRAY:
			return D_VAR_FLOATARR;
			break;
		case Tango::DEVVAR_DOUBLEARRAY:
			return D_VAR_DOUBLEARR;
			break;
		case Tango::DEVVAR_STRINGARRAY:
			return D_VAR_STRINGARR;
			break;

		default:
			return (D_VOID_TYPE);
			break;
		}
}


/**
 * When returning read and set point as for Tango 
 *	attributes, the result is always returned as an
 *	array of the attribute data type.
 *	Converts TANGO data type to TACO data type, but
 *	returns alaways the array type. 
 *
 * @tango_data_type - TANGO data type
 *
 *	@return TACO array data type
 */
 
long	AttrAccess::get_taco_array_data_type (long tango_data_type)
{
	switch (tango_data_type)
		{
		case Tango::DEV_STRING:
			return (D_VAR_STRINGARR);
			break;
			
		case Tango::DEV_SHORT:
			return (D_VAR_SHORTARR);
			break;
			
		case Tango::DEV_USHORT:
			return (D_VAR_USHORTARR);
			break;
			
		case Tango::DEV_BOOLEAN:
			return (D_VAR_SHORTARR);
			break;
			
		case Tango::DEV_UCHAR:
			return (D_VAR_SHORTARR);
			break;
			
		case Tango::DEV_LONG:
			return (D_VAR_LONGARR);
			break;

		case Tango::DEV_FLOAT:
			return (D_VAR_FLOATARR);
			break;			
			
		case Tango::DEV_DOUBLE:
			return (D_VAR_DOUBLEARR);
			break;
			
		case Tango::DEVVAR_SHORTARRAY:
			return D_VAR_SHORTARR;
			break;
			
		case Tango::DEVVAR_USHORTARRAY:
			return D_VAR_USHORTARR;
			break;
			
		case Tango::DEVVAR_LONGARRAY:
			return D_VAR_LONGARR;
			break;
			
		case Tango::DEVVAR_FLOATARRAY:
			return D_VAR_FLOATARR;
			break;
			
		case Tango::DEVVAR_DOUBLEARRAY:
			return D_VAR_DOUBLEARR;
			break;
			
		case Tango::DEVVAR_STRINGARRAY:
			return D_VAR_STRINGARR;
			break;

		default:
			return (D_VOID_TYPE);
			break;
		}
}


/**
 * Searches the requested attribute in the TACO and the TANGO system.
 *
 * The TACO database is searched first. If the name is not defined for TACO the 
 * TANGO database is searched. 
 * 
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::search_attr_name (DevLong *error)
{
	short	i;
	DevVarStringArray	attr_list;
	
	Tango::DeviceProxy	*tango_object;
	vector<string>			*tango_attr_list;
	

	*error 				= 0;
	tango_device 		= DS_NOTOK;
	tango_object 			= NULL;
	tango_attr_list 	= NULL;

	/*
	 * search attribute for a device defined in the TACO database
	 */
	
	if ( dev_get_sig_list (device_name, &attr_list, error) == DS_OK )
		{
		for (i=0; i <attr_list.length; i++)
			{
			if ( strcasecmp (attr_list.sequence[i], signal_name) == 0 )
				{
				taco_signal_index = i;
				tango_device = False;
				free_var_str_array (&attr_list);
				
				return (DS_OK);
				}
			}
			
		free_var_str_array (&attr_list);
		}
	 
	/*
	 * search attribute for a device in the TANGO database
	 */
	 
	try
		{
		*error = 0;
   	tango_object = new Tango::DeviceProxy (device_name);
		
		
		tango_attr_list = tango_object->get_attribute_list();
		for (i=0; i < (*tango_attr_list).size(); i++)
			{
			if ( strcasecmp ((*tango_attr_list)[i].c_str(), signal_name) == 0 )
				{
				tango_device = True;
				delete tango_object;
				delete tango_attr_list;
				
				return (DS_OK);
				}
			}
		
		delete tango_object;
		delete tango_attr_list;	
		}
		
	catch (Tango::DevFailed &e)
		{
		if ( tango_object != NULL )
			{
			delete tango_object;
			}
			
		if ( tango_attr_list != NULL )
			{
			delete tango_attr_list;
			}
	
		//
	   // recover TANGO error string and save it in the
	   // global error string so that
		// it can be printed out with dev_error_str()
		//
			
      tango_dev_error_string (e);
      *error = DevErr_TangoAccessFailed;
			
      return(DS_NOTOK);		
		}
	
	*error = DevErr_AttributeNotFound;
	return (DS_NOTOK);
}



/**
 * Searches the requested attribute only in the TANGO system. This is used 
 * if the attribute name contains the tango: prefix. 
 *
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::search_tango_attr_name (DevLong *error)
{
	short	i;
	
	Tango::DeviceProxy	*tango_object;
	vector<string>			*tango_attr_list;
	

	*error 				= 0;
	tango_device 		= DS_NOTOK;
	tango_object 			= NULL;
	tango_attr_list 	= NULL;
 
	/*
	 * search attribute for a device in the TANGO database
	 */
	 
	try
		{
		*error = 0;
   	tango_object = new Tango::DeviceProxy (device_name);
		
		tango_attr_list = tango_object->get_attribute_list();
		for (i=0; i < (*tango_attr_list).size(); i++)
			{
			if ( strcasecmp ((*tango_attr_list)[i].c_str(), signal_name) == 0 )
				{
				tango_device = True;
				delete tango_object;
				delete tango_attr_list;
				
				return (DS_OK);
				}
			}
			
		delete tango_object;
		delete tango_attr_list;	
		}
		
	catch (Tango::DevFailed &e)
		{
		if ( tango_object != NULL )
			{
			delete tango_object;
			}
			
		if ( tango_attr_list != NULL )
			{
			delete tango_attr_list;
			}
	
		//
	   // recover TANGO error string and save it in the
	   // global error string so that
		// it can be printed out with dev_error_str()
		//
			
      tango_dev_error_string (e);
      *error = DevErr_TangoAccessFailed;
			
      return(DS_NOTOK);		
		}
		 
	*error = DevErr_AttributeNotFound;
	return (DS_NOTOK);
}

/**
 * Recover TANGO error string stack from DevFailed exception.
 *
 * @param e DevFailed exception
 * @param dev_error_string global pointer to error string
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
static long tango_dev_error_string(Tango::DevFailed tango_exception)
{
   for (int i=0; i<tango_exception.errors.length(); i++)
   {
	 dev_error_push(const_cast<char *>(tango_exception.errors[i].desc.in()));
   }
	
  return(DS_OK);
}


/**
 * Reads the state of the TACO or TANGO device of the attribute handle.
 * Maps the TANGO states to TACO state values.
 *
 * @param argout
 * @param argout_type
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::read_state (DevArgument argout, DevType argout_type, DevLong *error)
{
	Tango::DeviceData	tango_argout;
	Tango::DevState 	tango_state;
	short 				*taco_state;
	
	*error = 0;
	
	if ( tango_device == False )
	 	{
		/*
		 * Read the state of the TACO device
		 */
		
		if ( taco_dev_putget (taco_obj, 4, NULL, D_VOID_TYPE,
								    argout, argout_type, error) == DS_NOTOK )
			{
			return (DS_NOTOK);
			}
		}
	else
		{
		/* 
		 * Read the state of the TANGO device
		 */
		 
		try
			{
			taco_state = (short *)argout;
			
			tango_argout = tango_obj->command_inout ("State");
			tango_argout >> tango_state;
			
        	switch (tango_state) 
            {
            case (Tango::ON): 
					*taco_state = DEVON; 
               break;
            case (Tango::OFF): 
					*taco_state = DEVOFF; 
               break;
            case (Tango::CLOSE): 
					*taco_state = DEVCLOSE; 
               break;
            case (Tango::OPEN): 
					*taco_state = DEVOPEN; 
               break;
            case (Tango::INSERT): 
					*taco_state = DEVINSERTED; 
               break;
            case (Tango::EXTRACT): 
					*taco_state = DEVEXTRACTED; 
               break;
            case (Tango::MOVING): 
					*taco_state = DEVMOVING;
               break;
            case (Tango::STANDBY): 
					*taco_state = DEVSTANDBY;
               break;
            case (Tango::FAULT): 
					*taco_state = DEVFAULT;
               break;
            case (Tango::INIT): 
					*taco_state = DEVINIT;
					break;
            case (Tango::RUNNING): 
					*taco_state = DEVRUN;
               break;
            case (Tango::ALARM): 
					*taco_state = DEVALARM;
               break;
            case (Tango::DISABLE): 
					*taco_state = DEVDISABLED;
               break;
            default: 
					*taco_state = DEVUNKNOWN;
               break;
            }; 				
			}
		
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);				
			}			
		}	
	
	return (DS_OK);
}




/**
 * Reads the status of the TACO or TANGO device of the attribute handle.
 *
 * @param argout
 * @param argout_type
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::read_status (DevArgument argout, DevType argout_type, DevLong *error)
{
	Tango::DeviceData	tango_argout;
	const char 			*tango_status;
	char 					**taco_status;
		
	*error = 0;
	
	if ( tango_device == False )
	 	{
		/*
		 * Read the status of the TACO device
		 */
		
		if ( taco_dev_putget (taco_obj, 8, NULL, D_VOID_TYPE,
								    argout, argout_type, error) == DS_NOTOK )
			{
			return (DS_NOTOK);
			}
		}
	else
		{
		/* 
		 * Read the status of the TANGO device
		 */
		 
		try
			{
			tango_argout = tango_obj->command_inout ("Status");
			tango_argout >> tango_status;
			
			taco_status  = (char**) argout;
         *taco_status = (char*) malloc (strlen(tango_status) + 1);
         strcpy (*taco_status, tango_status);				
			}
		
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);				
			}			
		}	
	
	return (DS_OK);
}

/**
 * Sends the abort command to the TACO or TANGO device of the attribute handle.
 * Should be used to abort running action which has set the device into a moving state. 
 *
 * @param error Taco error code
 * 
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long	AttrAccess::abort	(DevLong *error)
{
	*error = 0;
	
	if ( tango_device == False )
	 	{
		/*
		 * Send DevAbort command to TACO device
		 */
		
		if ( taco_dev_put (taco_obj, DevAbort, NULL, D_VOID_TYPE, error) 
				== DS_NOTOK )
			{
			return (DS_NOTOK);
			}
		}
	else
		{
		/* 
		 * Send "Abort" command to the TANGO device
		 */
		 
		try
			{
			tango_obj->command_inout ("Abort");
			}
			
		catch (Tango::DevFailed &e)
			{	
			//
			// recover TANGO error string and save it in the
			// global error string so that
			// it can be printed out with dev_error_str()
			//
			
         tango_dev_error_string (e);
         *error = DevErr_TangoAccessFailed;
			
         return(DS_NOTOK);				
			}			 
		}									
	
	return (DS_OK);
}


/**
 * Reads the actual data source used when reading a Tango attribute.
 * The value is returnd as a short value:
 *	SOURCE_DEVICE					0
 *	SOURCE_CACHE  					1
 *	SOURCE_CACHE_DEVICE			2		
 *      				 
 * @param argout - data source of attribute value
 * @param argout_type D_SHORT_TYPE
 * @param error Taco error code
 * 
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */

long AttrAccess::read_attr_mode (DevArgument argout, DevType argout_type, DevLong *error)
{
	short	*source = (short *)argout;
	*error = 0;
	
	/*
	 * Read the attribute data source
	 */
	
	if ( tango_device == False )
	 	{
		// No data source handling implemented for Taco signals
		
		*error = DevErr_CommandNotImplemented;
		return (DS_NOTOK);
		}
	
	try
		{	
		Tango::DevSource tango_source = tango_obj->get_source();
		
		switch (tango_source)
			{
			case Tango::DEV:
				*source = SOURCE_DEVICE;
				break;
			
			case Tango::CACHE:
				*source = SOURCE_CACHE;
				break;

			case Tango::CACHE_DEV:
				*source = SOURCE_CACHE_DEVICE;
				break;
			
			default:
				*source = DS_NOTOK;
				break;
			}		
		}
	catch (Tango::DevFailed &e)
		{	
		//
		// recover TANGO error string and save it in the
		// global error string so that
		// it can be printed out with dev_error_str()
		//
			
      tango_dev_error_string (e);
      *error = DevErr_TangoAccessFailed;
			
      return(DS_NOTOK);				
		}	
	
	return (DS_OK);
}


/**
 * Set the data source used when reading a Tango attribute.
 * The value can be set as a short value:
 *	SOURCE_DEVICE					0
 *	SOURCE_CACHE  					1
 *	SOURCE_CACHE_DEVICE			2		
 *      				 
 * @param argout - data source of attribute value
 * @param argout_type D_SHORT_TYPE
 * @param error Taco error code
 * 
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long AttrAccess::write_attr_mode (DevArgument argin, DevType argin_type, DevLong *error)
{
	short	*source = (short *)argin;
	*error = 0;
	
	/*
	 * Set the attribute data source
	 */
	
	if ( tango_device == False )
	 	{
		// No data source handling implemented for Taco signals
		
		*error = DevErr_CommandNotImplemented;
		return (DS_NOTOK);
		}
		
	Tango::DevSource tango_source;
	switch (*source)
		{
		case SOURCE_DEVICE:
			tango_source = Tango::DEV;
			break;
			
		case SOURCE_CACHE:
			tango_source = Tango::CACHE;
			break;

		case SOURCE_CACHE_DEVICE:
			tango_source = Tango::CACHE_DEV;
			break;
			
		default:
			*error = DevErr_ParameterOutOfRange;
			return (DS_NOTOK);
			break;
		}
	
	try
		{	
		tango_obj->set_source(tango_source);
		}		
		
	catch (Tango::DevFailed &e)
		{	
		//
		// recover TANGO error string and save it in the
		// global error string so that
		// it can be printed out with dev_error_str()
		//
			
      tango_dev_error_string (e);
      *error = DevErr_TangoAccessFailed;
		
		return(DS_NOTOK);
		}
				
	return (DS_OK);
}

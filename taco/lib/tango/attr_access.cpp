static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/tango/attr_access.cpp,v 1.1 2005-03-29 09:27:49 andy_gotz Exp $";
/********************************************************************

 File       :	attribute_access.cpp

 Project    :	Interface to TACO signals and TANGO attributes

 Description:	
	
 Author(s)  :	Jens Meyer

 Original   :	Sptember2002

 $Revision: 1.1 $
 $Date: 2005-03-29 09:27:49 $

 $Author: andy_gotz $

 $Log: not supported by cvs2svn $
 Revision 8.84  2005/01/18 21:58:55  goetz
 changed prototype of ds__signal() back to extern C

 Revision 8.83  2004/11/26 13:25:56  meyer
 Corrected multi tango host gandling in dev_api.c and attr_access.cpp

 Revision 8.82  2004/10/26 11:05:30  goetz
 patched rpc_check_host for suse72

 Revision 8.81  2004/09/13 10:44:10  pons
 Minor fix in tango_api.cpp:get_cmd_value()

// Revision 8.80  2004/09/10  17:05:39  17:05:39  pons (Jean-Luc Pons)
// Added taco/tango command mapping
// 
// Revision 8.79  2004/08/04  08:51:25  08:51:25  pons (Jean-Luc Pons)
// Added support D_VAR_CHARARR in tango_api
// 
// Revision 8.78  2004/03/24  06:29:24  06:29:24  kimdon (David Kimdon)
// make it possible for C++ device servers to use only libdsapi++
// 
// Revision 8.77  2004/01/20  14:23:38  14:23:38  pons (Jean-Luc Pons)
// fixed a bug in rpc_check_host()
// 
 Revision 8.76  2004/01/13 14:30:21  meyer
 Bug correction in dev_put_asyn() and TACO initialisation even for TANGO usage!

// Revision 8.75  2003/11/26  09:35:05  09:35:05  verdier ()
// just a little mistake....
// 
 Revision 8.74  2003/11/26 09:20:01  verdier
 Array attribute added in query cmd

 Revision 8.73  2003/11/26 09:02:45  goetz
 changed rpc_check_host() algorithm for Solaris; intermediate checkin for Verdier

 Revision 8.72  2003/10/23 11:48:04  goetz
 added support for spectrum attributes

 Revision 8.71  2003/10/23 11:26:59  verdier
 Changed source for TANGO Spectrum or Image attribute reading to the device!

 Revision 8.70  2003/08/06 18:16:15  goetz
 include errno.h in API.h

 Revision 8.69  2003/06/30 11:21:35  goetz
 tango command numbers for taco are auto-generated if they are not defined in the kernel nor database

 Revision 8.68  2003/06/27 11:58:50  meyer
 Bug corrections for attributes

 Revision 8.67  2003/06/26 17:25:44  meyer
 Changed source for TANGO attribute reading to the device!

 Revision 8.66  2003/06/12 08:46:22  meyer
 Case insensitve attribute access in attr_access.cpp

 Revision 8.65  2003/05/14 13:34:24  goetz
 fixed bug with DevState and DevStatus commands for tango devices

 Revision 8.64  2003/04/25 15:11:55  meyer
 Corrected bugs in attr_access.cpp and MDSSignal_config.c

 Revision 8.63  2003/04/23 14:52:05  goetz
 set cmd_value to zero if not defined for TANGO devices

 Revision 8.62  2003/04/23 14:40:46  goetz
 fixed bug in mapping State and Status to DevState and DevStatus in tango_api.cpp

 Revision 8.61  2003/04/22 19:30:58  goetz
 ported toGNU 3.2 on Solaris

 Revision 8.60  2003/02/24 08:28:30  meyer
 Added DevStatus to attribute access

 Revision 8.58  2003/02/11 08:45:53  goetz
 fixed memory leak in tango_get_cmd_value()

 Revision 8.57  2003/02/06 13:33:47  goetz
 added header to cern_select

// Revision 8.56  2003/02/05  20:08:24  20:08:24  goetz (Andy Goetz)
// improved tango device management - made it dynamic, struct based, non vectorial
// 
 Revision 8.55  2003/02/05 13:36:49  goetz
 modified to run multiple copies of the same device server on Windows; debug messages changed to dprintf() on Windows

 Revision 8.54  2002/11/26 17:35:57  meyer
 First release of attribute interface

// Revision 8.52  2002/11/07  16:24:32  16:24:32  meyer (Jens Meyer)
// Added functionality for state reading and abort.
// 
// Revision 8.51  2002/11/07  12:40:26  12:40:26  meyer (Jens Meyer)
// Tested for attribute read, write and query
// 
 Revision 8.50  2002/10/30 17:50:59  meyer
 First version with taco/tango attribute interface

 Revision 2.2  2002/10/28 11:52:54  meyer
 Tango problem

 Revision 2.1  2002/10/15 14:24:47  meyer
 *** empty log message ***

 
 Copyright (c) 1999 by European Synchrotron Radiation Facility,
                       Grenoble, France

*********************************************************************/ 

#include <attr_api.h>

// copy from tango_api.cpp, because declared as static function
static long tango_dev_error_string(Tango::DevFailed tango_exception);

// global TACO error message string
extern char *dev_error_string;

#undef DevState
#undef RUNNING

/*+**********************************************************************
 Function   :   AttrAccess::AttrAccess()

 Description:   Constructor for the attribute access class.
 					 Takes a four field attribute name as input.
					 Verifies whether it is TACO or a TANGO attribute
					 and opens a connection to the underlying device.
					 
 Arg(s) In  :   char *full_attr_name 	- name of Taco or TANGO attribute
 					 long	access			 	- Taco security access
 
 Arg(s) Out :   long	*error  				- Taco error code

 Return(s)  :   none
***********************************************************************-*/
AttrAccess::AttrAccess (char *full_attr_name, long access, long *error)
{
	char	*str_ptr;
	short	count;

	*error 				= 0;
	tango_device 		= False;
	taco_signal_index = (-1);
	
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
		sprintf (device_name, "%s", (attr_name));
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



/*+**********************************************************************
 Function   :   AttrAccess::~AttrAccess()

 Description:   Destructor for the attribute access class.
					 Closes the connection to an attribute and
					 cleans-up the memory.
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/
AttrAccess::~AttrAccess()
{
	long	error;
	
	/* 
	 * close access to the underlying device
	 */
	 
	 if ( tango_device == False )
	 	{
		if ( taco_dev_free (taco_obj, &error) == DS_NOTOK )
			{
			printf ("Cannot free access to %s :\n%s\n", attr_name, 
																	  dev_error_str (error));
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
			printf ("Cannot free access to %s :\n%s\n", attr_name, 
																	  dev_error_string);
         return;				
			}
		}
}



/*+**********************************************************************
 Function   :   long AttrAccess::attr_cmd_query()

 Description:   Maps the read and write functionality of attributes
					 to Taco commands.
					 A read only attribute can execute only DevRead and
					 a read/write attribute the commands DevRead and
					 DevWrite.
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   DevVarCmdArray *varcmdarr - Pointer to command array.
 					 long	*error  				   - Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::attr_cmd_query (DevVarCmdArray *attr_cmd_query_array,
											long *error)
{
	DevCmdInfo *cmd_info;
	
	*error = 0;
	
	/*
	 * Get the number of commands to return
	 */
	 
	if ( attr_config.writable != Tango::READ )
		{
		attr_cmd_query_array->length   = 5;
		}
	else
		{
		attr_cmd_query_array->length   = 4;
		}
	
	/*
 	 * Allocate memory for a sequence of DevCmdInfo structures
 	 * returned with attr_cmd_query_array.
 	 */
	
	cmd_info = (DevCmdInfo *) malloc
	    		  (attr_cmd_query_array->length * sizeof (DevCmdInfo));
	if ( cmd_info == NULL )
	{
		*error  = DevErr_InsufficientMemory;
		return (DS_NOTOK);
	}	 
	
	/*
	 * Command DevRead to read attribute values
	 */
	 
	cmd_info[0].cmd = 28;
	sprintf (cmd_info[0].cmd_name, "DevRead");
	cmd_info[0].in_name = NULL;
	cmd_info[0].in_type = D_VOID_TYPE;
	cmd_info[0].out_name = NULL;
	cmd_info[0].out_type = get_taco_data_type (attr_config.data_type);
	
	/*
	 * Command DevAbort to stop a moving device via the attribute handle
	 */
	 
	cmd_info[1].cmd = 33;
	sprintf (cmd_info[1].cmd_name, "DevAbort");
	cmd_info[1].in_name = NULL;
	cmd_info[1].in_type = D_VOID_TYPE;
	cmd_info[1].out_name = NULL;
	cmd_info[1].out_type = D_VOID_TYPE;	
	
	/*
	 * Command DevState to read the device state via a attribute handle
	 */
	 
	cmd_info[2].cmd = 4;
	sprintf (cmd_info[2].cmd_name, "DevState");
	cmd_info[2].in_name = NULL;
	cmd_info[2].in_type = D_VOID_TYPE;
	cmd_info[2].out_name = NULL;
	cmd_info[2].out_type = D_SHORT_TYPE;	

	/*
	 * Command DevStatus to read the device status via a attribute handle
	 */
	 
	cmd_info[3].cmd = 8;
	sprintf (cmd_info[3].cmd_name, "DevStatus");
	cmd_info[3].in_name = NULL;
	cmd_info[3].in_type = D_VOID_TYPE;
	cmd_info[3].out_name = NULL;
	cmd_info[3].out_type = D_STRING_TYPE;	
	
	/*
	 * Command DevWrite to write attribute values
	 */
	 
	/* Is the attribute writable? */
	
	if ( attr_config.writable != Tango::READ )
		{
		cmd_info[4].cmd = 29;
		sprintf (cmd_info[4].cmd_name, "DevWrite");
		cmd_info[4].in_name = NULL;
		if ( tango_device == False )
			{
			cmd_info[4].in_type = get_taco_data_type (taco_write_type);
			}
		else
			{
			cmd_info[4].in_type = get_taco_data_type (attr_config.data_type);
			}

		cmd_info[4].out_name = NULL;
		cmd_info[4].out_type = D_VOID_TYPE;
		}
	
	attr_cmd_query_array->sequence = cmd_info;	
	return (DS_OK);
}





long AttrAccess::read_attr_config (DevVarStringArray *attr_config_array,
											  long *error)
{
	*error = 0;
	
	return (DS_OK);
}





/*+**********************************************************************
 Function   :   long AttrAccess::write_attr()

 Description:   Writes the input argument to the attribute.
 					 The input value is casted if necessary and
					 written via the TACO command or the
					 TANGO write_attribute() methode.
					 
 Arg(s) In  :   DevArgument argin 	- Input argument
 					 DevType argin_type	- Type of input argument
 
 Arg(s) Out :   long	*error  				   - Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

long AttrAccess::write_attr (DevArgument argin, DevType argin_type, long *error)
{
	long							input_type;
	Tango::DeviceAttribute	attr_values;
	double						double_value;
	float							float_value;
	long							long_value;
	short							short_value;
	string						string_value;
	char							*c_str_ptr;
	void							*data_ptr;
	
	*error = 0;
	
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
			 * Convert input data if necessary to the attribute data type
			 */
			 
			if ( convert_data (input_type, argin, 
							       attr_config.data_type, data_ptr, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			
			/*
			 * Fill the TANGO attribute data object and write 
			 * the attribute value.
			 */
			 
			attr_values.set_name(signal_name);

			switch (attr_config.data_type)
				{
				case Tango::DEV_DOUBLE:
					// cout << "Write value = " << double_value << endl;
					attr_values << double_value;
					break;
					
				case Tango::DEV_LONG:
					attr_values << *((long *)data_ptr);
					break;
					
				case Tango::DEV_SHORT:
					attr_values << *((short *)data_ptr);
					break;
					
				case Tango::DEV_STRING:
					string_value = *((char **)data_ptr);
					attr_values << string_value;
					break;										
				}
				
			tango_obj->write_attribute (attr_values);
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



/*+**********************************************************************
 Function   :   long AttrAccess::read_attr()

 Description:   Reads the value of an attribute.
 					 The output data is casted to the requested data type
					 if possible.
					 The TACO command DEvReadSigValues or the TANGO methode
					 read_attribute() are used to read the value.
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   DevArgument argout 	- Output argument
 					 DevType argout_type	- Requested type for output argument
 					 long	*error  			- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

long AttrAccess::read_attr (DevArgument argout, DevType argout_type, long *error)
{
	Tango::DeviceAttribute	attr_values;
	double					double_value;
	float					float_value;
	long					long_value;
	short					short_value;
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
			//	Switch on TANGO argout
			switch (attr_config.data_type)
				{
				case Tango::DEV_DOUBLE:
					attr_values >> double_value;
					break;

				case Tango::DEV_LONG:
					attr_values >> long_value;
					break;

				case Tango::DEV_SHORT:
					attr_values >> short_value;
					break;

				case Tango::DEV_STRING:
					attr_values >> string_value;
					break;

				case Tango::DEVVAR_SHORTARRAY:
				case Tango::DEVVAR_LONGARRAY:
				case Tango::DEVVAR_DOUBLEARRAY:
					return to_taco_sequence(attr_values, argout,
								attr_config.data_type, argout_type, error);
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
			data_ptr = (void *)&short_value;
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
	 	attr_config.data_type==Tango::DEV_STRING)
		{
			if ( convert_data (attr_config.data_type, data_ptr, 
							 request_type, argout, error) == DS_NOTOK )
				return (DS_NOTOK);
		}

	return (DS_OK);
}

/*+**********************************************************************
 Function   :   long AttrAccess:to_xdr_sequence:()

 Description:   Convert a Tango sequence to the request TAco sequence
					 
 Arg(s) In  :   argin		Tango data read.
                data_type	argin and argout type.
 
 Arg(s) Out :   argout		Taco data returned
                long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::to_taco_sequence(Tango::DeviceAttribute attribute, 
                                 DevArgument argout,
								 DevType tango_type,
								 DevType taco_type,
								 long *error)
{
	vector<short>	s_vect;
	vector<long>	l_vect;
	vector<double>	d_vect;
	int		nb_data;
	short	status;

	//	Tango attribute to vector
	switch(tango_type)
	{
	case Tango::DEVVAR_SHORTARRAY:
		attribute >> s_vect;
		nb_data = s_vect.size();
		break;										

	case Tango::DEVVAR_LONGARRAY:
		attribute >> l_vect;
		nb_data = l_vect.size();
		break;										

	case Tango::DEVVAR_DOUBLEARRAY:
		attribute >> d_vect;
		nb_data = d_vect.size();
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
				sa.sequence[i] = (short)s_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				sa.sequence[i] = (short)l_vect[i];
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

	case D_VAR_LONGARR:
		DevVarLongArray	la;
		la.length = nb_data;
		la.sequence = new long[nb_data];
		for (int i=0 ; i<nb_data ; i++)
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
				la.sequence[i] = (long)s_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				la.sequence[i] = (long)l_vect[i];
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

	case D_VAR_DOUBLEARR:
		DevVarDoubleArray	da;
		da.length = nb_data;
		da.sequence = new double[nb_data];
		double	value;
		for (int i=0 ; i<nb_data ; i++)
		{
			switch(tango_type)
			{
			case Tango::DEVVAR_SHORTARRAY:
				da.sequence[i] = (double)s_vect[i];
				break;
			case Tango::DEVVAR_LONGARRAY:
				da.sequence[i] = (double)l_vect[i];
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
	}
}
/*+**********************************************************************
 Function   :   long AttrAccess:to_xdr_sequence:()

 Description:   Encode the outgoing arguments into XDR format.
	            Decode the arguments afterwards, to get the same memory
	            allocation by XDR as the dev_putget() working with RPCs.
					 
 Arg(s) In  :   argin		data read.
                data_type	argin and argout type.
 
 Arg(s) Out :   argout		data returned
                long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::to_xdr_sequence(DevArgument argin, 
                                 DevArgument argout,
								 DevType data_type,
								 long *error)
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


/*+**********************************************************************
 Function   :   long AttrAccess::create_attr_access()

 Description:   Opens a connection to the device of the attribute
 					 and initialises the attribute configuration structure.
					 The TANGO attribute_config structure is used to store
					 the values. All necessary information for TACO is 
					 read and also written into the TANGO attribute config
					 structure. 
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::create_attr_access (long *error)
{
	DevVarCmdArray     cmd_array;
	DevVarStringArray	 signal_config;
	char					 write_cmd_name[80];
	char					 *str_ptr;
	short					 i;
	
	
	*error = 0;
	
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
		 	 * Set the data source to direct device access
		 	 */
		 
		 	
			tango_obj->set_source(Tango::DEV);
			
			
			// printf ("Imported TANGO device : %s\n", device_name);
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
		attr_config.max_alarm = signal_config.sequence[7];
		attr_config.min_alarm = signal_config.sequence[8];
		attr_config.standard_unit = signal_config.sequence[11];					
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
			case Tango::DEV_SHORT:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_SHORTARRAY;
					break;
			case Tango::DEV_LONG:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_LONGARRAY;
					break;
			case Tango::DEV_DOUBLE:
				if (attr_config.data_format==Tango::SPECTRUM ||
					attr_config.data_format==Tango::IMAGE)
					attr_config.data_type = Tango::DEVVAR_DOUBLEARRAY;
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
}



/*+**********************************************************************
 Function   :   long AttrAccess::convert_data()

 Description:   Converts a data of a given type to another
 					 data type.
					 
 Arg(s) In  :   long data_type		- type of data to convert
 					 void *data_ptr		- data to convert
					 long conv_data_type - requested data type
					 
 Arg(s) Out :   void *conv_data_ptr	- converted data
 					 long	*error  			- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK   
***********************************************************************-*/
long AttrAccess::convert_data (long data_type, void *data_ptr,
										 long conv_data_type, void *conv_data_ptr, 
										 long *error)
{
	static char		c_string_value[80];
	char				**str_ptr;
	char				*c_str_ptr;
	double 			*double_ptr;
	float				*float_ptr;
	long				*long_ptr;
	short				*short_ptr;	
	
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
			short_ptr = (short *)data_ptr;
			// cout << "Read value = " << *short_ptr << endl;
			
			switch (conv_data_type)
				{
				case Tango::DEV_SHORT:
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

			
		default:
			*error = DevErr_CannotConvertAttributeDataType;
			return (DS_NOTOK);
			break;												
		}
	
	return (DS_OK);
}	


/*+**********************************************************************
 Function   :   long AttrAccess::check_requested_data_type()

 Description:   Checks whether the attribute data type can be
 					 casted into the requested data type.
					 
 Arg(s) In  :   long request_type	- requested data type
 					 long attr_type		- attribute data type
					 
 Arg(s) Out :   long	*error  			- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK   
***********************************************************************-*/
long AttrAccess::check_requested_data_type (long request_type, long attr_type, 
														  long *error)
{
	*error = 0;
	if ( request_type != attr_type )
		{
		switch (request_type)
			{
			case Tango::DEV_SHORT:
				*error = DevErr_CannotConvertAttributeDataType;
				return (DS_NOTOK);
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



/*+**********************************************************************
 Function   :   long AttrAccess::get_tango_data_type()

 Description:   Converts TACO data type to TANGO data type 
					 
 Arg(s) In  :   long taco_data_type - TACO data type
 
 Arg(s) Out :   none

 Return(s)  :   long tango_data_type - TANGO data type
***********************************************************************-*/
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


/*+**********************************************************************
 Function   :   long AttrAccess::get_taco_data_type()

 Description:   Converts TANGO data type to TACO data type 
					 
 Arg(s) In  :   long tango_data_type - TANGO data type
 
 Arg(s) Out :   none

 Return(s)  :   long taco_data_type - TACO data type
***********************************************************************-*/
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
		case Tango::DEVVAR_LONGARRAY:
			return D_VAR_LONGARR;
			break;
		case Tango::DEVVAR_DOUBLEARRAY:
			return D_VAR_DOUBLEARR;
			break;

		default:
			return (D_VOID_TYPE);
			break;
		}
}


/*+**********************************************************************
 Function   :   long AttrAccess::search_attr_name()

 Description:   Searches the requested attribute in the TACO and
 					 the TANGO system.
					 The TACO database is searched first. If the name is
					 not defined for TACO the TANGO database is searched. 
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::search_attr_name (long *error)
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



/*+**********************************************************************
 Function   :   long AttrAccess::search_tango_attr_name()

 Description:   Searches the requested attribute only in
 					 the TANGO system. This is used if the attribute
					 name contains the tango: prefix. 
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::search_tango_attr_name (long *error)
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




/*+**********************************************************************
 Function   :   long tango_dev_error_string()

 Description:   Recover TANGO error string stack from DevFailed exception.

 Arg(s) In  :   Tango::DevFailed e - DevFailed exception

 Arg(s) Out :   char *dev_error_string - global pointer to error string

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

static long tango_dev_error_string(Tango::DevFailed tango_exception)
{
   for (int i=0; i<tango_exception.errors.length(); i++)
   {
      if (i == 0)
      {
         dev_error_string = (char*)malloc(strlen
									 (tango_exception.errors[i].desc.in())+1);
         sprintf(dev_error_string, "%s", tango_exception.errors[i].desc.in());
      }
      else
      {
         dev_error_string = (char*)realloc(dev_error_string,strlen
									 (dev_error_string) + strlen
									 (tango_exception.errors[i].desc.in())+1);
         sprintf(dev_error_string+strlen(dev_error_string), "%s",
									 tango_exception.errors[i].desc.in());
      }
   }
	
  return(DS_OK);
}


/*+**********************************************************************
 Function   :   long AttrAccess::read_state()

 Description:   Reads the state of the TACO or TANGO device
 					 of the attribute handle.
					 Maps the TANGO states to TACO state values.
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::read_state (DevArgument argout, DevType argout_type, long *error)
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




/*+**********************************************************************
 Function   :   long AttrAccess::read_status()

 Description:   Reads the status of the TACO or TANGO device
 					 of the attribute handle.
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long AttrAccess::read_status (DevArgument argout, DevType argout_type, long *error)
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





/*+**********************************************************************
 Function   :   long AttrAccess::abort()

 Description:   Sends the abort command to the TACO or TANGO device
 					 of the attribute handle.
					 Should be used to abort running action which has
					 set the device into a moving state. 
					 
 Arg(s) In  :   none
 
 Arg(s) Out :   long	*error  	- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long	AttrAccess::abort	(long *error)
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

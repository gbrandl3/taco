static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/tango/attr_api.cpp,v 1.1 2005-03-29 09:27:50 andy_gotz Exp $";
/********************************************************************

 File       :	attr_api.cpp

 Project    :	Interface to TACO signals and TANGO attributes

 Description:	
	
 Author(s)  :	Jens Meyer

 Original   :	September 2002

 $Revision: 1.1 $
 $Date: 2005-03-29 09:27:50 $

 $Author: andy_gotz $

 Copyright (c) 2002-2005 by European Synchrotron Radiation Facility,
                       Grenoble, France

*********************************************************************/ 
#include <attr_api.h>

/*+**********************************************************************
 Function   :   long attribute_import()

 Description:   Hook to TACO dev_import().
 					 Takes a four field attribute name as input.
					 Verifies whether it is TACO or a TANGO attribute
					 and opens a connection to the underlying device.

 Arg(s) In  :   char *attribute_name - name of Taco or TANGO attribute
 					 long	access			 - Taco security access

 Arg(s) Out :   devserver *ds_ptr 	 - pointer for client handle to attribute
 					 long	*error			 - Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long attribute_import (char *attribute_name, long access, 
							  devserver *ds_ptr, long *error)
{
	AttrAccess	*attr;
	
	*error = 0;
	
	/*
	 * Create attribute access
	 */
	 
	attr = new AttrAccess (attribute_name, access, error);
	if ( attr == NULL || *error != DS_OK )
		{
		return (DS_NOTOK);
		}
		
	/*
	 * Create the TACO client handle
	 */ 
	 
	if ((*ds_ptr = (devserver)malloc(sizeof(struct _devserver))) == NULL)
		{
      *error = DevErr_InsufficientMemory;
      return(DS_NOTOK);
      }
		
   sprintf((*ds_ptr)->device_name, "%s", attribute_name);
   sprintf((*ds_ptr)->device_class,"AttributeClass");
   sprintf((*ds_ptr)->device_type,"attribute");
   sprintf((*ds_ptr)->server_name,"Unknown");
   sprintf((*ds_ptr)->server_host,"Unknown");
   (*ds_ptr)->clnt             = (CLIENT *)attr;
   (*ds_ptr)->ds_id            = 0;
   (*ds_ptr)->no_svr_conn      = 0;
   (*ds_ptr)->rpc_conn_counter = 0;
   (*ds_ptr)->dev_access       = access;
   (*ds_ptr)->i_nethost        = 0;
   (*ds_ptr)->rpc_protocol     = D_IIOP;
   (*ds_ptr)->rpc_timeout.tv_sec  = 0;
   (*ds_ptr)->rpc_timeout.tv_usec = 0;			
		
	return (DS_OK);
}

/*+**********************************************************************
 Function   :   long attribute_free()

 Description:   Hook to TACO dev_free().
 					 Frees the connectio to an attribute.
					 
 Arg(s) In  :   devserver ds - client handle to attribute
 
 Arg(s) Out :   long	*error  - Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long attribute_free (devserver ds, long *error)
{
	AttrAccess	*attr;

	*error = 0;
	attr = (AttrAccess *) ds->clnt;
	
	delete attr;
	free (ds);
	
	return (DS_OK);
}


/*+**********************************************************************
 Function   :   long attribute_cmd_query()

 Description:   Hook to TACO dev_cmd_query().
 					 Maps the read and write functionality of attributes
					 to Taco commands.
					 
 Arg(s) In  :   devserver ds 					- client handle to attribute
 
 Arg(s) Out :   DevVarCmdArray *varcmdarr - Pointer to command array.
 					 long	*error  				   - Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long attribute_cmd_query (devserver ds, DevVarCmdArray *varcmdarr, long *error)
{
	AttrAccess	*attr;

	*error = 0;
	attr = (AttrAccess *) ds->clnt;

	if ( attr->attr_cmd_query (varcmdarr, error) == DS_NOTOK )
		{
		return (DS_NOTOK);
		}
	
	return (DS_OK);
}



/*+**********************************************************************
 Function   :   long attribute_putget()

 Description:   Hook to TACO dev_putget().
 					 Calls the correct attribute access methode for 
					 commands defined for the attribute access.
					 
 Arg(s) In  :   devserver ds 			- client handle to attribute
 					 long cmd				- command to execute
					 DevArgument argin	- input argument to write attribute
					 DevType argin_type	- data type of input argument
 
 Arg(s) Out :   DevArgument argout  - output argument to read attribute
 					 DevType argout_type - data type of output argument
 					 long	*error  			- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long attribute_putget (devserver ds, long cmd, DevArgument argin,
			  				 DevType argin_type,DevArgument argout,
			  				 DevType argout_type, long *error)
{
	AttrAccess	*attr;

	*error = 0;
	attr = (AttrAccess *) ds->clnt;
	
	switch (cmd)
		{
		case DevRead:
		 	if ( attr->read_attr (argout, argout_type, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;
			
		case DevWrite:
		 	if ( attr->write_attr (argin, argin_type, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;
				
		case DevState:
		 	if ( attr->read_state (argout, argout_type, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;
			
		case DevStatus:
		 	if ( attr->read_status (argout, argout_type, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;				
						
		case DevAbort:
		 	if ( attr->abort (error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;
			
		default:
			*error = DevErr_CommandNotImplemented;
			return (DS_NOTOK);		
			break;						
		}
	
	return (DS_OK);
}



/*+**********************************************************************
 Function   :   long attribute_put()

 Description:   Hook to TACO dev_put().
 					 Calls the correct attribute access methode to  
					 write attribute values.
					 
 Arg(s) In  :   devserver ds 			- client handle to attribute
 					 long cmd				- command to execute
					 DevArgument argin	- input argument to write attribute
					 DevType argin_type	- data type of input argument
 
 Arg(s) Out :   long	*error  			- Taco error code

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
long attribute_put (devserver ds, long cmd, DevArgument argin,
		       		  DevType argin_type, long *error)
{
	AttrAccess	*attr;

	*error = 0;
	attr = (AttrAccess *) ds->clnt;
	
	switch (cmd)
		{
		case DevWrite:
		 	if ( attr->write_attr (argin, argin_type, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;
						
		case DevAbort:
		 	if ( attr->abort (error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;
			
		default:
			*error = DevErr_ValueOutOfBounds;
			return (DS_NOTOK);		
			break;						
		}	
	
	return (DS_OK);
}


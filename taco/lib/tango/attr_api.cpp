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
 * File       :	attr_api.cpp
 *
 * Project    :	Interface to TACO signals and TANGO attributes
 *
 * Description:	
 *	
 * Author(s)  :	Jens Meyer
 * 		$Author: andy_gotz $
 *
 * Original   :	September 2002
 *
 * Version    : $Revision: 1.5 $
 *
 * Date       : $Date: 2006-09-19 09:31:24 $
 *
 *********************************************************************/ 

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <attr_api.h>
static char RcsId[] = "@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/tango/attr_api.cpp,v 1.5 2006-09-19 09:31:24 andy_gotz Exp $";

/**
 * Hook to TACO dev_import().
 *
 * Takes a four field attribute name as input.  Verifies whether it is TACO or a TANGO attribute
 * and opens a connection to the underlying device.  
 *
 * @param attribute_name name of Taco or TANGO attribute
 * @param access Taco security access
 * @param ds_ptr pointer for client handle to attribute
 * @param error	Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
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

/**
 * Hook to TACO dev_free(). Frees the connectio to an attribute.
 *
 * @param ds client handle to attribute
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
long attribute_free (devserver ds, long *error)
{
	AttrAccess	*attr;

	*error = 0;
	attr = (AttrAccess *) ds->clnt;
	
	delete attr;
	free (ds);
	
	return (DS_OK);
}


/**
 * Hook to TACO dev_cmd_query().
 *
 * Maps the read and write functionality of attributes to Taco commands.
 *
 * @param ds client handle to attribute
 * @param varcmdarr Pointer to command array.
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
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



/**
 * Hook to TACO dev_putget().
 *
 * Calls the correct attribute access methode for commands defined for the attribute access.
 *
 * @param ds client handle to attribute
 * @param cmd command to execute
 * @param argin input argument to write attribute
 * @param argin_type data type of input argument
 * @param argout output argument to read attribute
 * @param argout_type data type of output argument
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
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
	
		case DevReadSetValue:
		 	if ( attr->read_set_attr (argout, argout_type, error) == DS_NOTOK )
				{
				return (DS_NOTOK);
				}
			break;	
			
		case DevIOState:
		 	if ( attr->read_attr_state (argout, argout_type, error) == DS_NOTOK )
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
			
		case DevReadConfig:
		 	if ( attr->read_attr_config (argout, argout_type, error) == DS_NOTOK )
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

/**
 * Hook to TACO dev_put().
 *
 * Calls the correct attribute access methode to  write attribute values.
 *
 * @param ds client handle to attribute
 * @param cmd command to execute
 * @param argin input argument to write attribute
 * @param argin_type data type of input argument
 * @param error Taco error code
 *
 * @return DS_NOTOK in case of failure otherwise DS_OK
 */
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


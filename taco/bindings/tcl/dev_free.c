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
 * File:        dev_free.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: Functions to free on or all client handles opened 
 *		by dev_io().
 *
 * Author(s):   J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:    July 1997
 *
 * Version:     $Revision: 1.4 $
 *
 * Date:        $Date: 2008-04-06 09:06:33 $
 */

#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <dev_io.h>
#include <debug.h>

extern IMPORTED_DEVICE *FirstDevice;  /* Head list */


/*======================================================================
 Function:      int dev_io_free()

 Description:   Frees the connection to one device specified with
		its device name. Called by the Tcl interpreter.

		Tcl syntax: 	dev_io_free status error device_name

 =======================================================================*/
int dev_io_free (ClientData clientdata, Tcl_Interp *interp, 
		 int argc, char **argv)
{
	IMPORTED_DEVICE 	*CurrentDevice;
	char			StringTemp[256];
	char			*State;
	char			*Error;
	char			*DeviceName;
	char			*error_str;
	DevLong			DeviceError  = 0;
	short			found        = False;
	short			not_imported = False;

	/*
	 * Check the input argument
	 */

	if ( argc != 4 )
	{
		Tcl_AppendResult (interp, "should be... dev_io_free 'status' 'error' 'device'", (char *)NULL);
		return TCL_ERROR;
 	}

 	State       = argv[1];
 	Error       = argv[2];
 	DeviceName  = argv[3];

      	/* Transforming State and Error in Tcl variable */
	Tcl_SetVar (interp,State,"0",0); /* No error by default */
	Tcl_SetVar (interp,Error,"",0);

	/*
	 * Search the device in the list of imported devices.
	 */

	CurrentDevice = FirstDevice->Next;
 	do
	{
/* Check if this device is already marked as imported */
		if ( strcmp (CurrentDevice->Name, DeviceName) == 0 )
		{
/* The device is in the list, but was already freed */
			if ( CurrentDevice->imported_flag == False )
				not_imported = True;

			found = True;
			break;
		}
		CurrentDevice = CurrentDevice->Next;
	}
 	while (CurrentDevice != NULL);

/*
 * The device name is not known, return an error.
 */
	if ( found == False || not_imported == True )
	{
		Tcl_SetVar (interp,State,"-1",0);

		DeviceError = DevErr_DeviceOfThisNameNotServed;
		error_str = dev_error_str (DeviceError);
		snprintf (StringTemp, sizeof(StringTemp), "%s [%ld]", error_str, (long)DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		free (error_str);
		return TCL_OK;
	}

/*
 * Free the connection to the device and set the
 * imported_flag to false.
 */
	if ( dev_free (CurrentDevice->ds, &DeviceError) == DS_NOTOK )
	{
		Tcl_SetVar (interp,State,"-1",0);

		error_str = dev_error_str (DeviceError);
		snprintf (StringTemp, sizeof(StringTemp), "%s [%ld]", error_str, (long)DeviceError);
		Tcl_SetVar (interp,Error,StringTemp,0);
		free (error_str);
	}

/*
 * Set the imported_flag to False, because the device connection
 * was freed.
 */
	CurrentDevice->imported_flag = False;
	return TCL_OK;
}
                  



/*======================================================================
 Function:      int dev_io_free_all()

 Description:   Frees the connections to all devices already imported
		by calling dev_io(). Called by the Tcl interpreter.

		Tcl syntax: 	dev_io_free status error

 =======================================================================*/
int dev_io_free_all (ClientData clientdata, Tcl_Interp *interp, 
		     int argc, char **argv)
{
	IMPORTED_DEVICE 	*CurrentDevice;
	char			*State;
	char			*Error;
	DevLong			DeviceError  = 0;

/*
 * Check the input argument
 */
	if ( argc != 3 )
 	{
  		Tcl_AppendResult (interp, "should be... dev_io_free_all 'status' 'error'", (char *)NULL);
		return TCL_ERROR;
 	}

 	State       = argv[1];
 	Error       = argv[2];
/* Transforming State and Error in Tcl variable */
	Tcl_SetVar (interp,State,"0",0); /* No error by default */
	Tcl_SetVar (interp,Error,"",0);

/*
 * Search the device in the list of imported devices.
 */
	CurrentDevice = FirstDevice->Next;
 	do
	{
/* The device is in the list, but was not freed */
		if ( CurrentDevice->imported_flag == True )
		{
			dev_free (CurrentDevice->ds, &DeviceError);
			CurrentDevice->imported_flag = False;
		}
    		CurrentDevice = CurrentDevice->Next;
   	}
 	while (CurrentDevice != NULL);

	return TCL_OK;
}

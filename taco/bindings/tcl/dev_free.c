static char RcsId[] =
" $Header: /home/jkrueger1/sources/taco/backup/taco/bindings/tcl/dev_free.c,v 1.1 2003-03-18 16:28:29 jkrueger1 Exp $ ";
/**
 * File:        dev_free.c
 *
 * Description: Functions to free on or all client handles opened 
 *		by dev_io().
 *
 * Author(s):   J.Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:    July 1997
 *
 * Version:	$Version$
 *
 * Date:	$Date: 2003-03-18 16:28:29 $
 *
 * Copyright(c) 1997 by European Synchrotron Radiation Facility,
 *                     Grenoble, France
 *********************************************************************/

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
	long			DeviceError  = 0;
	short			found        = False;
	short			not_imported = False;

	/*
	 * Check the input argument
	 */

	if ( argc != 4 )
 	   {
  	   Tcl_AppendResult (interp,
		"should be... dev_io_free 'status' 'error' 'device'", 
		(char *)NULL);
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
	         {
		 not_imported = True;
		 }

	      found = True;
	      break;
	      }
    	   CurrentDevice = CurrentDevice->Next;
   	   }
 	while (CurrentDevice != NULL);

	/*
	 * The device name is not known, return an error.
	 */

	if ( found == False ||
	     not_imported == True )
	   {
	   Tcl_SetVar (interp,State,"-1",0);

	   DeviceError = DevErr_DeviceOfThisNameNotServed;
	   error_str = dev_error_str (DeviceError);
	   sprintf (StringTemp,"%s [%d]", error_str, DeviceError);
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
	   sprintf (StringTemp,"%s [%d]", error_str, DeviceError);
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
	long			DeviceError  = 0;

	/*
	 * Check the input argument
	 */

	if ( argc != 3 )
 	   {
  	   Tcl_AppendResult (interp,
		"should be... dev_io_free_all 'status' 'error'", 
		(char *)NULL);
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

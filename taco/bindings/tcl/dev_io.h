/* 
 * File: 	dev_io.h
 *
 * Author(s): 	Denis Beauvois 
 * 		Gilbert Pepellin
 * 		J.Meyer
 *		$Author: jkrueger1 $
 *		
 * Original: 	June 1996
 *             
 * Description: This file is necessary to compile dev_io.c
 *           	It contains all the necessary datas
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2004-07-14 09:35:46 $
*/

#ifndef DEV_IO_H
#define DEV_IO_H 

#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <Admin.h>
#include <dev_xdr.h>

#include <tcl.h>

#include <string.h>

/* general defines */
#define False 0
#define True  1

/* For the flag which determines the type of the IO */
#define SYNCHRONOUS_MODE 0
#define ASYNCHRONOUS_MODE 1

/* For the flag which determines the debug mode */
#define DEBUG_LEVEL_0 0 /* no debug */
#define DEBUG_LEVEL_1 1
#define DEBUG_LEVEL_2 2
#define DEBUG_LEVEL_3 3 /* total debug */

/* Number of the types known by dev_io */
#define NUMBER_OF_TYPES 29

/* For having a link between the number of type and a string */
typedef struct DEVICE_TYPE {
                            DevType Type;
                            char *Name;
                           } DEVICE_TYPE;
                           
/* For keeping in memory the list of imported device */
typedef struct IMPORTED_DEVICE  {
                     	char 		*Name;
			short		imported_flag; /* Device is imported
					with a valid client handle if True */
                       	devserver 	ds;
                   	DevVarCmdArray 	InfoArray; /* authorized commands */
                      	struct 		IMPORTED_DEVICE *Next; /* linked list */
                        } IMPORTED_DEVICE;
                               
/* Number of arguments required in command line */
#define NUMBER_ARGUMENTS_MIN 4
#define NUMBER_ARGUMENTS_MAX 5

/* Flag for knowing if we use InputArg or OutputArg in the convert functions */
#define INPUT 0
#define OUTPUT 1

/* Necessary for the dev_putget command */
extern 	void 	*InputArg,
		*OutputArg;

/* Necessary for variable array */
extern 	void 	*SequencePointer;

/* length of TclAux variable */
#define TCLAUX_LENGTH 80 
                            
/* Necessary for the type string */
extern	DevString	StringType;
extern	DevString	StringTypeOut;  /* GP140197 */
extern 	DevString 	*StringArray;

       /* To encapsulate the variables */
IMPORTED_DEVICE* GetImportedDevice (IMPORTED_DEVICE *Device);
int SetIoMode (int Argin);
int GetIoMode ();
int GetDebugMode();
int SetDebugMode (int Argin);
int dev_io      (ClientData clientdata, Tcl_Interp *interp, 
	         int argc, char **argv);
int dev_io_asyn (ClientData clientdata, Tcl_Interp *interp,
                 int argc, char **argv);
int dev_io_free (ClientData clientdata, Tcl_Interp *interp,
                 int argc, char **argv);
int dev_io_free_all (ClientData clientdata, Tcl_Interp *interp,
                     int argc, char **argv);

#endif

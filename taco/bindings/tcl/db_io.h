/* 
 * File:	db_io.h		
 *
 * Author(s): 	Denis Beauvois 	
 *		Gilbert Pepellin
 *		J.Meyer
 *		$Author: jkrueger1 $
 *
 * Original: 	June, 1996
 *
 * Description: This is the include file for the new commands
 *	  	inside a tcl interpreter named db_io_get, 
 *	  	db_io_put and db_io_debug
 * Version:	$Revision: 1.1 $
 *
 * Date:	$Date: 2003-03-18 16:28:24 $
*/

#ifndef DB_IO_H
#define DB_IO_H	

#include <API.h>
#include <tcl.h>
#include <tk.h>

/* Number of types recognized by db_io_get */
#define NUMBER_OF_TYPES_KNOWN 11

/* Number of arguments required */
#define NUMBER_ARGUMENTS 6

/* Max length of the variable arrays */
#define MAX_LENGTH_VAR 100

/* Debug mode */
#define DB_DEBUG_0 0
#define DB_DEBUG_1 1

#ifndef False
#define False 0
#endif

#ifndef True
#define True 1
#endif

typedef struct DEVICE_KNOWN {
                             DevType Type;
                             char *Name;
                            } DEVICE_KNOWN;
                            
/* Prototypes of functions */
int SetDBDebugMode ();
int GetDBDebugMode ();
DevType GetType();





#endif

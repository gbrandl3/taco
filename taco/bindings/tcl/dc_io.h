/*	
 * File	: 	dc_io.h	
 *
 * Author(s): 	Denis Beauvois 	
 * 		Gilbert Pepellin
 * 		J.Meyer
 *		$Author: jkrueger1 $
 *
 * Original: 	June, 1996
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2003-05-07 13:19:25 $
 *
 */

#ifndef DC_IO_H
#define DC_IO_H 0

#include <API.h>
#include <dc.h>

#include <tcl.h>

#include <dev_io.h>

/* ---------------- */
/* general defines  */
/* ---------------- */

/* debug mode */
#define DC_DEBUG_0 0
#define DC_DEBUG_1 1

/* Call with history mode */
#define DC_HISTORY_MODE_OFF 0
#define DC_HISTORY_MODE_ON 1

/* Number of arguments in the command line */
#define DC_ARGUMENTS_NUMBER 5


/* For keeping in memory the list of imported device */
typedef struct DC_IMPORTED_DEVICE {
                                   char *Name;
                                   datco dc;
                                   struct DC_IMPORTED_DEVICE *Next;
                                  } DC_IMPORTED_DEVICE;

#endif		

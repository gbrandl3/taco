
//+**********************************************************************
//
// File:	Serial.h
//
// Project:	Device Servers in C++
//
// Description:	public include file containing definitions and declarations 
//		for implementing the Serial class in C++ (SerialClass).
//
// Author(s):	Manuel Perez
//
// Original:	May 1998 by Andy Gotz
//
//+**********************************************************************

#ifndef _Serial_H
#define _Serial_H
		
//#include <linux/types.h>
#include <BlcDsNumbers.h>

#define MAX_DEVICES 16


//
// Specific data structure
//
typedef struct _SerialLinePart
{
	char	*serialline; /* serial line device descriptor */
	int	serialin; /* serial line file descriptor used for input */
	int	serialout; /* serial line file descriptor for output */
	short	timeout; /* timeout in milliseconds */
	short	parity; /* parity */
	short	charlength; /* character size */
	short	stopbits; /* number of stop bits */
	short	baudrate; /* baud rate */
	short	newline; /* new line character */
	short	xon; /* output resume character */
	short	xoff; /* output suspend character */
	short	upper; /* upper case only flag */
	short	erase; /* erase on backspace flag */
	short	echo; /* input characters echoed flag */
	short	linefeed; /* automatic linefeed flag */
	short	null; /* set null character count */
	short	backspace; /* backspace character */
	short	del; /* delete character */
} SerialLinePart;





//
// Class definition
//
class Serial : public Device {

//
// private members
// 

 private :

   static short         class_inited;
   static short		class_destroyed;
   static short		debug;

   SerialLinePart 	serialline;

   long ClassInitialise	(long *error);
   long GetResources 	(char *res_name, long *error);
   long RawReadString 	(DevVoid *argin, DevString *argout, long *error);
   long NCharReadString	(DevLong *argin, DevString *argout, long *error);
   long LineReadString	(DevVoid *argin, DevString *argout, long *error);
   long RawReadChar	(DevVoid *argin, DevVarCharArray *argout, long *error);
   long NCharReadChar	(DevLong *argin, DevVarCharArray *argout, long *error);
   long LineReadChar	(DevVoid *argin, DevVarCharArray *argout, long *error);
   long Open_desc	(void);
   char * Decode_parameters(struct termios term, char eol);



//
// protected members
//
 protected:

   long StateMachine (long cmd, long *error);

//
// commands
//
   long SerWriteString	(void *argin, void *argout, long *error);
   long SerWriteChar	(void *argin, void *argout, long *error);
   long SerReadString	(void *argin, void *argout, long *error);
   long SerReadChar	(void *argin, void *argout, long *error);
   long SerSetParameter	(void *argin, void *argout, long *error);
   long Reset		(void *argin, void *argout, long *error);
   long Status		(void *argin, void *argout, long *error);
   long State		(void *argin, void *argout, long *error);

//
// public members
//
 public:

   Serial (char *name, long *error);
   ~Serial ();

};




//
// new commands specific to the Serial device server
//
#define DevSerWriteString	DevSerBase+1
#define DevSerWriteChar		DevSerBase+2
#define DevSerReadString	DevSerBase+3
#define DevSerReadChar		DevSerBase+4
#define DevSerSetParameter	DevSerBase+5




//
//maximum string length supported to date
//
#define SL_MAXSTRING 10240	// MP 22/02/99 : 1K -> 10K 




//
// symbolic defines - used by server and client alike
//
#define SL_RAW 0	/* raw read/write mode */
#define SL_NCHAR 1	/* character read/write mode */
#define SL_LINE 2	/* line read mode */

#define SL_NONE 0
#define SL_ODD 1
#define SL_EVEN 3

#define SL_DATA8 0
#define SL_DATA7 1
#define SL_DATA6 2
#define SL_DATA5 3

#define SL_STOP1 0
#define SL_STOP15 1
#define SL_STOP2 2

#define SL_TIMEOUT 3 	/* timeout parameter */
#define SL_PARITY 4 	/* number of parity bits parameter */
#define SL_CHARLENGTH 5	/* number of data bits parameter */
#define SL_STOPBITS 6 	/* number of stop bits parameter */
#define SL_BAUDRATE 7 	/* baud rate parameter */
#define SL_NEWLINE 8 	/* new line character parameter */


#endif /* _Serial_H */

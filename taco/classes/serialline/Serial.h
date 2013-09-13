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
 * File:	Serial.h
 *
 * Project:     Device Servers in C++
 *
 * Description:	public include file containing definitions and declarations 
 *		for implementing the Serial class in C++ (SerialClass).
 *
 * Author(s):	Manuel Perez
 *              $Author: jkrueger1 $
 *
 * Original:	May 1998 by Andy Gotz
 *
 * Version:     $Revision: 1.7 $
 *
 * Date:        $Date: 2008-10-14 09:59:03 $
 */

#ifndef _Serial_H
#define _Serial_H
		
#include <BlcDsNumbers.h>

#include <Device.h>

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

   long ClassInitialise	(DevLong *error);
   long GetResources 	(char *res_name, DevLong *error);
   long RawReadString 	(DevVoid *argin, DevString *argout, DevLong *error);
   long NCharReadString	(DevLong *argin, DevString *argout, DevLong *error);
   long LineReadString	(DevVoid *argin, DevString *argout, DevLong *error);
   long RawReadChar	(DevVoid *argin, DevVarCharArray *argout, DevLong *error);
   long NCharReadChar	(DevLong *argin, DevVarCharArray *argout, DevLong *error);
   long LineReadChar	(DevVoid *argin, DevVarCharArray *argout, DevLong *error);
   long Open_desc	(void);
   char * Decode_parameters(struct termios term, char eol);



//
// protected members
//
 protected:

   long StateMachine (DevCommand cmd, DevLong *error);

//
// commands
//
   long SerWriteString	(DevArgument argin, DevArgument argout, DevLong *error);
   long SerWriteChar	(DevArgument argin, DevArgument argout, DevLong *error);
   long SerReadString	(DevArgument argin, DevArgument argout, DevLong *error);
   long SerReadChar	(DevArgument argin, DevArgument argout, DevLong *error);
   long SerSetParameter	(DevArgument argin, DevArgument argout, DevLong *error);
   long Reset		(DevArgument argin, DevArgument argout, DevLong *error);
   long Status		(DevArgument argin, DevArgument argout, DevLong *error);
   long State		(DevArgument argin, DevArgument argout, DevLong *error);

//
// public members
//
 public:

   Serial (char *name, DevLong *error);
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
#define DevSerSetCharDelay      DevSerBase+6
#define DevSerSetLongParameter  DevSerBase+7
#define DevSerGetNChar          DevSerBase+8
#define DevSerWaitChar          DevSerBase+9
#define DevSerFlush             DevSerBase+10

//
//maximum string length supported to date
//
#define SL_MAXSTRING 10240	// MP 22/02/99 : 1K -> 10K 




//
// symbolic defines - used by server and client alike
//
#define SL_RAW 		0	/* raw read/write mode */
#define SL_NCHAR 	1	/* character read/write mode */
#define SL_LINE 	2	/* line read mode */

#define SL_NONE 	0
#define SL_ODD 		1
#define SL_EVEN 	3

#define SL_DATA8 	0
#define SL_DATA7 	1
#define SL_DATA6 	2
#define SL_DATA5 	3

#define SL_STOP1 	0
#define SL_STOP15 	1
#define SL_STOP2 	2

#define SL_TIMEOUT 	3 	/* timeout parameter */
#define SL_PARITY 	4 	/* number of parity bits parameter */
#define SL_CHARLENGTH 	5	/* number of data bits parameter */
#define SL_STOPBITS 	6 	/* number of stop bits parameter */
#define SL_BAUDRATE 	7 	/* baud rate parameter */
#define SL_NEWLINE 	8 	/* new line character parameter */


#endif /* _Serial_H */

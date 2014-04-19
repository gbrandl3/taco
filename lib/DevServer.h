/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:	DevServer.h
 * 
 * Project:	Device Servers
 * 
 * Description:	Public include for for applications wanting to
 * 		to use device servers.
 * 
 * Author(s):	Andy Goetz
 * 		Jens Meyer
 *  		$Author: jkrueger1 $
 * 
 * Original:	March 1990
 * 
 * Version:	$Revision: 1.11 $
 * 
 * Date:	$Date: 2008-04-06 09:06:58 $
 * 
 ********************************************************************-*/

#ifndef _DevServer_h
#define _DevServer_h

#include <API.h>

#ifdef __cplusplus
#include <Device.h>
#endif /* __cplusplus */

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevServerh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevServer.h,v 1.11 2008-04-06 09:06:58 jkrueger1 Exp $";
#endif /* _IDENT */

typedef long DevMethod;

#define Fontaine_ps 	1
#define WDK_ps 		2
#define Ritz_ps		3
#define Pirani_gauge 	4
#define Penning_gauge 	5
#define Fluor_screen	6
#define Vacuum_gauges	7
#define Dan8000_ps	8

/*
 * standard methods, available in every class
 */
#define DevMethodCreate			1
#define DevMethodDestroy		2
#define DevMethodClassInitialise	3
#define DevMethodStateHandler		4

/*
 * DevServerClass methods
 */
#define DevMethodCommandHandler		10
#define DevMethodErrorHandler		11
#define DevMethodInitialise		12
#define DevMethodDevExport		13

/*
 * Device dependent Methods
 */
#define DevMethodLineOpen		101
#define DevMethodLineClose		102
#define DevMethodWrite			103
#define DevMethodRead			104
#define DevMethodWriteCommand		105
#define DevMethodReadAnswer		106
#define DevMethodReadError		107
#define DevMethodControllerInitialise	108
#define DevMethodChangeCalib		109
#define DevMethodSetProtThresh		110
#define DevMethodResetInputBuffer	111
#define DevMethodDev_Diagnostics	112


/*
 * miscellaneous methods
 */
#define DevMethodHelloWorld		100

#ifndef __cplusplus
typedef long 	int(*DevMethodFunction)();
#else
typedef long 	int(*DevMethodFunction)(...);
#endif /* __cplusplus */

typedef struct _DevMethodListEntry {
		DevMethod method;
		DevMethodFunction fn;
	}DevMethodListEntry;

typedef struct _DevMethodListEntry *DevMethodList;


typedef struct _DevServerClassRec *DevServerClass;
typedef struct _DevServerRec *DevServer;

#ifndef __cplusplus
extern DevServerClass devServerClass;
#else
extern "C" DevServerClass devServerClass;
#endif

/*
 * typedefs for the device access structures
 * with security.
 */

typedef struct _DevServerAccess {
#ifndef __cplusplus
/*
 * OIC version
 */
	DevServer 		ds;
#else
/*
 * C++ version
 */
	Device			*device;
#endif
		char  			export_name[LONG_NAME_SIZE];
		long 			export_status;
		long 			export_counter;
		long    		single_user_flag;
		long    		admin_user_flag;
		long			si_client_id;
		long			si_access_right;
		long			si_connection_id;
		long			si_tcp_socket;
		struct sockaddr_in 	si_peeraddr;
	} DevServerDevices;

/*
 * Include security system definitions
 */

#include <DevSec.h>

/*
 * the method finder, indispensable for OOPS
 */

#ifdef __cplusplus
extern "C" {
#endif
DevMethodFunction ds__method_finder PT_( (void *ptr_ds, DevMethod method) );

long ds__method_search PT_( (void *ptr_ds_class, DevMethod method, DevMethodFunction *function_ptr) );
long ds__create PT_( (char *name, void *ptr_ds_class, void *ptr_ds_ptr, DevLong *error) );
long ds__destroy PT_( (void *ptr_ds, DevLong *error) );
long ds__svcrun PT_( (DevLong *error) );
long startup PT_( (char* svr_name,DevLong* error) );
long svc_check PT_( (DevLong* error) );
long db_check PT_( (DevLong* error) );


long ds__signal PT_( (int sig, void (*action)(int), long *error) );
int gettransient PT_( (const char *ds_name) );
void main_signal_handler PT_( (int sig) );
void unregister_server PT_( (void) );

#ifdef __cplusplus
	}
#endif /* __cplusplus */


/*
 * Convenience functions for the Device Server programs. These provide 
 * and alternate calling sequence for executing methods of the root 
 * class. 
 */

#ifdef __cplusplus
extern "C" {
#endif

long dev_cmd PT_( (void *ptr_ds, long cmd, DevArgument argin,
			long argin_type, DevArgument argout, long argout_type,
			DevLong *error) );

#ifdef __cplusplus
void dev_event_fire PT_( (Device *device, long event_type,
				DevArgument argout, DevType argout_type,
	 			long event_status, long event_error) );
long dev_export (char *name, Device *ptr_dev, DevLong *error);
}
#else
void dev_event_fire PT_( (DevServer ds, long event_type,
				DevArgument argout, DevType argout_type,
				long event_status, long event_error) );
long dev_export  PT_( (char *dev_name, void *ptr_ds, DevLong *error) );
#endif

/*
 * NT stuff for DevServerMain and its various Window elements
 */

#ifdef WIN32
extern HINSTANCE 	ghAppInstance; /*  the application's module handle */
extern HWND 		ghWndMain;     /*  the main window handle */
extern char* 		gszAppName;    /*  the application's name */

/*
#include "NT_debug.h"    // is done for all files in macros.h
*/
 
/*
 * Function called from 'libdsapi' for delayed startup. Useful for
 * Windows applications to perform startup operations when Window's
 * GUI has been initialized. If function pointer is NULL, no delayed
 * startup will take place.
 */
extern long (*DelayedStartup)();
/* 
 * Function called from 'libdsapi' for clean shutdown. Useful for
 * Windows applications to perform shutdown operations before the Window's
 * process is shutdown. If function pointer is NULL, no delayed
 * startup will take place.
 */
extern void (*OnShutDown)();

/* an array of strings to be displayed on the main window backdrop */
typedef struct {
		int lines;
		char **text;
	} MainWndTextDisplay;
extern MainWndTextDisplay gMWndTxtDisplay;

#endif  /* WIN32 */

#endif /* _DevServer_h */

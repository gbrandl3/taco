
/*+*******************************************************************

 File:		DevServerP.h

 Project:	Device Servers with SUN-RPC

 Description:	Private include file for applications using the device
 		server class.

 Author(s):	Andy Goetz , Jens Meyer
		$Author: jkrueger1 $

 Original:	March 1990

 Version:	$Revision: 1.2 $

 Date:		$Date: 2003-03-14 16:53:27 $

 Copyright (c) 1990-1997 by European Synchrotron Radiation Facility, 
                            Grenoble, France

********************************************************************-*/

#ifndef _DevServerP_h
#define _DevServerP_h

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevServerPh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevServerP.h,v 1.2 2003-03-14 16:53:27 jkrueger1 Exp $";
#endif /* _IDENT */

/*
 *  define to add a default device type
 *  if nothing is specified.
 */

#define TYPE_INIT	"DevType_"
#define TYPE_DEFAULT	"DevType_Default"


typedef struct _DevServerClassPart {
               int                  n_methods;
               DevMethodList        methods_list;
               DevServerClass       superclass;
               DevString	    class_name;
               DevBoolean           class_inited;
               int                  n_commands;
               DevCommandList       commands_list;
               int                  n_events;
               DevEventList         events_list;
                                  }
               DevServerClassPart;

typedef struct _DevServerPart {
               DevString       name;
	       char            dev_type[DEV_TYPE_LENGTH];
               DevServerClass  class_pointer;
               long            state;
               long            n_state;
                             }
               DevServerPart;

typedef struct _DevServerClassRec {
               DevServerClassPart 	devserver_class;
                                  }
               DevServerClassRec;

typedef struct _DevServerRec {
               DevServerPart		devserver;
                             }
               DevServerRec;

#ifdef __cplusplus
typedef char * (* DevRpcLocalFunc)(...);
#endif

#endif /* _DevServerP_h */

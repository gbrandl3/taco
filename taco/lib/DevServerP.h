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
 * File:	DevServerP.h
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	Private include file for applications using the device
 * 		server class.
 *
 * Author(s):	Andy Goetz
 *		Jens Meyer
 *		$Author: jkrueger1 $
 *
 * Original:	March 1990
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:06:58 $
 *
 ********************************************************************-*/

#ifndef _DevServerP_h
#define _DevServerP_h

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevServerPh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevServerP.h,v 1.5 2008-04-06 09:06:58 jkrueger1 Exp $";
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
} DevServerClassPart;

typedef struct _DevServerPart {
	DevString       name;
	char            dev_type[DEV_TYPE_LENGTH];
	DevServerClass  class_pointer;
	long            state;
	long            n_state;
} DevServerPart;

typedef struct _DevServerClassRec {
	DevServerClassPart 	devserver_class;
} DevServerClassRec;

typedef struct _DevServerRec {
	DevServerPart		devserver;
} DevServerRec;

#ifdef __cplusplus
typedef char * (* DevRpcLocalFunc)(...);
#endif

#endif /* _DevServerP_h */

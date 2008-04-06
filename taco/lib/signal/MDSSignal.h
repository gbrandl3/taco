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
 * File:	MDSSignal.h
 *
 * Project:	Multi Signal Device Server
 *
 * Description:	Device Server Class which is a top Layer of DSSignal
 *		Class to handle multiple signals.
 *
 * Author(s):	Ingrid Fladmark
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.5 $
 *
 * Original:	24th of january 1997
 *
 * Date:	$Date: 2008-04-06 09:07:18 $
 *
 *********************************************************************/

#ifndef _MDSSIGNAL_H
#define _MDSSIGNAL_H

#include <CntrlDsNumbers.h>

typedef struct _MDSSignalClassRec *MDSSignalClass;
typedef struct _MDSSignalRec *MDSSignal;

extern MDSSignalClass mDSSignalClass;

#define DevMethodCheckAlarms    	DevMSignalBase + 1
#define DevMethodReadAlarms    		DevMSignalBase + 2
#define DevMethodSignalsReset   	DevMSignalBase + 3
#define DevMethodGetSigObject		DevMSignalBase + 4

/*************************************************************************
 * Example of definition of errors which are not belonging to DevErrors.h
 * See DSN096.
 *************************************************************************
 */

#define DevErr_SignalNameNotFound    	DevMSignalBase + 1
#define DevErr_SignalListNotFound    	DevMSignalBase + 2

/*
 * Prototype of extraction function for signal configuration.
 * Defined in module MDSSignal_config.c
 */

#ifdef __cplusplus
extern "C" {
#endif

long dev_get_sig_config (char *device_name, DevVarStringArray *argout,
                         DevLong *error);

long dev_get_sig_list (char *device_name, DevVarStringArray *argout, 
		       DevLong *error);

long dev_get_sig_config_from_name (char *device_name, char *signal_name , 
		                   DevVarStringArray *argout, DevLong *error);

void get_sig_prop_suffix (DevVarStringArray *suf_list );
#ifdef __cplusplus
}
#endif


#endif /*_MDSSIGNAL_H*/

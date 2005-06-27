/*********************************************************************
 *
 * File:	MDSSignal.h
 *
 * Project:	Multi Signal Device Server
 *
 * Description:	Device Server Class which is a top Layer of DSSignal
 		Class to handle multiple signals.
 *		
 *
 * Author(s):	Ingrid Fladmark
 *		$Author: andy_gotz $
 *
 * Version:	$Revision: 1.3 $
 *
 * Original:	24th of january 1997
 *
 * Date:	$Date: 2005-06-27 12:53:06 $
 *
 * Copyright(c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
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
                         long *error);

long dev_get_sig_list (char *device_name, DevVarStringArray *argout, 
		       long *error);

long dev_get_sig_config_from_name (char *device_name, char *signal_name , 
		                   DevVarStringArray *argout, long *error);

void get_sig_prop_suffix (DevVarStringArray *suf_list );
#ifdef __cplusplus
}
#endif


#endif /*_MDSSIGNAL_H*/

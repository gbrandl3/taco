/*********************************************************************
 *
 * File:	MDSSignalP.h
 *
 * Project:	Multi Signal Device Server
 *
 * Description:	Device Server Class which is a top Layer of DSSignal
 		Class to handle multiple signals.
 *
 * Author(s):	Ingrid Fladmark
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.2 $
 *
 * Original:	24th of january 1997	
 *
 * Date:	$Date: 2005-03-30 06:28:13 $
 *
 * Copyright (c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *
 *********************************************************************/

#ifndef _MDSSIGNALP_H
#define _MDSSIGNALP_H

#include <DSSignalP.h>
#define 	MAXLENGTH	128


typedef struct _MDSSignalClassPart
{
   int	nada;
} MDSSignalClassPart;



typedef struct _MDSSignalPart
{
   	DevVarStringArray signal_names; /* List of all signals to handle. */
	Dssignal	  *signal_obj;
	DevVarStringArray *signals_properties;
	char              **property_ptr;
	long              *alarm_result;
} MDSSignalPart;



typedef struct _MDSSignalClassRec
{
   DevServerClassPart	devserver_class;
				
   MDSSignalClassPart	mDSSignal_class;
} MDSSignalClassRec;



extern MDSSignalClassRec mDSSignalClassRec;


typedef struct _MDSSignalRec
{
   DevServerPart 	devserver;
				
   MDSSignalPart 	mDSSignal;
} MDSSignalRec;

#endif /*_MDSSIGNALP_H*/


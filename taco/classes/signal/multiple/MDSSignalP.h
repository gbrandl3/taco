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
 * File:	MDSSignalP.h
 *
 * Project:	Multi Signal Device Server
 *
 * Description:	Device Server Class which is a top Layer of DSSignal
 *		Class to handle multiple signals.
 *
 * Author(s):	Ingrid Fladmark
 *              $Author: jkrueger1 $
 *
 * Original:	24th of january 1997	
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2008-04-06 09:06:38 $
 */

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
	DevLong           *alarm_result;
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


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
 * File:	DSSignalP.h
 *
 * Project:	SRRF3 control system
 *
 * Description:	A class to specify signal properties.
 *
 * Author(s):	J.Meyer
 *              $Author: jkrueger1 $
 *
 * Original:	June 1996	
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 22:41:57 $
 */

#ifndef _DSSignalP_H
#define _DSSignalP_H

#include <CntrlDsNumbers.h>

/*
 * Define the methods of the signal class
 */

#define DevMethodReadProperties		DevSignalBase + 1
#define DevMethodCheckAlarm		DevSignalBase + 2
#define DevMethodReadAlarm		DevSignalBase + 3
#define DevMethodCheckLimits		DevSignalBase + 4 
#define DevMethodSignalReset		DevSignalBase + 5


#define NUMBER_OF_RES		11	

typedef struct _DssignalClassPart
{
   	int		none;
} DssignalClassPart;



typedef struct _DssignalPart
{
	char	*label;
	char	*unit;
	char	*format;
	char	*description;
	char    *alarm_high;
	double 	alarm_high_value;
	char  	*alarm_low;
	double  alarm_low_value;
	char  	*max_limit;
	double  max_limit_value;
	char	*min_limit;
	double	min_limit_value;
	char	*dev_class_name;
	long	delta_time_value;
	char	*delta_time;
	double	delta_value;
	char	*delta;
	long	last_set_time;
	double	last_set_value;
	char	*last_alarm_msg;
	char	*std_unit_multiplier;
	double	std_unit_multiplier_value;
} DssignalPart;



typedef struct _DssignalClassRec
{
   	DevServerClassPart	devserver_class;
   	DssignalClassPart	dssignal_class;
} DssignalClassRec;



extern DssignalClassRec dssignalClassRec;


typedef struct _DssignalRec
{
   	DevServerPart 		devserver;
   	DssignalPart 		dssignal;
} DssignalRec;


typedef struct _DssignalClassRec *DssignalClass;
typedef struct _DssignalRec *Dssignal;

extern DssignalClass dssignalClass;

void  load_NaN(void *v);
int  Is_NaN(void *v);

#endif /*_DSSignalP_H*/

/*static char SccsId[] = " @(#) PowerSupply.h 1.2 90/04/27 14:48:03 ";*/
/*********************************************************************

 File:		PowerSupply.h

 Project:	Device Servers

 Description:	Public include file for applications using the power
		supply device server

 Author(s):	Andy Goetz

 Original:	March 1990


 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

		       All Rights Reserved

 *********************************************************************/

#ifndef _PowerSupply_h
#define _PowerSupply_h

typedef struct _PowerSupplyClassRec *PowerSupplyClass;
typedef struct _PowerSupplyRec *PowerSupply;

extern PowerSupplyClass powerSupplyClass;

#endif /* _PowerSupply_h */

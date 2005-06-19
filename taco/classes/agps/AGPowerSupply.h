/*static char RcsId[] = " $Header: /segfs/dserver/classes/powersupply/ag/include/RCS/AGPowerSupply.h,v 1.10 1997/12/02 15:26:14 klotz Rel $ ";*/

/*********************************************************************

 File:		AGPowerSupply.h

 Project:	Device Servers

 Description:	private include file for implementing the class
		of AG simulated powersupplies. 

 Author(s);	Andy Goetz 

 Original:	March 1991

 $Log: AGPowerSupply.h,v $
 Revision 1.10  1997/12/02 15:26:14  klotz
 Win_NT_95_released_NOV_97

 * Revision 1.1  96/08/21  14:22:29  14:22:29  goetz (Andy Goetz)
 * Initial revision
 * 

 Copyleft (c) 1991 by European Synchrotron Radiation Facility, 
                      Grenoble, France

 *********************************************************************/

#ifndef _AGPowerSupply_h
#define _AGPowerSupply_h

typedef struct _AGPowerSupplyClassRec *AGPowerSupplyClass;
typedef struct _AGPowerSupplyRec *AGPowerSupply;

extern AGPowerSupplyClass aGPowerSupplyClass;
extern AGPowerSupply aGPowerSupply;

/*
 * public symbols
 */


#endif 

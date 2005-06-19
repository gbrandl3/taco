/*static char RcsId[] = " $Header: /segfs/dserver/classes/powersupply/ag/include/RCS/AGPowerSupplyP.h,v 1.11 2000/06/22 09:10:48 goetz Exp $ ";*/

/*********************************************************************

 File:		AGPowerSupplyP.c

 Project:	Device Servers

 Description:	private include file for the class
		of AG simulated powersupplies.

 Author(s);	Andy Goetz 

 Original:	October 1990

 $Log: AGPowerSupplyP.h,v $
 Revision 1.11  2000/06/22 09:10:48  goetz
 *** empty log message ***

 * Revision 1.10  97/12/02  15:26:14  15:26:14  klotz (W.D. Klotz)
 * Win_NT_95_released_NOV_97
 * 
 * Revision 1.1  96/08/21  14:22:32  14:22:32  goetz (Andy Goetz)
 * Initial revision
 * 

 Copyleft (c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France


 *********************************************************************/

#ifndef _AGPOWERSUPPLYP_h
#define _AGPOWERSUPPLYP_h

/*
 * as subclass of the powerSupplyClass include PowerSupplyClass private
 * definitions
 */

#include <PowerSupplyP.h>

typedef struct _AGPowerSupplyClassPart {
                                         int nada;
                                        }
                AGPowerSupplyClassPart;

typedef struct _AGPowerSupplyPart {
                                    int nada;
                                   }
                AGPowerSupplyPart;

typedef struct _AGPowerSupplyClassRec {
                                         DevServerClassPart devserver_class;
                                         PowerSupplyClassPart powersupply_class;
                                         AGPowerSupplyClassPart agpowersupply_class;
                                       }
                AGPowerSupplyClassRec;
    
extern AGPowerSupplyClassRec aGPowerSupplyClassRec;

typedef struct _AGPowerSupplyRec {
                                    DevServerPart devserver;
                                    PowerSupplyPart powersupply;
                                    AGPowerSupplyPart agpowersupply;
                                  }
                AGPowerSupplyRec;

/*
 * private constants to be used in the AGPowerSupplyClass
 */

#define AG_MAX_CUR	100.0
#define AG_MIN_CUR 	0.0
#define AG_PER_ERROR 	0.001

/* fault values */

#define AG_OVERTEMP	0x01
#define AG_NO_WATER	0x02
#define AG_CROWBAR	0x04
#define AG_RIPPLE	0x08
#define AG_MAINS	0x10
#define AG_LOAD		0x20
#define AG_TRANSFORMER	0x40
#define AG_THYRISTOR	0x80

#endif /* _AGPOWERSUPPLYP_h */


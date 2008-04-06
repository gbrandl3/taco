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
 * File:        AGPowerSupplyP.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description:	private include file for the class
 *		of AG simulated powersupplies.
 *
 * Author(s);	Andy Goetz 
 *              $Author: jkrueger1 $
 *
 * Original:    October 1990
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#ifndef _AGPOWERSUPPLYP_h
#define _AGPOWERSUPPLYP_h

/*
 * as subclass of the powerSupplyClass include PowerSupplyClass private
 * definitions
 */

#include <PowerSupplyP.h>

typedef struct _AGPowerSupplyClassPart {
	int nada;
} AGPowerSupplyClassPart;

typedef struct _AGPowerSupplyPart {
	int nada;
} AGPowerSupplyPart;

typedef struct _AGPowerSupplyClassRec {
	DevServerClassPart 	devserver_class;
	PowerSupplyClassPart 	powersupply_class;
	AGPowerSupplyClassPart agpowersupply_class;
} AGPowerSupplyClassRec;
    
extern AGPowerSupplyClassRec aGPowerSupplyClassRec;

typedef struct _AGPowerSupplyRec {
	DevServerPart devserver;
	PowerSupplyPart powersupply;
	AGPowerSupplyPart agpowersupply;
} AGPowerSupplyRec;

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


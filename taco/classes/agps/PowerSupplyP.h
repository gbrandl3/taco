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
 * File:        PowerSupplyP.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description:	Private include file for applications using the
 *		power supply device server
 *
 * Author(s):	Andy Goetz
 *              $Author: jkrueger1 $
 *
 * Original:	March 1990
 *
 * Version:     $Revision: 1.9 $
 *
 * Date:        $Date: 2006/04/20 06:33:17 $
 */

#ifndef _PowerSupplyP_h
#define _PowerSupplyP_h

typedef struct _PowerSupplyClassPart {
	int nada;
} PowerSupplyClassPart;

typedef struct _PowerSupplyPart {
	DevType type;
	float set_val;
	float read_val;
	int channel;
	int n_ave;
	int fault_val;
	float cal_val;
	float conv_val;
	char *conv_unit;
	float set_offset;
	float read_offset;
	float set_u_limit;
	float set_l_limit;
	float idot_limit;
	int polarity;
	float delta_i;
	long time_const;
	long last_set_t;
} PowerSupplyPart;

typedef struct _PowerSupplyClassRec {
	DevServerClassPart devserver_class;
} PowerSupplyClassRec;

extern PowerSupplyClassRec powerSupplyClassRec;

typedef struct _PowerSupplyRec {
	DevServerPart 	devserver;
	PowerSupplyPart powersupply;
} PowerSupplyRec;

#define DevMethodCheckReadValue 1000
#define DevMethodPSInitialise   1001

#endif /* _PowerSupplyP_h */

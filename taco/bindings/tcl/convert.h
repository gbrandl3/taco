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
 * File :       convert.c
 *
 * Project:     TACO TCL bindings
 *
 * Description: This file contains function prototypes 
 *
 * Author(s):   Denis Beauvois, ESRF
 *              Gilbert Pepellin
 *              J. Meyer
 *              $Author: jkrueger1 $
 *
 * Original:    November 2007
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2008-04-06 09:06:33 $
 */

#ifndef CONVERT_H
#define CONVERT_H

int F_D_BOOLEAN_TYPE (Tcl_Interp *, char *, int);
int F_D_USHORT_TYPE (Tcl_Interp *, char *, int);
int F_D_SHORT_TYPE (Tcl_Interp *, char *, int);
int F_D_ULONG_TYPE (Tcl_Interp *, char *, int);
int F_D_LONG_TYPE (Tcl_Interp *, char *, int);
int F_D_FLOAT_TYPE (Tcl_Interp *, char *, int);
int F_D_DOUBLE_TYPE (Tcl_Interp *, char *, int);
int F_D_STRING_TYPE (Tcl_Interp *, char *, int);
int F_D_INT_FLOAT_TYPE (Tcl_Interp *, char *, int);
int F_D_FLOAT_READPOINT (Tcl_Interp *, char *, int);
int F_D_STATE_FLOAT_READPOINT (Tcl_Interp *, char *, int);
int F_D_LONG_READPOINT (Tcl_Interp *, char *, int);
int F_D_DOUBLE_READPOINT (Tcl_Interp *, char *, int);
int F_D_VAR_CHARARR (Tcl_Interp *, char *, int);
int F_D_VAR_STRINGARR (Tcl_Interp *, char *, int);
int F_D_VAR_USHORTARR (Tcl_Interp *, char *, int);
int F_D_VAR_SHORTARR (Tcl_Interp *, char *, int);
int F_D_VAR_ULONGARR (Tcl_Interp *, char *, int);
int F_D_VAR_LONGARR (Tcl_Interp *, char *, int);
int F_D_VAR_FLOATARR (Tcl_Interp *, char *, int);
int F_D_VAR_DOUBLEARR (Tcl_Interp *, char *, int);
int F_D_VAR_FRPARR (Tcl_Interp *, char *, int);
int F_D_VAR_SFRPARR (Tcl_Interp *, char *, int);
int F_D_VAR_LRPARR (Tcl_Interp *, char *, int);
int F_D_OPAQUE_TYPE (Tcl_Interp *, char *, int);
int F_D_MULMOVE_TYPE (Tcl_Interp *, char *, int);
int F_D_MOTOR_LONG (Tcl_Interp *, char *, int);
int F_D_MOTOR_FLOAT (Tcl_Interp *, char *, int);

#endif

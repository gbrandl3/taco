/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:   	tc_str.h
 *
 * Project:   	Type conversion library
 *
 * Description: The header filefor the C module including all the conversion
 *              functions from dev_xdr kernel types to string and the other
 *              way around (from string to the dev_xdr kernel types).
 *
 * Author(s):   Faranguiss Poncet
 *              $Author: jkrueger1 $
 * Original:   	November 1996
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:01:35 $
 *
 *****************************************************************************/

#ifndef TC_STR_H
#define TC_STR_H

#include <dev_xdr.h>
#include <maxe_xdr.h>

/*--------------   Conversion from types TO STRING   ---------------------*/


long tc_DevVoid_to_str(DevVoid *d_void, char *out_str);

long tc_DevBoolean_to_str(DevBoolean *d_boolean, char *out_str);

long tc_DevShort_to_str(DevShort *d_short, char *out_str);

long tc_DevUShort_to_str(DevUShort *d_ushort, char *out_str);

long tc_DevLong_to_str(DevLong *d_long, char *out_str);

long tc_DevULong_to_str(DevULong *d_ulong, char *out_str);

long tc_DevFloat_to_str(DevFloat *d_float, char *out_str);

long tc_DevDouble_to_str(DevDouble *d_double, char *out_str);

long tc_DevString_to_str(DevString *d_string, char *out_str);

long tc_DevIntFloat_to_str(DevIntFloat *d_intfloat, char *out_str);

long tc_DevFloatReadPoint_to_str(DevFloatReadPoint *d_flrp, char *out_str);

long tc_DevStateFloatReadPoint_to_str(DevStateFloatReadPoint *d_stflrp,
                                      char *out_str);

long tc_DevLongReadPoint_to_str(DevLongReadPoint *d_lnrp, char *out_str);

long tc_DevDoubleReadPoint_to_str(DevDoubleReadPoint *d_dblrp, char *out_str);

long tc_DevVarCharArray_to_str(DevVarCharArray *d_char_arr, char *out_str);

long tc_DevVarStringArray_to_str(DevVarStringArray *d_string_arr, char *out_str);

long tc_DevVarUShortArray_to_str(DevVarUShortArray *d_ushort_arr, char *out_str);

long tc_DevVarShortArray_to_str(DevVarShortArray *d_short_arr, char *out_str);

long tc_DevVarULongArray_to_str(DevVarULongArray *d_ulong_arr, char *out_str);

long tc_DevVarLongArray_to_str(DevVarLongArray *d_long_arr, char *out_str);

long tc_DevVarFloatArray_to_str(DevVarFloatArray *d_float_arr, char *out_str);

long tc_DevVarDoubleArray_to_str(DevVarDoubleArray *d_dbl_arr, char *out_str);

long tc_DevVarFloatReadPointArray_to_str(DevVarFloatReadPointArray *d_flrp_arr, char *out_str);

long tc_DevVarStateFloatReadPointArray_to_str( DevVarStateFloatReadPointArray *d_stflrp_arr, char *out_str);

long tc_DevVarLongReadPointArray_to_str( DevVarLongReadPointArray *d_lnrp_arr, char *out_str);


/*--------------   Conversion FROM STRING to types   ---------------------*/


long tc_str_to_DevVoid(char *in_str, DevVoid *d_void);

long tc_str_to_DevBoolean(char *in_str, DevBoolean *d_boolean);

long tc_str_to_DevShort(char *in_str, DevShort *d_short);

long tc_str_to_DevUShort(char *in_str, DevUShort *d_ushort);

long tc_str_to_DevLong(char *in_str, DevLong *d_long);

long tc_str_to_DevULong(char *in_str, DevULong *d_ulong);

long tc_str_to_DevFloat(char *in_str, DevFloat *d_float);

long tc_str_to_DevDouble(char *in_str, DevDouble *d_double);

long tc_str_to_DevString(char *in_str, DevString *d_string);

long tc_str_to_DevIntFloat(char *in_str, DevIntFloat *d_intfl);

long tc_str_to_DevFloatReadPoint(char *in_str, DevFloatReadPoint *d_flrp);

long tc_str_to_DevStateFloatReadPoint(char *in_str, DevStateFloatReadPoint *d_stflrp);

long tc_str_to_DevLongReadPoint(char *in_str, DevLongReadPoint *d_lngrp);

long tc_str_to_DevDoubleReadPoint(char *in_str, DevDoubleReadPoint *d_dblrp);

long tc_str_to_DevVarCharArray(char *in_str, DevVarCharArray *d_char_arr);

long tc_str_to_DevVarStringArray(char *in_str, DevVarStringArray *d_str_arr);

long tc_str_to_DevVarUShortArray(char *in_str, DevVarUShortArray *d_ushort_arr);

long tc_str_to_DevMulMove(char *in_str, DevMulMove *d_mulmove);

long tc_str_to_DevMotorLong(char *in_str, DevMotorLong *d_motorl);

long tc_str_to_DevMotorFloat(char *in_str, DevMotorFloat *d_motorf);


#endif /* tc_str_h */



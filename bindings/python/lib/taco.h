/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright(c) 1994-2014 by European Synchrotron Radiation Facility,
 *                     Grenoble, France
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
 * File:        taco.h
 *
 * Description: interface python - taco
 *
 * Author(s):   MCD
 *		$Author: jkrueger1 $
 *
 * Original:    December 99
 *
 * Date:	$Date: 2005-07-25 13:33:42 $
 *
 * Version:	$Revision: 1.3 $
 */

#ifndef _taco_py
#define _taco_py

long get_argin_single(DevArgument, long, PyObject *, char *);

long get_argin_array(DevArgument, long, PyObject *, char *);

long get_argin_special(DevArgument, long, PyObject *, char *);

long get_argout_single(DevArgument, long, PyObject **, char *);

long get_argout_array(DevArgument, long, PyObject **, char *);

long get_argout_special(DevArgument, long, PyObject **, char *);

long check_type(long, long *, long *, long *);

void display_single(DevArgument, long, char *);

void display_array(DevArgument, long, char *);

void display_special(DevArgument, long, char *);

long lenoftype(long);     

long Ctype2numeric(long);

#endif

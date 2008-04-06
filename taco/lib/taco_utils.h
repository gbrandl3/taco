/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 2007 Jens Krüger <jens.krueger@frm2.tum.de>
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
 * File       : util_api.c
 *
 * Project    : Device Servers with SUN-RPC
 *
 * Description: Application Programmers Interface
 *              Utilities for the interface to access and
 *              handle remote devices.
 *
 * Author(s)  : Jens Krüger <jens.krueger@frm2.tum.de>
 *              $Author: jkrueger1 $
 *
 * Original   : June 2007
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:                $Date: 2008-04-06 09:07:01 $
 *
 ********************************************************************-*/

#ifndef	TACO_UTILS_H
#define	TACO_UTILS_H

/**
 * @defgroup utilsAPI Some helpful utility functions
 * @ingroup API
 *
 * This group collects some helpful functions, i.e. copy and convert strings
 */
 
#ifdef __cplusplus
extern "C" {
#endif

char *str_tolower(char *str);
char *strcpy_tolower(char *dest, const char *src);
char *strncpy_tolower(char *dest, const char *src, size_t n);
char *strcat_tolower(char *dest, const char *src);
char *strdup_tolower(const char *str);
#ifdef _solaris
int setenv(const char *name, const char *value, int overwrite);
#endif /* _solaris */

#ifdef __cplusplus
}
#endif

#endif

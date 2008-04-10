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
 * Date:                $Date: 2008-04-10 15:02:29 $
 *
 ********************************************************************-*/
#ifdef HAVE_CONFIG_H
#       include "config.h"
#endif

#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup utilsAPI
 * Convert a string str to lower characters. The string will be changed.
 *
 * @param str pointer to the string to be converted;
 *
 * @return pointer to the converted string;
 */
char *str_tolower(char *str)
{
	int i;
	int len = strlen(str);
	for(i = 0; i < len; ++i)
		str[i] = tolower(str[i]);
	return str;
}	

/**
 * @ingroup utilsAPI
 * Duplicate the string given by str and converts the duplicated string
 * to lower case. The returned string must be freed.
 *
 * @param str string to be duplicated and convert
 *
 * @see strdup
 *
 * @return duplicated and converted string
 */
char *strdup_tolower(const char *str)
{
	char *tmp = strdup(str);
	if (tmp != NULL)
		return str_tolower(tmp);
	return tmp;
}

/**
 * @ingroup utilsAPI
 * Copy a string whereas the copy is converted to lower cases.
 *
 * @param dest target for the copy operation
 * @param src string to be copied and converted
 *
 * @see strcpy
 *
 * @return the copied and converted string
 */
char *strcpy_tolower(char *dest, const char *src)
{
	char 	*tmp,
		*ptr = dest;
	for (tmp = (char *)src; *tmp != '\0'; ++tmp, ++ptr)
		*ptr = tolower(*tmp);
	*ptr = '\0';
	return dest;
}	

/**
 * @ingroup utilsAPI
 * Copy n byte from the source src to the destination dest whereas the 
 * copy is converted to lower cases.
 *
 * @param dest target for the copy operation
 * @param src string to be copied and converted
 * @param n number of bytes to be copied
 *
 * @see strncpy
 *
 * @return the copied and converted string
 */
char *strncpy_tolower(char *dest, const char *src, size_t n)
{
	int i;
	for (i = 0; i < n; ++i)
		dest[i] = tolower(src[i]);
	return dest;
}

/**
 * @ingroup utilsAPI
 * Append the src string to the dest string overwriting the '\0' character at the end of dest, 
 * and then adds a terminating '\0' character.  
 *
 * @param dest target for the concat operation
 * @param src string to be copied and converted
 *
 * @see strcat
 *
 * @return the complete and converted string
 */
char *strcat_tolower(char *dest, const char *src)
{
	char 	*ptr = dest + strlen(dest),
		*tmp;
	for (tmp = (char *)src; *tmp != '\0'; ++tmp, ++ptr)
		*ptr = tolower(*tmp);
	*ptr = '\0';
	return dest;
}

#ifdef __cplusplus
}
#endif

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
 * File          : asc_api.h
 *
 * Project       : ASCII based interface to device server API
 *
 * Description   : The interface file (header file) for the library
 *
 * Author(s)     : Faranguiss Poncet
 *		   $Author: jkrueger1 $
 *
 * Original      : November 1996
 *
 * Version       : $Revision: 1.3 $                               
 *
 * Date          : $Date: 2005-07-25 12:51:32 $
 *
 ****************************************************************************/


#ifndef _ascapi_h
#define _ascapi_h

#define ASC_HPUX_DEF_PATH      "libascapi.sl"
#define ASC_SOLARIS_DEF_PATH   "libascapi.so"


#define ASC_GET_FUNCTION       "dev_get_ascii"
#define ASC_PUT_FUNCTION       "dev_put_ascii"
#define ASC_PUTGET_FUNCTION    "dev_putget_ascii"
#define ASC_FREE_FUNCTION      "dev_free_ascii"




long dev_get_ascii(char *dev_name, char *cmd_name,
                                   char **out_arg, char **str_err);


long dev_put_ascii(char *dev_name, char *cmd_name, char *in_arg,
                                   char **str_err);



long dev_putget_ascii(char *dev_name, char *cmd_name, char *in_arg,
                                      char **out_arg, char **str_err);


long dev_free_ascii(char *dev_name, char **str_err);


#endif /* _ascapi_h */

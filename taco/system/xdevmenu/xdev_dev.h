/*****************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        xdev_dev.h
 *
 * Project:     Xdevmenu application (new generation made by BX)
 *
 * Description: The header file containing all functions and data types
 *              exported to the reste of the application by xdev_dev.c module.
 *
 * Author(s):   Builder Xcessory then completed by Faranguiss Poncet
 *              $Author: jkrueger1 $
 *
 * Original:    February 1997
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2007-03-22 13:59:23 $
 *
 */

#ifndef _xdev_dev_h
#define _xdev_dev_h




/*
 * Functions to export to outside of the module
 */


long dev_get_exported_devices( char *, char *, char *,
                                       char ***, unsigned int *, char **);

long dev_free_dev_names(char ***, char **);

long dev_init();

long dev_import_device(char *, int *, char **);

long dev_get_cmd_names( int , char ***, int *);

long dev_free_cmd_names(char ***, int );

long dev_get_cmd_profile(int ,int , char **, char **, char **, long *, long *, char **);

long dev_exec_cmd(int , int , char *, char **, char **, char **);

long dev_get_cmd_name_profile( int , char *, int *, char **, char **,
                                                    long *, long *, char **);

long dev_info_device( int  ind_dev, char **, char **, char **,
                                    char **, char **, char **);

long dev_close_device( int , char **);

long dev_change_timeout( int , long , char **);

long dev_get_timeout( int , long *);

long dev_change_rpc_protocol( int , char *, char **);

int dev_sig_interface_exist ( int );


#endif /* _xdev_dev_h */


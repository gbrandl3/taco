/****************************************************************************

 File          :  xdev_dev.h

 Project       :  Xdevmenu application (new generation made by BX)

 Description   :  The header file containing all functions and data types
                  exported to the reste of the application by xdev_dev.c module.

 Author        :  Builder Xcessory then completed by Faranguiss Poncet

 Original      :  February 1997

 $Revision: 1.1 $				$Author: jkrueger1 $
 $Date: 2003-04-25 12:54:28 $					$State: Exp $

 Copyright (c) 1997 by European Synchrotron Radiation Facility,
		       Grenoble, France

                       All Rights Reserved

****************************************************************************/


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


#endif /* _xdev_dev_h */


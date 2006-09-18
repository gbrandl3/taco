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
 * File:        xdev_gui.h
 *
 * Project:     Xdevmenu application (new generation made by BX)
 *
 * Description: The header file containing all functions and data types
 *              exported to the reste of the application by xdev_gui.c module.
 *
 * Author(s):   Builder Xcessory then completed by Faranguiss Poncet
 *              $Author: jkrueger1 $
 *
 * Original:    January 1997
 *
 * Version:     $Revision: 1.2 $
 *
 * Date:        $Date: 2006-09-18 21:43:56 $
 *
 *****************************************************************************/

#ifndef _xdev_gui_h
#define _xdev_gui_h


#include <Xm/Xm.h>
#include "xdev_const.h"


#define GUI_MAX_WIDGETS                50



/*Widget  gui_widget_array[GUI_MAX_WIDGETS];*/


/*
 * Functions to export to outside of the module
 */


long gui_init();

void gui_fill_dev_list(unsigned int, char  ** );

void gui_display_error(char *);

void gui_error_out(int, char *, char *, char *);

void gui_add_device(char  *, int );

void gui_select_device(int );

void gui_show_cmds(char **, int );

int gui_get_selected_dev();

void gui_display_arg_out(char  *, char  *, long , char  *, char  *);

void gui_display_argin_window(int , char *, long );

long gui_get_selected_dev_cmd(int *, int *);

void gui_remove_device(int );


#endif /* _xdev_gui_h */


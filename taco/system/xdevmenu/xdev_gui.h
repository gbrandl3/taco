/****************************************************************************

 File          :  xdev_gui.h

 Project       :  Xdevmenu application (new generation made by BX)

 Description   :  The header file containing all functions and data types
                  exported to the reste of the application by xdev_gui.c module.

 Author        :  Builder Xcessory then completed by Faranguiss Poncet

 Original      :  February 1997

 $Revision: 1.1 $				$Author: jkrueger1 $
 $Date: 2003-04-25 12:54:28 $					$State: Exp $

 Copyright (c) 1997 by European Synchrotron Radiation Facility,
		       Grenoble, France

                       All Rights Reserved

****************************************************************************/


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


/****************************************************************************

 File          :   asc_api.h

 Project       :   ASCII based interface to device server API

 Description   :   The interface file (header file) for the library

 Author        :   Faranguiss Poncet

 Original      :   November 1996

 $Revision: 1.1 $                               $Author: jkrueger1 $
 $Date: 2003-04-25 11:21:41 $                                     $State: Exp $

 Copyright (c) 1996 by European Synchrotron Radiation Facility,
                       Grenoble, France

                       All Rights Reserved

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

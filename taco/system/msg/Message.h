
/*********************************************************************

 File:		Message.h

 Project:	Device Servers with sun-rpc

 Description:	Include file for applications using 
                the Message-Server.

 Author(s):	Jens Meyer

 Original:	January 1991

 $Revision: 1.1 $
 $Date: 2003-04-25 12:54:27 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

		       All Rights Reserved

 *********************************************************************/

#ifndef _Message_h
#define _Message_h

extern void msg_alarm_handler();
extern void msg_initialise();
extern void msg_fault_handler();

typedef struct _MessageServerPart {
                                  char      name [256];
                                  char      host_name [32];
				  long	    prog_number;
                                  char      display [32];
                                  char	    ER_file_dir [256];
                                  char      pipe_dir [256];
                                  char      aw_path [256];
                                  } MessageServerPart;

#endif /* _Message_h */


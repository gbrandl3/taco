
/*********************************************************************

 File:		Message.h

 Project:	Device Servers with sun-rpc

 Description:	Include file for applications using 
                the Message-Server.

 Author(s):	Jens Meyer

 Original:	January 1991

 $Revision: 1.2 $
 $Date: 2003-05-09 15:26:45 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

		       All Rights Reserved

 *********************************************************************/

#ifndef _Message_h
#define _Message_h

extern void msg_alarm_handler(short, char *, char *, char *, char *);
extern void msg_initialise(char *);
extern void msg_fault_handler(DevString);

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


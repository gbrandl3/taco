
/*********************************************************************

 File:		Message.h

 Project:	Device Servers with sun-rpc

 Description:	Include file for applications using 
                the Message-Server.

 Author(s):	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.5 $

 Date:		$Date: 2005-02-24 15:55:38 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

		       All Rights Reserved

 *********************************************************************/

#ifndef _Message_h
#define _Message_h

extern void msg_alarm_handler(short, char *, char *, char *, char *);
extern void msg_initialise(char *);
extern void msg_fault_handler(DevString);

typedef struct _MessageServerPart 
{
	char	name [256];
	char	host_name [HOST_NAME_LENGTH];
	long	prog_number;
	char	display [HOST_NAME_LENGTH + 10]; // e.g. hostname.1234:1234
	char	ER_file_dir [256];
	char	pipe_dir [256];
	char	aw_path [256];
	char	logfile[256];
} MessageServerPart;

#endif /* _Message_h */


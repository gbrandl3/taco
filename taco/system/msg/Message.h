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
 * File:		Message.h
 *
 * Project:	Device Servers with sun-rpc
 *
 * Description:	Include file for applications using 
 *                the Message-Server.
 *
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:		$Date: 2006-09-18 21:46:20 $
 */

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


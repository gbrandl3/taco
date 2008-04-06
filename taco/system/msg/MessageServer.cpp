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
 * File:	MessageServer.c
 *
 * Project:	Device Servers with sun-rpc
 *
 * Description:	Source code for implementing a Message-Server 
 *
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	January 1991
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2008-04-06 09:07:52 $
 */

/*
 * Include files and Static Routine definitions
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <errno.h>
#include <API.h>
#include <private/ApiP.h>
#include <Message.h>
#include <DevErrors.h>

#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif
#if HAVE_FCNTL_H
#	include <fcntl.h>
#endif
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#else
#	error Could not find signal.h
#endif
#if TIME_WITH_SYS_TIME
# 	include <sys/time.h>
#	include <time.h>
#else
#	if HAVE_SYS_TIME_H
#		include <sys/time.h>
#	elif HAVE_TIME_H
#		include <time.h>
#	endif
#endif

extern MessageServerPart 	msg;
extern FILE		 	*logFile;

char *getTimeString(const char *name);

/***************************************
 * Message-Server initialise           *
 ***************************************/

void msg_initialise (const char *dshome)
{
/*
 * initialise the MessageServerPart
 */

#ifdef OSK
        snprintf (msg.ER_file_dir, sizeof(msg.ER_file_dir), "%s/api/error/", dshome );
        snprintf (msg.pipe_dir, sizeof(msg.pipe_dir), "%s/api/pipe/", dshome );
        snprintf (msg.aw_path, sizeof(msg.aw_path), "%s/api/bin/S_Alarm", dshome );
#endif /* OSK */

#ifdef unix
        snprintf (msg.ER_file_dir, sizeof(msg.ER_file_dir), "%s/system/error/", dshome );
        snprintf (msg.pipe_dir, sizeof(msg.pipe_dir), "%s/system/pipe/", dshome );
#endif /* unix */

#ifdef linux
        snprintf (msg.aw_path, sizeof(msg.aw_path), "%s/sbin/S_Alarm", dshome );
#endif

#ifdef __hp9000s300
        snprintf (msg.aw_path, sizeof(msg.aw_path), "%s/system/bin/s300/S_Alarm", dshome );
#endif /* __hp9000s300 */

#ifdef __hp9000s700
        snprintf (msg.aw_path, sizeof(msg.aw_path), "%s/system/bin/s700/S_Alarm", dshome );
#else
#  ifdef __hp9000s800
        snprintf (msg.aw_path, sizeof(msg.aw_path), "%s/system/bin/s800/S_Alarm", dshome );
#  endif /* __hp9000s800 */
#endif /* __hp9000s700 */

#ifdef sun
        snprintf (msg.aw_path, sizeof(msg.aw_path), "%s/system/bin/sun4/S_Alarm", dshome );
#endif /* sun */

  	fprintf (logFile, "%s Error message dir : %s\n", getTimeString("MessageServer"), msg.ER_file_dir);
  	fprintf (logFile, "%s Pipe message dir : %s\n", getTimeString("MessageServer"), msg.pipe_dir);
  	fprintf (logFile, "%s Alarm window program : %s\n", getTimeString("MessageServer"), msg.aw_path);
}


/*******************************************
 * Message-Server alarm handler method     *
 *******************************************/
void msg_alarm_handler(short alarm_type,
			const char *server_name,
			const char *host_name,
			const char *file_name,
			const char *display)
{
	char *cmd_argv [10];
	char buffer [3];
	int i;
	int pid = 0;

/*
 * Alarm handler starts an alarm window on the defined display.
 */
	if (( pid = fork () ) < 0 )
	{
  		fprintf (logFile, "%s Cannot start alarm window because fork failed!\n", getTimeString("MessageServer"));
		fprintf (logFile, "%s Error in %s on host %s !\n", getTimeString("MessageServer"), server_name, host_name);

		switch (alarm_type)
		{
			case 2 :
				fprintf (logFile,"%s cannot open error file :  %s\n", getTimeString("MessageServer"), file_name);
				break;
			case 1 :
			case 0 :
				fprintf (logFile,"%s error file :  %s\n", getTimeString("MessageServer"), file_name);
				break;
			case -1 :
				fprintf (logFile,"%s exiting!\n\n", getTimeString("MessageServer"));
				kill(getpid(), SIGQUIT);
		}
		return;
	}

	if (!pid)
	{
		snprintf (buffer, sizeof(buffer), "%d", alarm_type);
		i=0;
		cmd_argv[i++] = const_cast<char *>("S_Alarm"); 
		cmd_argv[i++] = const_cast<char *>(buffer); 
		cmd_argv[i++] = const_cast<char *>(server_name); 
		cmd_argv[i++] = const_cast<char *>(host_name); 
		cmd_argv[i++] = const_cast<char *>(file_name); 
		cmd_argv[i++] = const_cast<char *>("-display"); 
		cmd_argv[i++] = const_cast<char *>(display); 
		cmd_argv[i] = 0; 
		execv (msg.aw_path, cmd_argv);
		fprintf (logFile,"%s can not start Alarm Window !\n", getTimeString("MessageServer"));
		fflush(logFile);
		exit (-1);
	}
}


/*******************************************
 * Message-Server fault handler method     *
 *******************************************/
void msg_fault_handler (const DevString error_msg)
{
        FILE *filepointer;
        char file_name [256];
        char *time_string;
        time_t clock;
        
        snprintf (file_name, sizeof(file_name), "%s%s.ERR",msg.ER_file_dir, msg.name);

        time (&clock);
        time_string = ctime (&clock);

        if ( (filepointer = fopen (file_name,"a")) == NULL)
        {
        	msg_alarm_handler(2, msg.name, msg.host_name, file_name, msg.display);
		return;
        }
        fprintf (filepointer,"\n     %s%s",time_string, error_msg);
        fclose (filepointer);
        msg_alarm_handler(1, msg.name, msg.host_name, file_name, msg.display);
        return;
}

/*******************************************
 *					   *
 *        Message-Server routines          *
 *					   *
 *******************************************/

/*
 * sending messages to the right output device !
 *
 *       error messages      -> output file : hostname_prognumber.ERR
 *       debug messages      -> named pipe  : hostname_prognumber.DBG
 *       diagnostic messages -> named pipe  : hostname_prognumber.DIA
 */
_msg_out *rpc_msg_send_1 (_msg_data *msg_data)
{
	static _msg_out	msg_out;
        FILE 		*filepointer;
        mode_t 		mode;
        char 		pipe_name [256];
        char 		file_name [256];
        char 		error_msg [256];
        char 		acm [256];
        char 		*char_ptr;
        char 		*time_string;
        time_t 		clock;
        int 		fildes;
        int 		nbytes;
	int 		j;
        int 		iret = 0;

	msg_out.status = 0;
	msg_out.error = 0;

	nbytes = strlen (msg_data->message_buffer);
	char_ptr = msg_data->message_buffer;

/*
 *  control os9 messages for CR characters
 */
	for ( j=0; j<nbytes; j++ , char_ptr++)
        	if ( *char_ptr == 0x0D )
			*char_ptr = 0x0A;

/*
 *  send message to special output
 */
	switch ( msg_data->type_of_message )
	{
/*
 * write error messages to a named error file !
 */
		case ERROR_TYPE:
			snprintf (file_name, sizeof(file_name), "%s%s_%d.ERR",msg.ER_file_dir,
                                            msg_data->host_name,
					    msg_data->prog_number);

			time (&clock);
			time_string = ctime (&clock);

			if ((filepointer = fopen(file_name,"a")) == NULL)
			{
				snprintf (error_msg, sizeof(error_msg), "rpc_msg_send :\ncannot open error file > %s\n", file_name);
				msg_fault_handler (error_msg);
		
				msg_out.error = DevErr_CannotOpenErrorFile;
				msg_out.status = -1;
				return (&msg_out);
			}
		
			fprintf(filepointer,"\n     %s%s",time_string, msg_data->message_buffer);
			fclose(filepointer);
			chmod(file_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
/*
 * open alarm window
 */
			msg_alarm_handler(0 ,msg_data->devserver_name, msg_data->host_name,file_name, msg_data->display);
			return (&msg_out);
			break;

/*
 * get pipe name for debug or diagnostic messages !
 */
            	case DEBUG_TYPE:
			snprintf(pipe_name, sizeof(pipe_name), "%s%s_%d.DBG",  msg.pipe_dir,
                                               msg_data->host_name,
					       msg_data->prog_number);
			break;

		case DIAG_TYPE:
			snprintf(pipe_name, sizeof(pipe_name), "%s%s_%d.DIA",  msg.pipe_dir,
                                               msg_data->host_name,
					       msg_data->prog_number);
			break;
	}

/*
 * write debug or diagnostic messages to named pipe !
 */
	fildes = open (pipe_name, O_APPEND | O_WRONLY);
	if (fildes == -1)
		fildes = open(pipe_name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fildes == -1)
	{
  		fprintf (logFile, "%s Pipe name : %s, %d (%s)\n", getTimeString("MessageServer"), pipe_name, errno, strerror(errno));
		msg_out.error = DevErr_CannotOpenPipe;
		msg_out.status = -1;
		return (&msg_out);
	}

	iret = write(fildes,msg_data->message_buffer,nbytes);
       	if ( iret == -1 || iret == 0)
       	{
		msg_out.error = DevErr_CannotWriteToPipe;
		msg_out.status = -1;
		return (&msg_out);
       	}
	close (fildes);
	return (&msg_out);
}


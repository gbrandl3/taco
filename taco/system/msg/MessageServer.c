/*********************************************************************

 File:		MessageServer.c

 Project:	Device Servers with sun-rpc

 Description:	Source code for implementing a Message-Server 

 Author(s):	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.7 $

 Date:		$Date: 2005-02-24 15:55:38 $

 Copyright (c) 1990 by	European Synchrotron Radiation Facility, 
			Grenoble, France

			All Rights Reserved

 *********************************************************************/

/*
 * Include files and Static Routine definitions
 */
#include "config.h"
#include <API.h>
#include <private/ApiP.h>
#include <Message.h>
#include <DevErrors.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if HAVE_SIGNAL_H
#       include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#       include <sys/signal.h>
#else
#error Could not find signal.h
#endif
#if TIME_WITH_SYS_TIME
# 	include <sys/time.h>
#	include <time.h>
#else
#	if HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else
#		include <time.h>
#	endif
#endif

extern MessageServerPart 	msg;
extern FILE		 	*logFile;
char *getTimeString(const char *name);

/***************************************
 * Message-Server initialise           *
 ***************************************/

void msg_initialise (char *dshome)
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

}


/*******************************************
 * Message-Server alarm handler method     *
 *******************************************/
void msg_alarm_handler(short alarm_type,
			char *server_name,
			char *host_name,
			char *file_name,
			char *display)
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
  		fprintf (logFile, "\n%s Cannot start alarm window because fork failed!", getTimeString("MessageServer"));
		fprintf (logFile, "\n\nError in %s on host %s !\n",server_name,host_name);

		switch (alarm_type)
		{
			case 2 :
				fprintf (logFile,"%s cannot open error file :  %s\n",file_name, getTimeString("MessageServer"));
				break;
			case 1 :
			case 0 :
				fprintf (logFile,"%s error file :  %s\n", getTimeString("MessageServer"), file_name);
				break;
			case -1 :
				fprintf (logFile,"%s exiting!\n", getTimeString("MessageServer"));
				kill(getpid(), SIGQUIT);
		}
		return;
	}

	if (!pid)
	{
		snprintf (buffer, sizeof(buffer), "%d", alarm_type);
		i=0;
		cmd_argv[i++] = "S_Alarm"; 
		cmd_argv[i++] = buffer; 
		cmd_argv[i++] = server_name; 
		cmd_argv[i++] = host_name; 
		cmd_argv[i++] = file_name; 
		cmd_argv[i++] = "-display"; 
		cmd_argv[i++] = display; 
		cmd_argv[i] = 0; 
		execv (msg.aw_path, cmd_argv);
		fprintf (logFile,"\n%s can not start Alarm Window !\n", getTimeString("MessageServer"));
		fflush(logFile);
		exit (-1);
	}
}


/*******************************************
 * Message-Server fault handler method     *
 *******************************************/
void msg_fault_handler (DevString error_msg)
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

			if ( (filepointer = fopen (file_name,"a")) == NULL)
			{
				snprintf (error_msg, sizeof(error_msg), "rpc_msg_send :\ncannot open error file > %s\n", file_name);
				msg_fault_handler (error_msg);
		
				msg_out.error = DevErr_CannotOpenErrorFile;
				msg_out.status = -1;
				return (&msg_out);
			}
		
			fprintf (filepointer,"\n     %s%s",time_string, msg_data->message_buffer);
			fclose (filepointer);
			chmod(file_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
/*
 * open alarm window
 */
			msg_alarm_handler (0,msg_data->devserver_name, msg_data->host_name,file_name, msg_data->display);
			return (&msg_out);
			break;

/*
 * get pipe name for debug or diagnostic messages !
 */
            	case DEBUG_TYPE:
			snprintf (pipe_name, sizeof(pipe_name), "%s%s_%d.DBG",  msg.pipe_dir,
                                               msg_data->host_name,
					       msg_data->prog_number);
			break;

		case DIAG_TYPE:
			snprintf (pipe_name, sizeof(pipe_name), "%s%s_%d.DIA",  msg.pipe_dir,
                                               msg_data->host_name,
					       msg_data->prog_number);
			break;
	}

/*
 * write debug or diagnostic messages to named pipe !
 */
	fildes = open (pipe_name,O_WRONLY);
	if (fildes == -1)
	{
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


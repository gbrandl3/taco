/*********************************************************************

 File:		MessageServer.c

 Project:	Device Servers with sun-rpc

 Description:	Source code for implementing a Message-Server 

 Author(s):	Jens Meyer
 		$Author: jkrueger1 $

 Original:	January 1991

 Version:	$Revision: 1.4 $

 Date:		$Date: 2004-02-03 09:27:03 $

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

extern MessageServerPart msg;



/***************************************
 * Message-Server initialise           *
 ***************************************/

void msg_initialise (char *dshome)
{
/*
 * initialise the MessageServerPart
 */

#ifdef OSK
        snprintf (msg.ER_file_dir, sizeof(msg.ER_file_dir) - 1, "%s/api/error/", dshome );
        snprintf (msg.pipe_dir, sizeof(msg.pipe_dir) - 1, "%s/api/pipe/", dshome );
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/api/bin/S_Alarm", dshome );
#endif /* OSK */

#ifdef unix
        snprintf (msg.ER_file_dir, sizeof(msg.ER_file_dir) - 1, "%s/system/error/", dshome );
        snprintf (msg.pipe_dir, sizeof(msg.pipe_dir) - 1, "%s/system/pipe/", dshome );
#endif /* unix */

#ifdef linux
#	ifdef x86
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/system/bin/linux/x86/S_Alarm", dshome );
#	endif
#	ifdef 68k
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/system/bin/linux/68k/S_Alarm", dshome );
#	endif
#endif

#ifdef __hp9000s300
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/system/bin/s300/S_Alarm", dshome );
#endif /* __hp9000s300 */

#ifdef __hp9000s700
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/system/bin/s700/S_Alarm", dshome );
#else
#  ifdef __hp9000s800
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/system/bin/s800/S_Alarm", dshome );
#  endif /* __hp9000s800 */
#endif /* __hp9000s700 */

#ifdef sun
        snprintf (msg.aw_path, sizeof(msg.aw_path) - 1, "%s/system/bin/sun4/S_Alarm", dshome );
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
  		fprintf (stderr, "\nCannot start alarm window because fork failed!!");
		fprintf (stderr, "\n\nError in %s on host %s !!",server_name,host_name);

		switch (alarm_type)
		{
			case 2 :
				fprintf (stderr,"\ncannot open error file :  %s\n",file_name);
				break;
			case 1 :
			case 0 :
				fprintf (stderr,"\nerror file :  %s\n",file_name);
				break;
			case -1 :
				fprintf (stderr,"\nMessage Server exiting !!\n");
				exit (-1);
		}
		return;
	}

	if (!pid)
	{
		snprintf (buffer, sizeof(buffer) - 1, "%d", alarm_type);
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
		fprintf (stderr,"\nMessage Server can not start Alarm Window !!\n");
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
        
        snprintf (file_name, sizeof(file_name) - 1, "%s%s.ERR",msg.ER_file_dir, msg.name);

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
 *
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
			snprintf (file_name, sizeof(file_name) - 1, "%s%s_%d.ERR",msg.ER_file_dir,
                                            msg_data->host_name,
					    msg_data->prog_number);

			time (&clock);
			time_string = ctime (&clock);

			if ( (filepointer = fopen (file_name,"a")) == NULL)
			{
				snprintf (error_msg, sizeof(error_msg) - 1, "rpc_msg_send :\ncannot open error file > %s\n", file_name);
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
			snprintf (pipe_name, sizeof(pipe_name) - 1, "%s%s_%d.DBG",  msg.pipe_dir,
                                               msg_data->host_name,
					       msg_data->prog_number);
			break;

		case DIAG_TYPE:
			snprintf (pipe_name, sizeof(pipe_name) - 1, "%s%s_%d.DIA",  msg.pipe_dir,
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


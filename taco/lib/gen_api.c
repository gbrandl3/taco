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
 * File       :	gen_api.c
 * 
 * Project    :	Device Servers
 *
 * Description:	General Application Programmers Interface to the device
 *		servers and clients using the SUN-RPC.
 *
 * Author(s)  :	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original   :	January 1991
 *
 * Version    :	$Revision: 1.28 $
 *
 * Date       : $Date: 2006-09-18 22:31:44 $
 *
 ********************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif 
#include <API.h>
#include <private/ApiP.h>
#include <API_xdr_vers3.h>
#include <stdlib.h>

#include <Admin.h>
#include <DevErrors.h>
#include <DevSignal.h>
#ifdef vxworks
#	include <taskLib.h>
#endif /* vxworks */

/*
 * Include file for variable function arguments.
 */
#if HAVE_STDARG_H
#	include <stdarg.h>
#elif HAVE_VARARGS_H
#	include <varargs.h>
#endif

#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error could not find signal.h
#endif

extern _DLLFunc long 	setup_config_multi(char *nethost, long *error);
static void 		msg_write(_Int msg_type, char *msg_string);
static void 		msg_send (_Int msg_type);
static void 		msg_clear(_Int msg_type);
static char 		*get_error_string(long error);

/*
 * UCC has not prototype for putenv() , define one ! 
 *
 * - andy 22nov96
 */
#ifdef _OSK
long putenv(char *);
#endif /* _OSK */

char *dev_error_string=NULL; /* global pointer to dynamic error string */
char *dev_error_stack=NULL; /* global pointer to dynamic error stack */

/****************************************
 *          Globals	                *
 ****************************************/

/* for Windows C++ these symbols must be seen by libdbapi
 * therefore declare them as extern "C" - andy 24/4/02
 */

#ifdef __cplusplus
extern "C" {
#endif
/*
 *  Types for global state flags for the current
 *  server or client status and for Message Server and Database
 *  Server information are defined in API.h
 */

/*
 *  Configuration flags - modified by ag 16sep94
 *
 *  configuration 	== false
 *  message_server 	== false
 *  database_server	== false
 *  startup		== false
 *  security		== false
 */


configuration_flags	config_flags =
{
	False,False,False,False,False, "", "", 0L
};


/*
 *  Database Server globals
 */

dbserver_info		db_info;

/*
 *  Message Server globals
 */

msgserver_info		msg_info;
static _message_buffer 	message_buffer [NUMBER_OF_MSG_TYPES];


struct _devserver 	*msg_ds, 
			*db_ds;
/*
 * multi-nethost support requires an array of nethost_info structures
 * to keep track of the multple nethosts and their databases and
 * message servers. The following structures is used for this.
 * The first element (nethost=0) is the default nethost, a copy 
 * of the (old) globals config_flags, db_info and msg_info.
 * Additional nethosts are stored starting from element 1 on.
 *
 * The maximum number of nethosts starts off at MIN_NETHOST.
 * Space is allocated as needed for more nethosts in slices of
 * MIN_NETHOST (10)
 *
 */

nethost_info 	*multi_nethost;
long max_nethost = 0;

#ifdef WIN32
static long win32_rpc_inited;
#endif /* WIN32 */

/*
 *  Debug globals
 */

long debug_flag = 0x0;

#ifdef __cplusplus
}
#endif

/**
 * @defgroup messageAPI Message API
 * @ingroup dsAPI
 * Functions to send some messages to the user or administrator.
 */
/**
 * @defgroup messageAPIintern Internal functions
 * @ingroup messageAPI
 * These function will be used internal by the message API.
 */


/**@ingroup messageAPI
 * This function imports a message service.  The parameters are used to name the
 * approriate error file and to display a correct error window. 
 *
 * @param DS_name 	name of the device server
 * @param DS_host       host of the device server
 * @param DS_prog_number program number of the device server
 * @param DS_display    display to use
 * @param error         Will contain an appropriate error
 *			code if the corresponding call
 *		    	returns a non-zero value.
 *
 * @return	DS_OK or DS_NOTOK
 */
long _DLLFunc msg_import (char *DS_name, char *DS_host, long DS_prog_number, char *DS_display, long *error)
{
	CLIENT		*clnt;
	char		*hstring;

	*error = DS_OK;

	/*
	 * check wether a message server is already imported
	 */

	if (config_flags.message_server)
		return (DS_OK);

	/*
	 * check wether the system is already configured
	 */

	if ( !config_flags.configuration )
	{
		if ( (setup_config_multi(NULL,error)) < 0 )
			return (DS_NOTOK);

	}


	/*
	 * Create message server client handle with data from
	 * global msgserver_info structure.
	 */

	clnt = clnt_create ( msg_info.conf->server_host,
	    msg_info.conf->prog_number,
	    msg_info.conf->vers_number,
	    "udp");
	if (clnt == NULL)
	{
		hstring = clnt_spcreateerror ("msg_import");
		dev_printerror (SEND,"%",hstring);
		*error = DevErr_MsgImportFailed;
		return (DS_NOTOK);
	}

	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &msg_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &msg_timeout);


	/* 
 	 * pass the information to the msgserver_info structure
 	 */

	strncpy (msg_info.conf->device_name, "MessageServer", sizeof(msg_info.conf->device_name));
        msg_info.conf->device_name[sizeof(msg_info.conf->device_name) - 1] = '\0';
	msg_info.conf->clnt = clnt;
	msg_info.conf->ds_id = 0;
	msg_info.conf->no_svr_conn = 1;

	snprintf (msg_info.DS_name, sizeof(msg_info.DS_name), "%s", DS_name);
	snprintf (msg_info.DS_host, sizeof(msg_info.DS_host), "%s", DS_host);
	msg_info.DS_prog_number         =   DS_prog_number;
	snprintf (msg_info.DS_display, sizeof(msg_info.DS_display), "%s", DS_display);

	config_flags.message_server = True;

	return (DS_OK);
}



/*
 * Attention: OS9 version 2.4 can not handle variable argument
 * lists. It will still use the old tricky function!!
 */
#if (!defined OSK) && (!defined _GNU_PP)
/**@ingroup messageAPI
 * If a message service is imported, all error messages are sent to an error file, on the NETHOST, called:
 * @verbatim
	NETHOST:/DSHOME/api/error/hostname_program-number
   @endverbatim
 * where 
 * 	- NETHOST the device server system host
 * 	- DSHOME the device server system directory on NETHOST
 * 	- li hostname the name of the host where the server is running.
 * 	- prog_number the program number of the registered service.
 * If no message service is imported, all error messages are sent to @c stderr and printed on the terminal.
 *
 * The mode parameter indicates, how to handle the error message buffer. Single messages can only be 256
 * characters long. To printout longer messages, short strings can be buffered and printed later as a text.
 * @li @b WRITE: writes error message to buffer.
 * @li @b SEND: adds the last error message to the buffer, sends the buffer contents to an output devices and clears the buffer.
 * @li @b CLEAR: clears the message buffer from all stored messages.
 *
 * @param mode  mode for message buffer : WRITE or SEND
 * @param fmt   format string in printf() style.
 * @param ...   variable argument list of variables to print
 */
_DLLFunc void dev_printerror (DevShort mode, char *fmt, ...)
{
	char		buffer [256];
	va_list 	args;

	if ( mode == CLEAR )
	{
		msg_clear ( ERROR_TYPE );
		return;
	}

/*
 * get variable argument list pointer in order to pass it to vsnprintf()
 */
#if HAVE_VA_START_P1
	va_start(args);
#elif HAVE_VA_START_P1_P2
	va_start(args, fmt);
#elif WIN32
	va_start(args, fmt);
#endif /* sun */

	vsnprintf (buffer, sizeof(buffer), fmt, args);
	strcat (buffer,"\n");

	va_end(args);

	msg_write ( ERROR_TYPE, buffer );

	if ( mode != WRITE )
	{
		/*
            * send messages to message server if imported
            */

		if ( config_flags.message_server )
			msg_send ( ERROR_TYPE );
		else
		{
#ifdef WIN32
			char msg [1024];
			snprintf ( msg, sizeof(msg), "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
			PRINTF(msg); //conout(msg);
#else
			fprintf (stderr, "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
#endif
			msg_clear ( ERROR_TYPE );
		}
	}
}

/*
 * For OS9 version 2.4 and Solaris with g++
 */
#else
/**@ingroup messageAPI
 * Send device server error_string to a message 
 * service or to stdout, if a message service
 * is not imported.
 * Error strings can be only 256 characters long.
 * Longer texts can be send by storing short
 * strings in the message buffer with the WRITE
 * mode and send the whole buffer by using
 * the SEND mode with the last string.
 *
 * a variable list of arguments which should contain 
 *
 * @param mode  mode for message buffer : WRITE or SEND
 * @param fmt   format string in printf() style.
 * @param str   variable argument list of variables to print
 */
_DLLFunc void dev_printerror (DevShort mode,char *fmt, char *str)
{
	char  		buffer [256];

	if ( mode == CLEAR )
	{
		msg_clear ( ERROR_TYPE );
		return;
	}

	if (str != NULL)
		snprintf (buffer, sizeof(buffer), fmt, str);
	else
		snprintf (buffer, sizeof(buffer), fmt);
	if (strlen(buffer) < sizeof(buffer) - 2)
		strcat (buffer,"\n");
	else
		strcat (&buffer[sizeof(buffer) - 2], "\n")

	msg_write ( ERROR_TYPE, buffer );

	if ( mode != WRITE )
	{
/*
 * send messages to message server if imported
 */
		if ( config_flags.message_server )
			msg_send ( ERROR_TYPE );
		else
		{
#ifdef WIN32
			char msg[1024];
			snprintf ( msg, sizeof(msg), "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
			PRINTF(msg); //conout(msrg);
#else
			fprintf (stderr, "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
#endif
			msg_clear ( ERROR_TYPE );
		}
	}
}
#endif /* OSK */

/**@ingroup messageAPI
 * If a message service is imported, all error messages are sent to an error file, on the NETHOST, called:
 * @verbatim
	NETHOST:/DSHOME/api/error/hostname_program-number
   @endverbatim
 * where 
 * 	- NETHOST the device server system host
 * 	- DSHOME  the device server system directory on NETHOST
 * 	- hostname the name of the host where the server is running.
 * 	- prog_number the program number of the registered service.
 *
 * If no message service is imported, all error messages are sent to @c stderr and printed on the terminal.
 *
 * The mode parameter indicates, how to handle the error message buffer. Single messages can only be 256
 * characters long. To printout longer messages, short strings can be buffered and printed later as a text.
 * @li @b WRITE: writes error message to buffer.
 * @li @b SEND: adds the last error message to the buffer, sends the buffer contents to an output devices and clears the buffer.
 * @li @b CLEAR: clears the message buffer from all stored messages.
 *
 * @param mode  mode for message buffer : WRITE or SEND
 * retrieves the related error string for an error number
 * from the list in DevErrors.h or the rsource database
 * and sends the error string with the choosen mode 
 * to dev_printerror().
 * 
 * @param mode     	mode for message buffer : WRITE or SEND
 * @param comment  	comment to be printed before error string.
 * @param dev_errno 	device server system error number.
 */ 
void _DLLFunc dev_printerror_no (DevShort mode, char *comment, long dev_errno)
{
	char *error_string = get_error_string ( dev_errno);
/*
 * get error string from the resource database
 */
	if (comment != NULL)
		dev_printerror (mode, "%s", comment);
	dev_printerror (mode, "%s", error_string);
	if (error_string != NULL) 
		free(error_string);
}


/**@ingroup dsAPIintern
 * This function returns the error string for a given error number. It first checks to see
 * if the error is negative. If so it returns a standard error message "negative errors are not supported".
 * Then it checks if the error is one of the kernel errors (e.g. NETHOST not defined, RPC timeout etc) and
 * returns a corresponding error message. Then it checks to see if a dynamic error message was returned by
 * the last @ref dev_putget(), @ref dev_put(), or @ref dev_putget_asyn() call, if so it returns this error
 * message. If none of the above are trus it searches the TACO database for the (static) error string. If an
 * appropriate error string cannot be found in the database, this function returns a string, indicating the
 * failure. <b>This function allocates memory for the return error string everytime using malloc(3C), it is
 * the client's responsiblity to free this memory using free(3C)</b>
 * \note This is common source of memory leaks in TACO clients.
 * 
 * @param dev_errno device server system error number.
 * 
 * @return error string, related to dev_errno.
 */ 
_DLLFunc char * dev_error_str (long dev_errno)
{
	return get_error_string (dev_errno);
}

/*
 * Attention: OS9 version 2.4 can not handle variable argument
 * lists. It will still use the old tricky function!!
 */
#if (!defined OSK) && (!defined _GNU_PP)
/**@ingroup messageAPI
 * This function sends the debug information if the specified debug_bits are set.
 * 
 * Possible debug_bits (debug flags) are:
 * @li @b DBG_TRACE
 * @li @b DBG_ERROR
 * @li @b DBG_INTERRUPT
 * @li @b DBG_TIME
 * @li @b DBG_WAIT
 * @li @b DBG_EXCEPT
 * @li @b DBG_SYNC
 * @li @b DBG_HARDWARE
 * @li @b DBG_STARTUP
 * @li @b DBG_DEV_SVR_CLASS
 * @li @b DBG_API
 * @li @b DBG_COMMANDS
 * @li @b DBG_METHODS
 * @li @b DBG_SEC
 * @li @b DBG_ASYNCH
 * 
 * If a message service is imported, all error messages are sent to an error file, on the NETHOST, called:
 * @verbatim
	NETHOST:/DSHOME/api/error/hostname_program-number
   @endverbatim
 * where 
 * @li NETHOST - the device server system host
 * @li DSHOME  - the device server system directory on NETHOST
 * @li hostname - the name of the host where the server is running.
 * @li prog_number - the program number of the registered service.
 * If no message service is imported, all error messages are sent to @c stderr and printed on the terminal.
 *
 * Debug strings can be only 256 characters long.
 *
 * @param debug_bits  	debug bits on which to send the information.
 * @param fmt        	format string in printf() style.
 * @param ... 		reserving memory for variables to print.
 */ 
void _DLLFunc dev_printdebug (long debug_bits, char *fmt, ...)
{
	char		debug_string[256];
	va_list	args;
/*
 * get variable argument list pointer in order to pass it to vsnprintf()
 */
#if HAVE_VA_START_P1
	va_start(args);
#elif HAVE_VA_START_P1_P2
	va_start(args, fmt);
#elif WIN32
	va_start(args, fmt);
#endif /* sun */

/*
 *  is debuging switched on ?
 */

	if ( debug_flag & DEBUG_ON_OFF )
	{
/*
 *  are the right debug bits set ?
 */
		if ( (debug_flag & debug_bits) != 0 )
		{
			vsnprintf (debug_string, sizeof(debug_string), fmt,args);
/*
 *  is the message server imported ?
 */
			if ( config_flags.message_server )
			{
				msg_write ( DEBUG_TYPE, debug_string );
				msg_send ( DEBUG_TYPE );
			}
			else
			{
#ifdef WIN32
				PRINTF(debug_string); //conout( debug_string );
#else
				printf ( debug_string );
#endif
				msg_clear ( DEBUG_TYPE );
			}
		}
	}

	va_end(args);
}

/*
 * For OS9 version 2.4 and Solaris with g++
 */
#else
/**@ingroup messageAPI
 * Send debug string to a message service or to stdout, if a message service
 * is not imported.
 *
 * A debug string will be send if one of the debug_bits
 * related to the string is set in the global debug_flag.
 *
 * Debug strings can be only 256 characters long.
 *
 * @param debug_bits  	debug bits on which to send the information.
 * @param fmt        	format string in printf() style.
 * @param str		reserving memory for variables to print.
 */
void dev_printdebug (long debug_bits,char *fmt, char *str)
{
	char 	debug_string[256];

/*
 *  is debuging switched on ?
 */

	if ( debug_flag & DEBUG_ON_OFF )
	{
/*
 *  are the right debug bits set ?
 */
		if ( (debug_flag & debug_bits) != NULL )
		{
			if (str != NULL)
				snprintf (debug_string, sizeof(debug_string), fmt,str);
			else
				snprintf (debug_string, sizeof(debug_string), fmt);
/*
 *  is the message server imported ?
 */
			if ( config_flags.message_server )
			{
				msg_write ( DEBUG_TYPE, debug_string );
				msg_send ( DEBUG_TYPE );
			}
			else
			{
#ifdef WIN32
				PRINTF(debug_string); //conout ( debug_string );
#else
				printf ( debug_string );
#endif
				msg_clear ( DEBUG_TYPE );
			}
		}
	}
}
#endif /* OSK */

/*
 * Attention: OS9 version 2.4 can not handle variable argument
 * lists. It will still use the old tricky function!!
 */
#if (!defined OSK) && (!defined _GNU_PP)
/**@ingroup messageAPI
 * Send device server diagnostic_string to a message 
 * service or to stdout, if a message service
 * is not imported.
 *
 * Diagnostic strings can be only 256 characters long.
 * Longer texts can be send by storing short
 * strings in the message buffer with the WRITE
 * mode and send the whole buffer by using
 * the SEND mode with the last string.
 *
 * @param mode mode for message buffer : WRITE or SEND
 * @param fmt  format string in printf() style.
 * @param ...  reserving memory for variables to print.
 */
_DLLFunc void dev_printdiag (DevShort mode, char *fmt, ...)
{
	char  		buffer [256];
	va_list		args;

	if ( mode == CLEAR )
	{
		msg_clear ( DIAG_TYPE );
		return;
	}
/*
 * get variable argument list pointer in order to pass it to vsnprintf()
 */
#if HAVE_VA_START_P1
	va_start(args);
#elif HAVE_VA_START_P1_P2
	va_start(args, fmt);
#elif WIN32
	va_start(args, fmt);
#endif /* sun */

/*
 * vsnprintf() gives a core dump at present (17sep94) replace it with
 * a simple snprintf() and ignore the variable arguments for the moment
 *
 	snprintf (buffer, sizeof(buffer), fmt);
 */
	vsnprintf (buffer, sizeof(buffer), fmt, args);
	va_end(args);
	msg_write ( DIAG_TYPE, buffer );

	if ( mode != WRITE )
	{
/*
 * send messages to message server if imported
 */
		if ( config_flags.message_server )
			msg_send ( DIAG_TYPE );
		else
		{
#ifdef WIN32
			char msg[1024];
			snprintf (msg, sizeof(msg), "%s", message_buffer[DIAG_TYPE].messages);
			PRINTF(msg); //conout(msg);
#else
			printf ("%s", message_buffer[DIAG_TYPE].messages);
#endif
			msg_clear ( DIAG_TYPE );
		}
	}
}

/*
 * For OS9 version 2.4 and Solaris with g++
 *
 * NOTE : these platforms have difficulty with the variable arguments 
 *        therefore only a reduced form of dev_printdiag() is supported
 *        i.e. with only a single argument to print (char *fmt)
 *
 *        andy 5aug99
 */
#else

/**@ingroup messageAPI
 * Send device server diagnostic_string to a message 
 * service or to stdout, if a message service
 * is not imported.
 *
 * Diagnostic strings can be only 256 characters long.
 * Longer texts can be send by storing short
 * strings in the message buffer with the WRITE
 * mode and send the whole buffer by using
 * the SEND mode with the last string.
 *
 * @param mode mode for message buffer : WRITE or SEND
 * @param fmt  format string in printf() style.
 * @param str  reserving memory for variables to print.
 */
void dev_printdiag (DevShort mode,char *fmt,char *str)
{
	char	buffer [256];

	if ( mode == CLEAR )
	{
		msg_clear ( DIAG_TYPE );
		return;
	}

	if (str != NULL)
		snprintf (buffer, sizeof(buffer), fmt,str);
	else
		snprintf (buffer, sizeof(buffer), fmt);

	msg_write ( DIAG_TYPE, buffer );

	if ( mode != WRITE )
	{
/*
 * send messages to message server if imported
 */
		if ( config_flags.message_server )
			msg_send ( DIAG_TYPE );
		else
		{
#ifdef WIN32
			char msg[1024];
			snprintf (msg, sizeof(msg), "%s", message_buffer[DIAG_TYPE].messages);
			PRINTF(msg); //conout(msg);
#else
			printf ("%s", message_buffer[DIAG_TYPE].messages);
#endif
			msg_clear ( DIAG_TYPE );
		}
	}
}
#endif /* OSK */


/**@ingroup messageAPIintern
 * This function writes a messages into the message buffer  
 *
 * @param msg_type   type of message: error or debug
 * @param msg_string message string
 */
static void msg_write (_Int msg_type, char *msg_string)
{
	char 		*help,
			*time_string;
	int  		len;
	int   		i;
	int		pid = 0;
	static short 	init_flg = 0;
	time_t		clock;

	time (&clock);
	time_string = ctime (&clock);
	time_string[strlen(time_string) - 1] = ':';
/*
 * initialise all message buffer
 */
	if ( init_flg == 0 )
	{
		for ( i=0; i<NUMBER_OF_MSG_TYPES; i++ )
		{
			message_buffer[i].init_flg = 0;
			message_buffer[i].nbytes = 0;
			message_buffer[i].messages = NULL;
		}
		init_flg = 1;
	}

/*
 * allocate space for the first message
 */
	if ( message_buffer[msg_type].init_flg == 0 )
	{
		len = strlen(time_string) + strlen(msg_string) + 1;
		if ( (message_buffer[msg_type].messages = (char *)malloc (len)) == NULL )
		{
#ifdef WIN32
			PRINTF("msg_write() : Insufficient memory for allocation !");
#else
			printf ("msg_write() : Insufficient memory for allocation !");
#endif
#if !defined (WIN32)
#if !defined (vxworks)
			pid = getpid ();
#else  /* !vxworks */
			pid = taskIdSelf ();
#endif /* !vxworks */
			kill (pid,SIGQUIT);
#else
			raise(SIGTERM);
#endif	/* WIN32 */
		}

		message_buffer[msg_type].nbytes = len;
		strcpy(message_buffer[msg_type].messages, time_string);
		strcat(message_buffer[msg_type].messages , msg_string);
		message_buffer[msg_type].init_flg = 1;
	}

	else
/*
 * reallocate space for further messages
 */
	{
		len = message_buffer[msg_type].nbytes + strlen (msg_string) + strlen(time_string);
		help = message_buffer[msg_type].messages;

		if ((help=(char *)realloc(message_buffer[msg_type].messages,len)) == NULL)
		{
#ifdef WIN32
			{
				char msg[1024];
				PRINTF("msg_write() : Insufficient memory for reallocation !");
				snprintf(msg, sizeof(msg), "message_buffer contents :\n%s",
				    message_buffer[msg_type].messages );
				PRINTF(msg);
			}
#else
			printf("msg_write() : Insufficient memory for reallocation !");
			printf ("message_buffer contents :\n%s",
			    message_buffer[msg_type].messages );
#endif
#if !defined (WIN32)
#if !defined (vxworks)
			pid = getpid ();
#else  /* !vxworks */
			pid = taskIdSelf ();
#endif /* !vxworks */
			kill (pid,SIGQUIT);
#else
			raise(SIGTERM);
#endif /* WIN32 */
		}

		message_buffer[msg_type].nbytes = len;
		message_buffer[msg_type].messages = help;
		strcat ( message_buffer[msg_type].messages , time_string);
		strcat ( message_buffer[msg_type].messages , msg_string);
	}
}


/**@ingroup messageAPIintern
 * This function sends the contents of the message buffer to the message server or stdout, if no
 * message service is imported.
 *
 * @param msg_type     type of message: error or debug
 */ 
static void msg_send (_Int msg_type)
{
	_msg_data 		msg_data;
	_msg_out		msg_out;
	enum clnt_stat	clnt_stat;
	int			pid = 0;


	if ( message_buffer[msg_type].init_flg == 1 )
	{
/*
 * initialise message structur
 */
		msg_data.devserver_name  = msg_info.DS_name;
		msg_data.host_name       = msg_info.DS_host;
		msg_data.prog_number     = msg_info.DS_prog_number;
		msg_data.display         = msg_info.DS_display;
		msg_data.type_of_message = msg_type;
		msg_data.message_buffer  = message_buffer[msg_type].messages;

/*
 * send message structur to message server
 */
		clnt_stat = clnt_call (msg_info.conf->clnt,RPC_MSG_SEND,
		    (xdrproc_t)xdr__msg_data, (caddr_t) &msg_data,
		    (xdrproc_t)xdr__msg_out, (caddr_t) &msg_out, TIMEVAL(timeout));
		if (clnt_stat != RPC_SUCCESS)
		{
#ifdef WIN32
			{
				char msg[1024];
				PRINTF(clnt_sperror (msg_info.conf->clnt,"msg_send()"));
				snprintf(msg, sizeof(msg), "message_buffer contents :\n%s",
				    message_buffer[msg_type].messages );
				PRINTF(msg);
			}
#else
			clnt_perror (msg_info.conf->clnt,"msg_send()");
			printf ("message_buffer contents :\n%s",
			    message_buffer[msg_type].messages );
#endif
/*
 *  exit server if message server connection fails
 */

#if !defined (WIN32)
#if !defined (vxworks)
			pid = getpid ();
#else  /* !vxworks */
			pid = taskIdSelf ();
#endif /* !vxworks */
			kill (pid,SIGQUIT);
#else
			raise(SIGTERM);
#endif /* WIN32 */
			return;
		}

		if (msg_out.status < 0)
		{
			if (msg_type == ERROR_TYPE)
			{
#ifdef WIN32
				{
					char msg[1024];
					snprintf (msg, sizeof(msg), "Message Server : error number %d",msg_out.error);
					PRINTF(msg);
					snprintf (msg, sizeof(msg), "message_buffer contents :\n%s",
					    message_buffer[msg_type].messages );
					PRINTF(msg);
				}
#else
				printf ("Message Server : error number %d",msg_out.error);
				printf ("message_buffer contents :\n%s",
				    message_buffer[msg_type].messages );
#endif
/*
 *  exit server if message server fails to
 *  process an error message
 */

#if !defined (WIN32)
#if !defined (vxworks)
				pid = getpid ();
#else  /* !vxworks */
                        	pid = taskIdSelf ();
#endif /* !vxworks */
				kill (pid,SIGQUIT);
#else
				raise(SIGTERM);
#endif /* WIN32 */
				return;
			}
			dev_printerror_no (WRITE,"Message Server : ",msg_out.error);
			dev_printerror (SEND,"%s","msg_send : Message transfer failed");
			return;
		}
/*
 * free message buffer
 */
		msg_clear ( msg_type );
	}
}

/**@ingroup messageAPIintern
 * This functions frees the contents of the message buffer.
 * 
 * @param msg_type type of message: error or debug
 */
static void msg_clear (_Int msg_type)
{
	if (message_buffer[msg_type].messages != NULL) 
		free ( message_buffer[msg_type].messages );
	message_buffer[msg_type].init_flg = 0;
	message_buffer[msg_type].nbytes = 0;
	message_buffer[msg_type].messages = NULL;
}

/**@ingroup dbaseAPI
 * This function imports the static database service.
 *
 * @param error Will contain an appropriate error
 *		code if the corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc db_import (long *error)
{
	CLIENT		*clnt;
        enum clnt_stat  clnt_stat;
	char		*hstring;
	char		nethost[HOST_NAME_LENGTH], 
			*nethost_env;

	*error = DS_OK;

/*
 * check wether a database server is already imported
 */
	if (config_flags.database_server)
		return (DS_OK);

/*
 * check wether the system is already configured
 */
	if ( !config_flags.configuration && (setup_config(error) != DS_OK) )
		return (DS_NOTOK);

/*
 * Create database server client handle with data from
 * global dbserver_info structure.
 *
 * Create a client handle for version 3! 
 */
	clnt = clnt_create ( db_info.conf->server_host, db_info.conf->prog_number, DB_VERS_3, "udp");
	if (clnt == NULL)
	{
		hstring = clnt_spcreateerror ("db_import");
		dev_printerror (SEND,"%s",hstring);
		*error = DevErr_DbImportFailed;
		return (DS_NOTOK);
	}

	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &dbase_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &dbase_timeout);

/*
 * Check the database server version. 
 * If it is not a new version 3 server, create a handle to version 2!!!
 */ 
        clnt_stat = clnt_call (clnt, NULLPROC, (xdrproc_t)xdr_void, NULL, 
			       (xdrproc_t)xdr_void, NULL, TIMEVAL(timeout));
        if (clnt_stat != RPC_SUCCESS)
        {
        	if ( clnt_stat != RPC_PROGVERSMISMATCH )
		{
			clnt_perror (clnt,"db_import()");
			clnt_destroy (clnt);
			*error = DevErr_DbImportFailed;
			return (DS_NOTOK);
		}

/*
 * If it was an old version 2 of the database server,
 * a version mismatch occured because the client handle
 * was created for version 3.
 * Destroy the handle and use version 2.
 */
		else
		{
/*
 * Destroy version 3 handle.
 */
			clnt_destroy (clnt);

/*
 * Set version number to 2 and recreate the
 * client handle.
 *  read environmental variable NETHOST
 */
			clnt = clnt_create ( db_info.conf->server_host, db_info.conf->prog_number, DB_VERS_2, "udp");
			if (clnt == NULL)
			{
				hstring = clnt_spcreateerror ("db_import()");
				dev_printerror (SEND,"%s",hstring);
				*error = DevErr_DbImportFailed;
				return (DS_NOTOK);
			}

			db_info.conf->vers_number = DB_VERS_2;
		}
	}
	if ((nethost_env = (char *)getenv ("NETHOST")) == NULL)
	{
	   	db_info.conf->vers_number = DB_VERS_3;
		*error = DevErr_NethostNotDefined;
		return (DS_NOTOK);
	}

/* 
 * pass the information to the database server_info structure
 */
	strncpy (db_info.conf->device_name, "DatabaseServer", sizeof(db_info.conf->device_name));
        db_info.conf->device_name[sizeof(db_info.conf->device_name) - 1] = '\0';
	db_info.conf->clnt = clnt;
	db_info.conf->ds_id = 0;
	db_info.conf->no_svr_conn = 1;

	config_flags.database_server = True;
	strncpy(nethost, nethost_env, sizeof(nethost));
        nethost[sizeof(nethost) - 1] = '\0';

/*
 * for multi-nethost support copy the default configuration
 * and database info to multi_nethost[0] in case one of the 
 * api routines makes a reference to it.
 */
	multi_nethost[0].config_flags = config_flags;
	multi_nethost[0].db_info = db_info.conf;
	return db_import_multi(nethost,error);
}

/**@ingroup dbaseAPI
 * import the static database service for a multi-nethost
 *
 * @param nethost 	which nethost to import the static database for
 * @param error 	Will contain an appropriate error code if the 
 *			corresponding call returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long _DLLFunc db_import_multi (char *nethost, long *error)
{
	CLIENT		*clnt;
        enum clnt_stat  clnt_stat;
	char		*hstring;
	nethost_info	*nethost_i;
	long 		i_nethost;

	*error = DS_OK;

/*
 * identify the nethost in the array of multi-nethosts
 */
	if ((i_nethost = get_i_nethost_by_name(nethost,error)) < 0)
	{
/*
 * if not try to configure the new nethost
 */
                if ( (setup_config_multi (nethost,error)) < 0 )
                        return (DS_NOTOK);
		i_nethost = get_i_nethost_by_name(nethost,error);
	}
/* 
 * which nethost to import the database from
 */
	nethost_i = &(multi_nethost[i_nethost]);

/*
 * check wether a database server is already imported
 */
	if (nethost_i->config_flags.database_server)
		return (DS_OK);

/*
 * Create message server client handle with data from
 * global dbserver_info structure.
 */

/* 
 * Create a client handle for version 3! 
 */
	clnt = clnt_create ( nethost_i->db_info->server_host, nethost_i->db_info->prog_number, DB_VERS_3, "udp");
	if (clnt == NULL)
	{
		hstring = clnt_spcreateerror ("db_import");
		dev_printerror (SEND,"%s",hstring);
		*error = DevErr_DbImportFailed;
		return (DS_NOTOK);
	}

	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &dbase_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &dbase_timeout);

/*
 * Check the database server version. 
 * If it is not a new version 3 server, create a handle to version 2!!!
 */ 
	clnt_stat = clnt_call (clnt, NULLPROC, (xdrproc_t)xdr_void, NULL, 
			       (xdrproc_t)xdr_void, NULL, TIMEVAL(timeout));
	if (clnt_stat != RPC_SUCCESS)
	{
		if ( clnt_stat != RPC_PROGVERSMISMATCH )
		{
		clnt_perror (clnt,"db_import()");
		clnt_destroy (clnt);
		*error = DevErr_DbImportFailed;
		return (DS_NOTOK);
		}

/*
 * If it was an old version 2 of the database server,
 * a version mismatch occured because the client handle
 * was created for version 3.
 * Destroy the handle and use version 2.
 */
		else
		{
/*
 * Destroy version 3 handle.
 */
			clnt_destroy (clnt);
/*
 * Set version number to 2 and recreate the client handle.
 */
			clnt = clnt_create ( nethost_i->db_info->server_host, nethost_i->db_info->prog_number, DB_VERS_2, "udp");
			if (clnt == NULL)
			{
				hstring = clnt_spcreateerror ("db_import()");
				dev_printerror (SEND,"%s",hstring);
				*error = DevErr_DbImportFailed;
				return (DS_NOTOK);
			}
			nethost_i->db_info->vers_number = DB_VERS_2;
		}
	}
	else
		nethost_i->db_info->vers_number = DB_VERS_3;

/* 
 * pass the information to the database server_info structure
 */
	strncpy(nethost_i->db_info->device_name,"DatabaseServer", sizeof(nethost_i->db_info->device_name));
	nethost_i->db_info->device_name[sizeof(nethost_i->db_info->device_name)] = '\0';
	nethost_i->db_info->clnt = clnt;
	nethost_i->db_info->ds_id = 0;
	nethost_i->db_info->no_svr_conn = 1;
	nethost_i->config_flags.database_server = True;
	if (i_nethost == 0)
	{
/*
 * if this is the first nethost (i_nethost=0) then add mono-nethost 
 * support by calling db_import to import the "default" nethost
 */
		if (!config_flags.database_server)
		{
			if (db_import(error) != DS_OK)
				return(DS_NOTOK);
			config_flags = nethost_i->config_flags;
			db_info.conf = multi_nethost[0].db_info ;
		}
	}
	return (DS_OK);
}

/**@ingroup dsAPIintern
 * This function gets the necessary configuration information for a static database service and a message
 * service from  a network manager.
 *
 * The host of the network manager must be specified by the environment variable NETHOST.    
 *
 * @param error Will contain an appropriate error code if the corresponding call returns a non-zero value.
 * 
 * @return  DS_OK or DS_NOTOK
 */
long setup_config (long *error)
{
	char				nethost[HOST_NAME_LENGTH], 
					*nethost_env;
	CLIENT				*clnt;
	enum clnt_stat			clnt_stat;
	int				pid;
	static _manager_data 		manager_data;
	static _register_data  		register_data;
	static char			host_name[HOST_NAME_LENGTH];
#if 0
	static struct _devserver	msg_ds, db_ds;
#endif
	dev_printdebug (DBG_TRACE | DBG_API, "\nsetup_config() : entering routine\n");

#ifdef WIN32
	if (!win32_rpc_inited)
	{
		rpc_nt_init();
		win32_rpc_inited = 1;
	}
#endif /* WIN32 */

	*error = DS_OK;
	memset ((char *)&manager_data,0,sizeof(manager_data));

/*
 *  read environmental variable NETHOST
 */
	if ( (nethost_env = (char *)getenv ("NETHOST")) == NULL )
	{
		*error = DevErr_NethostNotDefined;
		return (DS_NOTOK);
	}
	strncpy(nethost, nethost_env, sizeof(nethost));
        nethost[sizeof(nethost) - 1] = '\0';

/*
 *  create registration information that is send to
 *  the network manager and stored in the System.log file.
 */
	taco_gethostname (host_name, sizeof(host_name));

#if !defined (WIN32)
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
#else  /* !WIN32 */
        pid = _getpid ();
#endif /* !WIN32 */
	register_data.host_name   = host_name;
	register_data.prog_number = pid;
	register_data.vers_number = 0;

/*
 * Create network manager client handle 
 */
	clnt = clnt_create ( nethost, NMSERVER_PROG, NMSERVER_VERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror ("setup_config");
		*error = DevErr_NoNetworkManagerAvailable;
		return (DS_NOTOK);
	}
	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &api_timeout);

/*
 *   get configuration information from a network manager
 */
	clnt_stat = clnt_call (clnt, RPC_GET_CONFIG,
	    (xdrproc_t)xdr__register_data, (caddr_t) &register_data,
	    (xdrproc_t)xdr__manager_data, (caddr_t) &manager_data, TIMEVAL(timeout));
	if (clnt_stat != RPC_SUCCESS)
	{
		if ( clnt_stat != RPC_PROGVERSMISMATCH )
		{
			clnt_perror (clnt,"setup_config ()");
			clnt_destroy (clnt);
			*error = DevErr_NetworkManagerNotResponding;
			return (DS_NOTOK);
		}

/*
 * If it was an old version 1 of the manager process,
 * a version mismatch occured because the client handle
 * was created for version 4.
 * Destroy the handle and use version 1.
 */
		else
		{
/*
 * Destroy version 4 handle.
 */
			clnt_destroy (clnt);

/*
 * Set version number to 1 and recreate the client handle.
 */
			clnt = clnt_create (nethost,NMSERVER_PROG,NMSERVER_VERS_1,"udp");
			if (clnt == NULL)
			{
				clnt_pcreateerror ("setup_config");
				*error = DevErr_NoNetworkManagerAvailable;
				return (DS_NOTOK);
			}

/*
 *   get configuration information from a network manager running version 1.
 */
			clnt_stat = clnt_call (clnt, RPC_GET_CONFIG,
						(xdrproc_t)xdr__register_data, (caddr_t) &register_data,
						(xdrproc_t)xdr__manager_data_3, (caddr_t) &manager_data, TIMEVAL(timeout));
			if (clnt_stat != RPC_SUCCESS)
			{
				clnt_perror (clnt,"setup_config ()");
				clnt_destroy (clnt);
				*error = DevErr_NetworkManagerNotResponding;
				return (DS_NOTOK);
			}
		}
	}

	if (manager_data.status < 0)
	{
		*error = manager_data.error;
/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
		xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
		
		clnt_destroy (clnt);
		return (DS_NOTOK);
	}

/*
  * Initialise the XDR data type list with all data types
 * specified in the Kernel of the system.
 */

	if ( xdr_load_kernel (error) == DS_NOTOK )
	{
/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
		xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
		clnt_destroy (clnt);
		return (DS_NOTOK);
	}

/*
 *  put message server and database server configuration
 *  into the global structures msgserver_info and
 *  dbserver_info.
 *
 * do not allocate space for _devserver structure because this routine
 * could be called multiple times e.g. when database server has to be
 * reimported, simply point to a static _devserver structure which
 * is locally allocated in static space
 */
	if (max_nethost <= 0) 
		nethost_alloc(error);
	msg_info.conf=(devserver)msg_ds;
	db_info.conf =(devserver)db_ds;

	snprintf (msg_info.conf->server_host, sizeof(msg_info.conf->server_host), "%s", 
	    manager_data.msg_info.host_name);
	msg_info.conf->prog_number =  manager_data.msg_info.prog_number;
	msg_info.conf->vers_number =  manager_data.msg_info.vers_number;

	snprintf (db_info.conf->server_host, sizeof(db_info.conf->server_host), "%s", 
	    manager_data.db_info.host_name);
	db_info.conf->prog_number =  manager_data.db_info.prog_number;
	db_info.conf->vers_number =  manager_data.db_info.vers_number;

/*
 * Enable the security system if the Manager indicates 
 * the use of the security system.
 */
	config_flags.security =  manager_data.security; 

/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
	xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
	clnt_destroy (clnt);
	config_flags.configuration = True;
/*
 * for multi-nethost support copy the default configuration,
 * database and message info to multi_nethost[0] in case one of the 
 * api routines makes a reference to it.
 *
 * first allocate space for a minimum no. of nethosts
 */
	if (max_nethost <= 0) 
		nethost_alloc(error);
	strncpy(nethost,nethost_env,HOST_NAME_LENGTH);
        nethost[sizeof(nethost) - 1] = '\0';
//	snprintf(nethost, sizeof(nethost), "%s",nethost_env);

	strncpy(multi_nethost[0].nethost,nethost, sizeof(multi_nethost[0].nethost));
        multi_nethost[0].nethost[sizeof(multi_nethost[0].nethost) - 1] = '\0';
	multi_nethost[0].config_flags = config_flags;
	multi_nethost[0].db_info = db_info.conf;
	multi_nethost[0].msg_info = msg_info.conf;
	return setup_config_multi(nethost,error);
}


/**@ingroup dsAPI
 * This function gets the necessary configuration information for a static database service and a message
 * service from  a network manager in a multi-nethost environment.
 *
 * The host of the network manager is specified by the nethost parameter.    
 *
 * @param nethost  name of nethost to configure
 *
 * @param error Will contain an appropriate error code if the corresponding call
 *		returns a non-zero value.
 *
 * @return DS_OK or DS_NOTOK
 */
long setup_config_multi (char *nethost, long *error)
{
	_manager_data 		manager_data={0};
	_register_data  	register_data;
	static char		host_name[HOST_NAME_LENGTH];
	CLIENT			*clnt;
	enum clnt_stat		clnt_stat;
	int			pid;
	long			i_nethost, i;
#ifndef _UCC
	char*                   nethost_env;
	static char             nethost_buffer[80]; /* used if * nethost=NULL */

#endif

#ifdef WIN32
	if (!win32_rpc_inited)
	{
		rpc_nt_init();
		win32_rpc_inited = 1;
	}
#endif /* WIN32 */

	*error = DS_OK;
	memset ((char *)&manager_data,0,sizeof(manager_data));

	dev_printdebug (DBG_TRACE | DBG_API, "\nsetup_config_multi() : entering routine\n");

/*
 *  read environmental variable NETHOST
 */
	if ( nethost == NULL )
	{
		if ( (nethost_env = (char *)getenv ("NETHOST")) == NULL )
		{
			*error = DevErr_NethostNotDefined;
			return (DS_NOTOK);
		}
		nethost=nethost_buffer;
		snprintf(nethost, sizeof(nethost), "%s",nethost_env);
	}

/*
 * look for the nethost in the array of multi_nethosts[] and
 * return its index; if the nethost is not found then return the
 * index of the next free slot in the array
 */
	if ((i_nethost = get_i_nethost_by_name(nethost,error)) < 0)
	{
/*
 * nethost is not defined in the list of imported nethosts, add it
 * to the next free slot in multi_nethosts
 *
 * but first check to see whether the $NETHOST environment variable is
 * defined. If so then set it up first as default nethost by calling 
 * setup_config(). The $NETHOST environment variable has precedance 
 * over the nethost defined in the device name
 *
 * only call setup_config() if it hasn't been called already - andy 19nov98
 */
		if ((char *)getenv("NETHOST") != NULL)
		{
			if ( !config_flags.configuration )
			{
				if ( (setup_config (error)) != DS_OK )
					return (DS_NOTOK);
			}
		}
		for (i=0; i< max_nethost; i++)
		{
			if (!multi_nethost[i].config_flags.configuration)
			{
				break;
			}
		}
		i_nethost = i;
		dev_printdebug (DBG_TRACE | DBG_API, "\nsetup_config_multi() : add nethost %d\n",i_nethost);
/*
 * if this is the first time a nethost is being defined or the nethost
 * then table is full therefore allocate space for another MIN_NETHOST 
 * nethosts i.e. there is no limit on the maximumu number of nethosts
 * 
 * andy 30/6/98
 */
		if ((i == 0) || (i >= max_nethost))
		{
			nethost_alloc(error);
		}
 	}
/*
 *  create registration information to be sent to
 *  the network manager and stored in the System.log file.
 */
	taco_gethostname (host_name, sizeof(host_name));

#if !defined (WIN32)
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
#else /* !WIN32 */
	pid = _getpid ();
#endif	/* !WIN32 */
	register_data.host_name   = host_name;
	register_data.prog_number = pid;
	register_data.vers_number = 0;

/*
 * Create network manager client handle 
 */
	clnt = clnt_create ( nethost, NMSERVER_PROG, NMSERVER_VERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror ("setup_config");
		*error = DevErr_NoNetworkManagerAvailable;
		return (DS_NOTOK);
	}
	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &api_timeout);

/*
 * get configuration information from a network manager
 */

	clnt_stat = clnt_call (clnt, RPC_GET_CONFIG,
	    (xdrproc_t)xdr__register_data, (caddr_t) &register_data,
	    (xdrproc_t)xdr__manager_data, (caddr_t) &manager_data, TIMEVAL(timeout));
	if (clnt_stat != RPC_SUCCESS)
	{
		if ( clnt_stat != RPC_PROGVERSMISMATCH )
		{
			clnt_perror (clnt,"setup_config ()");
			clnt_destroy (clnt);
			*error = DevErr_NetworkManagerNotResponding;
			return (DS_NOTOK);
		}
/*
 * If it was an old version 1 of the manager process,
 * a version mismatch occured because the client handle
 * was created for version 4.
 * Destroy the handle and use version 1.
 */
		else
		{
/*
 * Destroy version 4 handle.
 */
			clnt_destroy (clnt);
/*
 * Set version number to 1 and recreate the client handle.
 */
			clnt = clnt_create (nethost,NMSERVER_PROG,NMSERVER_VERS_1,"udp");
			if (clnt == NULL)
			{
				clnt_pcreateerror ("setup_config");
				*error = DevErr_NoNetworkManagerAvailable;
				return (DS_NOTOK);
			}
/*
 * get configuration information from a network manager running version 1.
 */
			clnt_stat = clnt_call (clnt, RPC_GET_CONFIG,
				(xdrproc_t)xdr__register_data, (caddr_t) &register_data,
				(xdrproc_t)xdr__manager_data_3, (caddr_t) &manager_data, TIMEVAL(timeout));
			if (clnt_stat != RPC_SUCCESS)
			{
				clnt_perror (clnt,"setup_config ()");
				clnt_destroy (clnt);
				*error = DevErr_NetworkManagerNotResponding;
				return (DS_NOTOK);
			}
		}
	}


	if (manager_data.status < 0)
	{
		*error = manager_data.error;
/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
		xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
		clnt_destroy (clnt);
		return (DS_NOTOK);
	}

/*
 * put message server and database server configuration
 * into the global structures msgserver_info and
 * dbserver_info.
 *
 * do not allocate space for _devserver structure because this routine
 * could be called multiple times e.g. when database server has to be
 * reimported, simply point to a static _devserver structure which
 * is always allocated
 */
	multi_nethost[i_nethost].msg_info = (devserver)&msg_ds[i_nethost];
	multi_nethost[i_nethost].db_info = (devserver)&db_ds[i_nethost];

	snprintf (multi_nethost[i_nethost].msg_info->server_host, sizeof(multi_nethost[i_nethost].msg_info->server_host), "%s", 
	    manager_data.msg_info.host_name);
	multi_nethost[i_nethost].msg_info->prog_number =  
                                 manager_data.msg_info.prog_number;
	multi_nethost[i_nethost].msg_info->vers_number =  
                                 manager_data.msg_info.vers_number;

	snprintf (multi_nethost[i_nethost].db_info->server_host, sizeof(multi_nethost[i_nethost].db_info->server_host), "%s", 
	    manager_data.db_info.host_name);
	multi_nethost[i_nethost].db_info->prog_number =  
                                 manager_data.db_info.prog_number;
	multi_nethost[i_nethost].db_info->vers_number =  
                                 manager_data.db_info.vers_number;

/*
 * Enable the security system if the Manager indicates 
 * the use of the security system.
 */
	multi_nethost[i_nethost].config_flags.security = manager_data.security;

/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
	xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
	clnt_destroy (clnt);
	strncpy(multi_nethost[i_nethost].nethost,nethost, sizeof(multi_nethost[i_nethost].nethost));
        multi_nethost[i_nethost].nethost[sizeof(multi_nethost[i_nethost].nethost) - 1] = '\0';

	multi_nethost[i_nethost].config_flags.configuration = True;
	if (i_nethost == 0)
	{
/*
 * if this is the first nethost (i_nethost=0) then add mono-nethost 
 * support by calling setup_config to initialise the default nethost
 * i.e. the present nethost
 */
		if (!config_flags.configuration)
		{
/*
 * The string pointed to by string becomes part of the environment, so altering the string 
 * changes the environment.
 */
			static char	*nethost_tmp = NULL;
			if (!nethost_tmp)
				nethost_tmp = (char*)malloc(64);
/* 
 * Ultra-C++ does not find the symbole putenv() (maybe the prototype
 * is wrong). Supress its use for Ultra-C++. This means Ultra-C++
 * programs have to specify $NETHOST
 *
 * - andy 27nov96
 * Initialise the XDR data type list with all data types
 * specified in the Kernel of the system. 
 */
#ifndef _UCC
			snprintf(nethost_tmp, HOST_NAME_LENGTH + 20, "NETHOST=%s", nethost);
			putenv(nethost_tmp);
			nethost_env = (char *)getenv ("NETHOST");
#endif /*!_UCC*/
//			if (setup_config(error) != DS_OK)
		    	if ( xdr_load_kernel (error) == DS_NOTOK )
			{
/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
				xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
				clnt_destroy (clnt);
				return(DS_NOTOK);
			}
		    	config_flags = multi_nethost[0].config_flags;
		    	db_info.conf = multi_nethost[0].db_info;
		    	msg_info.conf = multi_nethost[0].msg_info;
		}
	}
	return (DS_OK);
}

/**@ingroup clientAPI
 * This function sets the host given by nethost as the current default nethost. In case of failure
 * the old nethost will be set.
 *
 * @param nethost host name of the new nethost
 * @param error points to the possible error code, only set if returns DS_NOTOK
 *
 * @return DS_OK or DS_NOTOK in case of failure
 */
long db_ChangeDefaultNethost(char* nethost,long *error)
{
	int i_nethost = get_i_nethost_by_name(nethost,error);
/* 
 * lookup in nethost array 
 */
	if (i_nethost < 0)
	{
/* 
 * lookup failed, try to import the host 
 */
		if(db_import_multi(nethost,error) != DS_OK)
			return (DS_NOTOK);

/* 
 * retry the lookup, it should now succeed 
 */
		if((i_nethost = get_i_nethost_by_name(nethost,error)) < 0)
/* 
 * this should never happen 
 */
			return (DS_NOTOK);
	}
/* 
 * set the default nethost vars from multi-nethost array 
 *
 * HINT: some day, there should be a global index, and all
 * functions could use multi_nethost[glob_index]  
 */
	config_flags = multi_nethost[i_nethost].config_flags;
	db_info.conf = multi_nethost[i_nethost].db_info;
	msg_info.conf = multi_nethost[i_nethost].msg_info;
	return (DS_OK);
}
/* Function: */

/**@ingroup dsAPIintern
 * This function formats the error string with a timestamp. The resulting string contains at
 * first the time stamp and then the message divided by a blank. The returned
 * string is malloced and has to be freed by the caller.
 * 
 * @param time_stamp 	time stamp string containing date and time
 * @param message 	the error message.
 * 
 * @return formatted error message, in case of failure NULL
 */
static char* format_error_string(const char* time_stamp,const char* message )
{
	char * error_str = (char*)malloc(strlen(time_stamp)+strlen(message)+2);
	if(error_str)
		snprintf(error_str, strlen(time_stamp) + strlen(message) + 2, "%s %s", time_stamp, message );
	return (error_str);
}


/**@ingroup dsAPIintern
 * This function reads the error string from the global error table or from the resource database.
 *
 * The resource name is: ERROR/team_no/server_no/error_ident
 * 
 * @param error   error number
 * 
 * @return error string.
 */ 
static char *get_error_string (long error)
{
	char		res_path[LONG_NAME_SIZE];
	char		res_name[SHORT_NAME_SIZE];
	char		*ret_str = NULL;
	char		*error_str = NULL;
	db_resource 	res_tab;
	long		db_error;
	unsigned short 	error_number_mask = 0xfff;
	unsigned short 	team;
	unsigned short 	server;
	unsigned short 	error_ident;
	short		i;

	time_t		sec;
	char		*time_stamp;
	static char	*no_error_mess=(char*)"No error message stored in the database for negative errors\n";
	static char	*db_error_mess=(char*)"Failed to get error message from database\n";

	dev_printdebug (DBG_TRACE | DBG_API, "\nget_error_string() : entering routine %d\n", error);

	db_error = DS_OK;

/*
 * Get the time stamp
 */
	time (&sec);
	time_stamp = ctime (&sec);
	time_stamp[24] = '\0';

/*
 * Negative errors are not supported, return an appropriate message
 * and a return the value DS_WARNING.
 */
        if ( error < DS_OK )
        {
	    return format_error_string(time_stamp,no_error_mess);
        }

/*
 * Decode the error number into the fields:
 * team, server and error_ident.
 */
	team   = (_Int)(error >> DS_TEAM_SHIFT);
	team   = team & DS_TEAM_MASK;
	server = (_Int)(error >> DS_IDENT_SHIFT);
	server = server & DS_IDENT_MASK;
	error_ident = (_Int)(error & error_number_mask);


/*
 * Search the global error table first, if the error number
 * indicates team = 0 and server = 0.
 */

	if ( team == 0 && server == 0 )
	{
		for (i=0; i<(MAX_DEVERR); i++)
		{
			if ((_Int)(DevErr_List[i].dev_errno) == (_Int)error)
			{
			    return format_error_string(time_stamp,DevErr_List[i].message);
			}
		}
	}
/*
 * Treat dynamic errors by returning the dynamically generated server 
 * error string rather than the static error string stored in the global
 * error table or the database
 */
	if (dev_error_string != NULL)
	{
	    return format_error_string(time_stamp, dev_error_string);
	}

/*
 * Create the resource path and the resource structure.
 */

	snprintf (res_path, sizeof(res_path), "ERROR/%d/%d", team, server);
	snprintf (res_name, sizeof(res_name), "%d", error_ident);
	dev_printdebug (DBG_TRACE | DBG_API, "get_error_string() : res_path = %s\n", res_path);
	dev_printdebug (DBG_TRACE | DBG_API, "get_error_string() : res_name = %s\n", res_name);

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

/*
 * Read the error string from the database.
 */

	if (db_getresource (res_path, &res_tab, 1, &db_error) == DS_NOTOK)
	{
		dev_printdebug (DBG_API | DBG_ERROR, "get_error_string() : db_getresource failed with error %d\n", db_error);

/*
 * If the database call returned an error, search the global
 * error table for a description.
 */

		for (i=0; i<(MAX_DEVERR); i++)
		{
			if ((_Int)(DevErr_List[i].dev_errno) == (_Int)db_error)
			{
			    return format_error_string(time_stamp,DevErr_List[i].message);
			}
		}

		return format_error_string(time_stamp,db_error_mess);
	}

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */
	if ( ret_str == NULL )
	{
	    return format_error_string(time_stamp,no_error_mess);
	}

/*
 * Found the error string in the database. Return the string with 
 * a time stamp.
 */
	error_str=format_error_string(time_stamp,ret_str);
	free (ret_str);
	return (error_str);
}

/**@ingroup dsAPI
 * This function is a server side call for generating dynamic error strings. If called
 * by the server while executing a @ref dev_putget() it will make a copy of the error
 * string and transmit it back to the client. The client can recover the error string 
 * by calling @ref dev_error_string() immediatly after the return of the @ref dev_putget()
 * call in question. Not if a new call to @ref dev_putget() is made the error string
 * returned by the previous call(s) is lost. This function can be called multiple times
 * to stack errors if necessary e.g. to return errors from multiple nested calls.
 *
 * @param error_string error string
 * 
 * @return   DS_OK or DS_NOTOK 
 */
long dev_error_push (char *error_string)
{
	if (dev_error_stack == NULL)
	{
		dev_error_stack = (char*)malloc(strlen(error_string)+1);
		if(!dev_error_stack)
			return DS_NOTOK;
		dev_error_stack[0] = 0;
	}
	else
	{
		char* tmp;
		tmp = (char*)realloc(dev_error_stack,
			strlen(dev_error_stack)+strlen(error_string)+1);
		if(!tmp)
			return DS_NOTOK;
		dev_error_stack=tmp;
	}
	strcat(dev_error_stack, error_string);
	return(DS_OK);
}

/**@ingroup dsAPI
 * Clear the dynamic error stack. This can be useful for servers
 * who want to remove error messages from the stack and return
 * their own error only.
 *
 * @return DS_OK 
 */
long dev_error_clear(void)
{
	if (dev_error_stack != NULL)
	{
		free(dev_error_stack);
		dev_error_stack = NULL;
	}
	return(DS_OK);
}

#define MAXLEVEL 10
#define MAX_ERR_STR 80
/**@ingroup dsAPI
 * This function is a server side call for generating dynamic error strings. If called
 * by the server while executing a @ref dev_putget() it will make a copy of the error
 * string and transmit it back to the client. The client can recover the error string 
 * by calling @ref dev_error_string() immediatly after the return of the @ref dev_putget()
 * call in question. Not if a new call to @ref dev_putget() is made the error string
 * returned by the previous call(s) is lost. This function can be called multiple times
 * to stack errors if necessary e.g. to return errors from multiple nested calls.
 *
 * The error messages will be intended by some spaces given by the value level.
 * @see dev_error_push
 *
 * @return DS_OK
 */
long dev_error_push_level(const char * message,int level)
{
	char tmp_store[MAX_ERR_STR];
	int cnt;
	if(level < 1)
		level=1;
/* 
 * max 10 level, to avoid too long lines 
 */
	if(level > MAXLEVEL) 
		level=MAXLEVEL; 
	for(cnt = 0; cnt < level; cnt++)
		tmp_store[cnt]=' ';
	tmp_store[0]='\n';
	strncpy(tmp_store+level,message,MAX_ERR_STR-level);
        tmp_store[MAX_ERR_STR - 1] = '\0'; 
	return dev_error_push(tmp_store);
}

static char RcsId[] = 
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/gen_api.c,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";
/*+*******************************************************************

 File       :	gen_api.c

 Project    :	Device Servers

 Description:	General Application Programmers Interface to the device
		servers and clients using the SUN-RPC.

 Author(s)  :	Jens Meyer
 		$Author: jkrueger1 $

 Original   :	January 1991

 Version    :	$Revision: 1.1 $

 Date       : 	$Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990-2000 by European Synchrotron Radiation Facility, 
                            Grenoble, France

********************************************************************-*/
#include <config.h>
#include <API.h>
#include <ApiP.h>
#include <API_xdr_vers3.h>

#include <Admin.h>
#include <DevErrors.h>
#include <DevSignal.h>
#ifdef vxworks
#include <taskLib.h>
#endif /* vxworks */

/*
 * Include file for variable function arguments.
 */
#ifdef sun
#include <varargs.h>
#include <signal.h>
#else
#ifndef OSK
#include <stdarg.h>
#endif
#endif

static long setup_config 	(long *error);
extern _DLLFunc long setup_config_multi 	(char *nethost, long *error);
static void msg_write 		(_Int msg_type, char *msg_string);
static void msg_send 		(_Int msg_type);
static void msg_clear 		(_Int msg_type);
static char *get_error_string 	(long error);

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
	False,False,False,False,False, "", "", 0L};


/*
 *  Database Server globals
 */

dbserver_info		db_info;

/*
 *  Message Server globals
 */

msgserver_info		msg_info;
static _message_buffer 	message_buffer [NUMBER_OF_MSG_TYPES];

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
 *
 * imports a message service.
 * DS_xxx parameters are used to name the
 * approriate error file and to display
 * a correct error window. 
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
long _DLLFunc msg_import (char *DS_name, char *DS_host, long DS_prog_number,
		char *DS_display, long *error)
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
		if ( (setup_config (error)) < 0 )
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

	sprintf (msg_info.conf->device_name,"MessageServer");
	msg_info.conf->clnt = clnt;
	msg_info.conf->ds_id = 0;
	msg_info.conf->no_svr_conn = 1;

	sprintf (msg_info.DS_name,    "%s", DS_name);
	sprintf (msg_info.DS_host,    "%s", DS_host);
	msg_info.DS_prog_number         =   DS_prog_number;
	sprintf (msg_info.DS_display, "%s", DS_display);

	config_flags.message_server = True;

	return (DS_OK);
}



/*
 * Attention: OS9 version 2.4 can not handle variable argument
 * lists. It will still use the old tricky function!!
 */
#if (!defined OSK) && (!defined _GNU_PP)
/**
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
 * get variable argument list pointer in order to pass it to vsprintf()
 */
#if (defined sun) || (defined irix)
	va_start(args);
#else
	va_start(args, fmt);
#endif /* sun */

	vsprintf (buffer,fmt,args);
	strcat (buffer,"\n");

	va_end(args);

	msg_write ( ERROR_TYPE, buffer );

	if ( mode != WRITE )
	{
		/*
            * send messages to message server if imported
            */

		if ( config_flags.message_server )
		{
			msg_send ( ERROR_TYPE );
		}
		else
		{
#ifdef _NT
			char msg [1024];
			sprintf ( msg, "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
			PRINTF(msg); //conout(msg);
#else
			printf ( "\n\n%s\n", 
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
/**
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
	{
		sprintf (buffer,fmt,str);
	}
	else
	{
		sprintf (buffer,fmt);
	}
	strcat (buffer,"\n");

	msg_write ( ERROR_TYPE, buffer );

	if ( mode != WRITE )
	{
		/*
            * send messages to message server if imported
            */

		if ( config_flags.message_server )
		{
			msg_send ( ERROR_TYPE );
		}
		else
		{
#ifdef _NT
			char msg[1024];
			sprintf ( msg, "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
			PRINTF(msg); //conout(msrg);
#else
			printf ( "\n\n%s\n", 
			    message_buffer[ERROR_TYPE].messages);
#endif
			msg_clear ( ERROR_TYPE );
		}
	}
}
#endif /* OSK */

/*+**********************************************************************
 Function   :	extern void *dev_printerror_no()

 Description:   retrieves the related error string for an error number
		from the list in DevErrors.h or the rsource database
		and sends the error string with the choosen mode 
		to dev_printerror().

 Arg(s) In  :	short mode     - mode for message buffer : WRITE or SEND
	    :	char *comment  - comment to be printed before 
				 error string.
	    :   long dev_errno - device server system error number.

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

void _DLLFunc dev_printerror_no (DevShort mode, char *comment, long dev_errno)
{
	char *error_string=NULL;

	/*
         * get error string from the resource database
         */

	error_string = get_error_string ( dev_errno);

	if (comment != NULL)
	{
		dev_printerror (mode,"%s",comment);
	}

	dev_printerror (mode,"%s",error_string);
	if (error_string != NULL) free(error_string);
}


/*+**********************************************************************
 Function   :	extern char *dev_error_str()

 Description:   retrieves the related error string for an error number
		from the list in DevErrors.h.

 Arg(s) In  :   long dev_errno - device server system error number.

 Arg(s) Out :   none

 Return(s)  :   error string, related to dev_errno.
***********************************************************************-*/

_DLLFunc char * dev_error_str (long dev_errno)
{
	char		*error_string = NULL;

	/*
	 * get error string from the resource database
	 */

	error_string = get_error_string (dev_errno);

	return(error_string);

}

/*
 * Attention: OS9 version 2.4 can not handle variable argument
 * lists. It will still use the old tricky function!!
 */
#if (!defined OSK) && (!defined _GNU_PP)
/*+**********************************************************************
 Function   :   extern void dev_printdebug()

 Description:   Send debug string to a message service
		or to stdout, if a message service
		is not imported.
            :   A debug string will be send if one of the debug_bits
		related to the string is set in the global debug_flag.
            :   Debug strings can br only 256 characters long.

 Arg(s) In  :	long debug_bits  - debug bits on which to send the
				   information.
	    :   char *fmt        - format string in printf() style.
            :   double a0,...,a9 - reserving memory for variables to print.

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

void _DLLFunc dev_printdebug (long debug_bits, char *fmt, ...)
{
	char		debug_string[256];
	va_list	args;
/*
 * get variable argument list pointer in order to pass it to vsprintf()
 */
#if (defined sun) || (defined irix)
	va_start(args);
#else
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
			vsprintf (debug_string,fmt,args);
			/*printf("dev_printdebug(): strlen(fmt) %d strlen(debug_string) %d\n",strlen(fmt),strlen(debug_string));*/

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
#ifdef _NT
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
/*+**********************************************************************
 Function   :   extern void dev_printdebug()

 Description:   Send debug string to a message service
		or to stdout, if a message service
		is not imported.
            :   A debug string will be send if one of the debug_bits
		related to the string is set in the global debug_flag.
            :   Debug strings can br only 256 characters long.

 Arg(s) In  :	long debug_bits  - debug bits on which to send the
				   information.
	    :   char *fmt        - format string in printf() style.
            :   double a0,...,a9 - reserving memory for variables to print.

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/
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
			{
				sprintf (debug_string,fmt,str);
			}
			else
			{
				sprintf (debug_string,fmt);
			}

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
#ifdef _NT
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

/*+**********************************************************************
 Function   :   extern void dev_printdiag()

 Description:   Send device server diagnostic_string to a message 
		service or to stdout, if a message service
		is not imported.
            :   Diagnostic strings can be only 256 characters long.
		Longer texts can be send by storing short
		strings in the message buffer with the WRITE
		mode and send the whole buffer by using
		the SEND mode with the last string.

 Arg(s) In  :	short mode       - mode for message buffer : WRITE or SEND
	    :   char *fmt        - format string in printf() style.
            :   double a0,...,a9 - reserving memory for variables to print.

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

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
 * get variable argument list pointer in order to pass it to vsprintf()
 */
#if (defined sun) || (defined irix)
	va_start(args);
#else
	va_start(args, fmt);
#endif /* sun */

	/*
 * vsprintf() gives a core dump at present (17sep94) replace it with
 * a simple sprintf() and ignore the variable arguments for the moment
 *
 	sprintf (buffer,fmt);
 */
	vsprintf (buffer,fmt,args);

	va_end(args);

	msg_write ( DIAG_TYPE, buffer );

	if ( mode != WRITE )
	{
		/*
            * send messages to message server if imported
            */

		if ( config_flags.message_server )
		{
			msg_send ( DIAG_TYPE );
		}
		else
		{
#ifdef _NT
			char msg[1024];
			sprintf (msg,"%s", message_buffer[DIAG_TYPE].messages);
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

/**/
/*+**********************************************************************
 Function   :   extern void dev_printdiag()

 Description:   Send device server diagnostic_string to a message 
		service or to stdout, if a message service
		is not imported.
            :   Diagnostic strings can be only 256 characters long.
		Longer texts can be send by storing short
		strings in the message buffer with the WRITE
		mode and send the whole buffer by using
		the SEND mode with the last string.

 Arg(s) In  :	short mode       - mode for message buffer : WRITE or SEND
	    :   char *fmt        - format string in printf() style.

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/
void dev_printdiag (DevShort mode,char *fmt,char *str)
{
	char  		buffer [256];

	if ( mode == CLEAR )
	{
		msg_clear ( DIAG_TYPE );
		return;
	}

	if (str != NULL)
	{
		sprintf (buffer,fmt,str);
	}
	else
	{
		sprintf (buffer,fmt);
	}

	msg_write ( DIAG_TYPE, buffer );

	if ( mode != WRITE )
	{
		/*
            * send messages to message server if imported
            */

		if ( config_flags.message_server )
		{
			msg_send ( DIAG_TYPE );
		}
		else
		{
#ifdef _NT
			char msg[1024];
			sprintf (masg,"%s", message_buffer[DIAG_TYPE].messages);
			PRINTF(msg); //conout(msg);
#else
			printf ("%s", message_buffer[DIAG_TYPE].messages);
#endif
			msg_clear ( DIAG_TYPE );
		}
	}
}
#endif /* OSK */


/*+**********************************************************************
 Function   :	static void msg_write()

 Description:   writing messages into the message buffer  

 Arg(s) In  :   int msg_type     - type of message : error or debug
	    :	char *msg_string - message string

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

static void msg_write (_Int msg_type, char *msg_string)
{
	char 	*help;
	int  	len;
	int   	i;
	int		pid = 0;
	static short init_flg = 0;

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
		len = strlen(msg_string) + 1;
		if ( (message_buffer[msg_type].messages = (char *)malloc (len)) == NULL )
		{
#ifdef _NT
			PRINTF("msg_write() : Insufficient memory for allocation !");
#else
			printf ("msg_write() : Insufficient memory for allocation !");
#endif
#if !defined (_NT)
#if !defined (vxworks)
			pid = getpid ();
#else  /* !vxworks */
			pid = taskIdSelf ();
#endif /* !vxworks */
			kill (pid,SIGQUIT);
#else
			raise(SIGTERM);
#endif	/* _NT */
		}

		message_buffer[msg_type].nbytes = len;
		strcpy ( message_buffer[msg_type].messages , msg_string);
		message_buffer[msg_type].init_flg = 1;
	}

	else
		/*
       * reallocate space for further messages
       */

		{
			len = message_buffer[msg_type].nbytes + strlen (msg_string);
			help = message_buffer[msg_type].messages;

			if ((help=(char *)realloc(message_buffer[msg_type].messages,len)) == NULL)
			{
#ifdef _NT
				{
				char msg[1024];
				PRINTF("msg_write() : Insufficient memory for reallocation !");
				sprintf(msg,"message_buffer contents :\n%s",
				    message_buffer[msg_type].messages );
				PRINTF(msg);
				}
#else
				printf("msg_write() : Insufficient memory for reallocation !");
				printf ("message_buffer contents :\n%s",
				    message_buffer[msg_type].messages );
#endif
#if !defined (_NT)
#if !defined (vxworks)
				pid = getpid ();
#else  /* !vxworks */
				pid = taskIdSelf ();
#endif /* !vxworks */
				kill (pid,SIGQUIT);
#else
				raise(SIGTERM);
#endif /* _NT */
			}

			message_buffer[msg_type].nbytes = len;
			message_buffer[msg_type].messages = help;
			strcat ( message_buffer[msg_type].messages , msg_string);
		}

}


/*+**********************************************************************
 Function   :	static void msg_send()

 Description:   sends the contents of the message buffer
		to the message server or stdout, if no
		message service is imported.

 Arg(s) In  :   int msg_type     - type of message : error or debug

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

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
#ifdef _NT
			{
			char msg[1024];
			PRINTF(clnt_sperror (msg_info.conf->clnt,"msg_send()"));
			sprintf(msg, "message_buffer contents :\n%s",
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

#if !defined (_NT)
#if !defined (vxworks)
			pid = getpid ();
#else  /* !vxworks */
			pid = taskIdSelf ();
#endif /* !vxworks */
			kill (pid,SIGQUIT);
#else
			raise(SIGTERM);
#endif /* _NT */
			return;
		}

		if (msg_out.status < 0)
		{
			if (msg_type == ERROR_TYPE)
			{
#ifdef _NT
				{
				char msg[1024];
				sprintf (msg,"Message Server : error number %d",msg_out.error);
				PRINTF(msg);
				sprintf (msg,"message_buffer contents :\n%s",
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

#if !defined (_NT)
#if !defined (vxworks)
				pid = getpid ();
#else  /* !vxworks */
                        	pid = taskIdSelf ();
#endif /* !vxworks */
				kill (pid,SIGQUIT);
#else
				raise(SIGTERM);
#endif /* _NT */
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


/*+**********************************************************************
 Function   :	static void msg_clear()

 Description:   frees the contents of the message buffer

 Arg(s) In  :   int msg_type     - type of message : error or debug

 Arg(s) Out :   none

 Return(s)  :   none
***********************************************************************-*/

static void msg_clear (_Int msg_type)
{
	if (message_buffer[msg_type].messages != NULL) 
		free ( message_buffer[msg_type].messages );
	message_buffer[msg_type].init_flg = 0;
	message_buffer[msg_type].nbytes = 0;
	message_buffer[msg_type].messages = NULL;
}


/*+**********************************************************************
 Function   :	extern long db_import()

 Description:   import the static database service

 Arg(s) In  :   none

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
		    	      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

long _DLLFunc db_import (long *error)
{
	CLIENT		*clnt;
        enum clnt_stat  clnt_stat;
	char		*hstring;

	*error = DS_OK;

	/*
	 * check wether a database server is already imported
	 */

	if (config_flags.database_server)
		return (DS_OK);

	/*
	 * check wether the system is already configured
	 */

	if ( !config_flags.configuration )
	{
		if ( (setup_config (error)) < 0 )
			return (DS_NOTOK);
	}


	/*
	 * Create message server client handle with data from
	 * global dbserver_info structure.
	 */

	/* Create a client handle for version 3! */
	clnt = clnt_create ( db_info.conf->server_host,
	    		     db_info.conf->prog_number,
	    		     DB_VERS_3,
	    		     "udp");
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
	 * If it is not a new version 3 server, create a handle to
	 * version 2!!!
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
               */

	      clnt = clnt_create ( db_info.conf->server_host,
	    		           db_info.conf->prog_number,
	    		           DB_VERS_2,
	    		           "udp");
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
	else
	   {
	   db_info.conf->vers_number = DB_VERS_3;
	   }


	/* 
 	 * pass the information to the database server_info structure
 	 */

	sprintf (db_info.conf->device_name,"DatabaseServer");
	db_info.conf->clnt = clnt;
	db_info.conf->ds_id = 0;
	db_info.conf->no_svr_conn = 1;

	config_flags.database_server = True;

/*
 * for multi-nethost support copy the default configuration
 * and database info to multi_nethost[0] in case one of the 
 * api routines makes a reference to it.
 */
	multi_nethost[0].config_flags = config_flags;
	multi_nethost[0].db_info = db_info.conf;

	return (DS_OK);
}


/*+**********************************************************************
 Function   :	extern long db_import_multi()

 Description:   import the static database service for a multi-nethost

 Arg(s) In  :   nethost - which nethost to import the static database
		          for

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
		    	      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
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

	/* Create a client handle for version 3! */
	clnt = clnt_create ( nethost_i->db_info->server_host,
	    		     nethost_i->db_info->prog_number,
	    		     DB_VERS_3,
	    		     "udp");
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
	 * If it is not a new version 3 server, create a handle to
	 * version 2!!!
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
               */

	      clnt = clnt_create ( nethost_i->db_info->server_host,
	    		           nethost_i->db_info->prog_number,
	    		           DB_VERS_2,
	    		           "udp");
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
	   {
	   nethost_i->db_info->vers_number = DB_VERS_3;
	   }


	/* 
 	 * pass the information to the database server_info structure
 	 */

	sprintf (nethost_i->db_info->device_name,"DatabaseServer");
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
			{
				return(DS_NOTOK);
			}
		}
	}

	return (DS_OK);
}


/*+**********************************************************************
 Function   :	static long setup_config()

 Description:   gets the necessary configuration information
		for a static database service and a message
		service from  a network manager.
            :   The host of the network manager must be 
	        specified by the environment variable NETHOST.    

 Arg(s) In  :   none

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
		    	      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

static long setup_config (long *error)
{
	char				nethost[SHORT_NAME_SIZE], *nethost_env;
	CLIENT				*clnt;
	enum clnt_stat			clnt_stat;
	int				pid;
	static _manager_data 		manager_data;
	static _register_data  		register_data;
	static char			host_name[SHORT_NAME_SIZE];
	static struct _devserver	msg_ds, db_ds;

#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\nsetup_config() : entering routine\n");
#endif /* EBUG */

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
	sprintf(nethost, "%s",nethost_env);

	/*
	 *  create registration information that is send to
	 *  the network manager and stored in the System.log file.
	 */

	gethostname (host_name,32);

#if !defined (_NT)
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
#else  /* !_NT */
        pid = _getpid ();
#endif /* !_NT */
	register_data.host_name   = host_name;
	register_data.prog_number = pid;
	register_data.vers_number = 0;

	/*
	 * Create network manager client handle 
	 */

	clnt = clnt_create ( nethost,
	    NMSERVER_PROG,
	    NMSERVER_VERS,
	    "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror ("setup_config");
		*error = DevErr_NoNetworkManagerAvailable;
		return (DS_NOTOK);
	}

	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &api_timeout);

	/*
       	 *   get configuration information from
       	 *   a network manager
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
               * Set version number to 1 and recreate the
               * client handle.
               */

	clnt = clnt_create (nethost,NMSERVER_PROG,NMSERVER_VERS_1,"udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror ("setup_config");
		*error = DevErr_NoNetworkManagerAvailable;
		return (DS_NOTOK);
	}

	/*
       	       *   get configuration information from a network manager
	       *   running version 1.
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
	 */

/*
 * do not allocate space for _devserver structure because this routine
 * could be called multiple times e.g. when database server has to be
 * reimported, simply point to a static _devserver structure which
 * is locally allocated in static space
 */
	msg_info.conf=(devserver)&msg_ds;
	db_info.conf =(devserver)&db_ds;

	sprintf (msg_info.conf->server_host, "%s", 
	    manager_data.msg_info.host_name);
	msg_info.conf->prog_number =  manager_data.msg_info.prog_number;
	msg_info.conf->vers_number =  manager_data.msg_info.vers_number;

	sprintf (db_info.conf->server_host, "%s", 
	    manager_data.db_info.host_name);
	db_info.conf->prog_number =  manager_data.db_info.prog_number;
	db_info.conf->vers_number =  manager_data.db_info.vers_number;

	/*
	 * Enable the security system if the Manager indicates 
	 * the use of the security system.
	 */

	if (manager_data.security == True)
	{
		config_flags.security =  True;
	}

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
	if (max_nethost <= 0) nethost_alloc(error);

	strcpy(multi_nethost[0].nethost,nethost);
	multi_nethost[0].config_flags = config_flags;
	multi_nethost[0].db_info = db_info.conf;
	multi_nethost[0].msg_info = msg_info.conf;

	return (DS_OK);
}


/*struct _devserver msg_ds[MIN_NETHOST], db_ds[MIN_NETHOST];*/
struct _devserver *msg_ds, *db_ds;


/*+**********************************************************************
 Function   :	static long setup_config_multi()

 Description:   gets the necessary configuration information
		for a static database service and a message
		service from  a network manager in a multi-nethost
		environment.
            :   The host of the network manager is specified
	        by the nethost_name parameter.    

 Arg(s) In  :   nethost_name - name of nethost to configure

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
		    	      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/

long setup_config_multi (char *nethost, long *error)
{
	_manager_data 		manager_data;
	_register_data  	register_data;
	static char		host_name[SHORT_NAME_SIZE];
	CLIENT			*clnt;
	enum clnt_stat		clnt_stat;
	int			pid;
	long			i_nethost, i;
#ifndef _UCC
	static char		nethost_env[80];
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

#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
		    "\nsetup_config_multi() : entering routine\n");
#endif /* EBUG */

	/*
 	 *  read environmental variable NETHOST
  	 */

	if ( nethost == NULL )
	{
		*error = DevErr_NethostNotDefined;
		return (DS_NOTOK);
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
#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
		    "\nsetup_config_multi() : add nethost %d\n",i_nethost);
#endif /* EBUG */
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

	gethostname (host_name,32);

#if !defined (_NT)
#if !defined (vxworks)
        pid = getpid ();
#else  /* !vxworks */
        pid = taskIdSelf ();
#endif /* !vxworks */
#else /* !_NT */
	pid = _getpid ();
#endif	/* !_NT */
	register_data.host_name   = host_name;
	register_data.prog_number = pid;
	register_data.vers_number = 0;

	/*
	 * Create network manager client handle 
	 */

	clnt = clnt_create ( nethost,
	    NMSERVER_PROG,
	    NMSERVER_VERS,
	    "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror ("setup_config");
		*error = DevErr_NoNetworkManagerAvailable;
		return (DS_NOTOK);
	}

	clnt_control (clnt, CLSET_RETRY_TIMEOUT, (char *) &api_retry_timeout);
	clnt_control (clnt, CLSET_TIMEOUT, (char *) &api_timeout);

	/*
       	 *   get configuration information from
       	 *   a network manager
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
               * Set version number to 1 and recreate the
               * client handle.
               */

	clnt = clnt_create (nethost,NMSERVER_PROG,NMSERVER_VERS_1,"udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror ("setup_config");
		*error = DevErr_NoNetworkManagerAvailable;
		return (DS_NOTOK);
	}

	/*
       	       *   get configuration information from a network manager
	       *   running version 1.
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
	 *  put message server and database server configuration
	 *  into the global structures msgserver_info and
	 *  dbserver_info.
	 */

/*
 * do not allocate space for _devserver structure because this routine
 * could be called multiple times e.g. when database server has to be
 * reimported, simply point to a static _devserver structure which
 * is always allocated
 */
	multi_nethost[i_nethost].msg_info = (devserver)&msg_ds[i_nethost];
	multi_nethost[i_nethost].db_info = (devserver)&db_ds[i_nethost];

	sprintf (multi_nethost[i_nethost].msg_info->server_host, "%s", 
	    manager_data.msg_info.host_name);
	multi_nethost[i_nethost].msg_info->prog_number =  
                                 manager_data.msg_info.prog_number;
	multi_nethost[i_nethost].msg_info->vers_number =  
                                 manager_data.msg_info.vers_number;

	sprintf (multi_nethost[i_nethost].db_info->server_host, "%s", 
	    manager_data.db_info.host_name);
	multi_nethost[i_nethost].db_info->prog_number =  
                                 manager_data.db_info.prog_number;
	multi_nethost[i_nethost].db_info->vers_number =  
                                 manager_data.db_info.vers_number;

	/*
	 * Enable the security system if the Manager indicates 
	 * the use of the security system.
	 */

	if (manager_data.security == True)
	{
		multi_nethost[i_nethost].config_flags.security =  True;
	}
	else
	{
		multi_nethost[i_nethost].config_flags.security =  False;
	}

/*
 * free memory allocated by xdr in manager_data (assume we have connected
 * to version 4 of the Manager
 */
	xdr_free((xdrproc_t)xdr__manager_data, (char *)&manager_data);
	clnt_destroy (clnt);
	strcpy(multi_nethost[i_nethost].nethost,nethost);
/*
	printf( "setup_config_multi(): configured nethost[%d] %s\n",
	        i_nethost,multi_nethost[i_nethost].nethost);
 */
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
 * Ultra-C++ does not find the symbole putenv() (maybe the prototype
 * is wrong). Supress its use for Ultra-C++. This means Ultra-C++
 * programs have to specify $NETHOST
 *
 * - andy 27nov96
 */
#ifndef _UCC
			sprintf(nethost_env,"NETHOST=%s",nethost);
			putenv(nethost_env);
#endif /*!_UCC*/
			if (setup_config(error) != DS_OK)
			{
				return(DS_NOTOK);
			}
		}
	}

	return (DS_OK);
}


/*+**********************************************************************
 Function   :	static long get_error_string()

 Description:   Read the error string from the global error table
		or from the resource database.
		The rsource name is:
		ERROR/team_no/server_no/error_ident:
                DS_WARNING is returned, if the function was
                executed correctly, but no error
                string was found in the database.

 Arg(s) In  :   long error    - error number

 Arg(s) Out :   char *error_str - error string.

 Return(s)  :   DS_OK or DS_NOTOK or DS_WARNING
***********************************************************************-*/

static char *get_error_string (long error)
{
	char				res_path[LONG_NAME_SIZE];
	char				res_name[SHORT_NAME_SIZE];
	char				*ret_str = NULL;
	char				*error_str = NULL;
	db_resource 	res_tab;
	long				db_error;
	unsigned short 	error_number_mask = 0xfff;
	unsigned short 	team;
	unsigned short 	server;
	unsigned short 	error_ident;
	short		i;

	time_t		sec;
	char		*time_stamp;
	static char	*no_error_mess=(char*)"No error message stored in the database for negative errors\n";
	static char	*db_error_mess=(char*)"Failed to get error message from database\n";


#ifdef EBUG
	dev_printdebug (DBG_TRACE | DBG_API,
	    "\nget_error_string() : entering routine\n");
#endif /* EBUG */

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
		error_str = (char*)malloc(strlen(time_stamp)+strlen(no_error_mess)+3);
                sprintf ( error_str, "%s  %s", time_stamp, no_error_mess );
                /*return (DS_WARNING);*/
                return (error_str);
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
				error_str = (char*)malloc(strlen(time_stamp)+
					strlen(DevErr_List[i].message)+3);
				sprintf ( error_str, "%s  %s", time_stamp, 
				    DevErr_List[i].message );
				/*return (DS_OK);*/
				return (error_str);
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
		error_str = (char*)malloc(strlen(time_stamp)+strlen(dev_error_string)+3);
		sprintf ( error_str, "%s  %s", time_stamp, dev_error_string );
		error_str[strlen(error_str)] = 0;
		/*return (DS_OK);*/
		return (error_str);
	}

/*
 * Create the resource path and the resource structure.
 */

	sprintf (res_path, "ERROR/%d/%d", team, server);
	sprintf (res_name, "%d", error_ident);
#ifdef EBUG
	dev_printdebug (DBG_API,
	    "get_error_string() : res_path = %s\n", res_path);
	dev_printdebug (DBG_API,
	    "get_error_string() : res_name = %s\n", res_name);
#endif /* EBUG */

	res_tab.resource_name = res_name;
	res_tab.resource_type = D_STRING_TYPE;
	res_tab.resource_adr  = &ret_str;

/*
 * Read the error string from the database.
 */

	if (db_getresource (res_path, &res_tab, 1, &db_error) == DS_NOTOK)
	{
#ifdef EBUG
		dev_printdebug (DBG_API | DBG_ERROR,
		    "get_error_string() : db_getresource failed with error %d\n",
		    db_error);
#endif /* EBUG */

/*
 * If the database call returned an error, search the global
 * error table for a description.
 */

		for (i=0; i<(MAX_DEVERR); i++)
		{
			if ((_Int)(DevErr_List[i].dev_errno) == (_Int)db_error)
			{
				error_str = (char*)malloc(strlen(time_stamp)+
					strlen(DevErr_List[i].message)+3);
				sprintf ( error_str, "%s  %s", time_stamp, 
				    DevErr_List[i].message );
				/*return (DS_NOTOK);*/
				return (error_str);
			}
		}

		error_str = (char*)malloc(strlen(time_stamp)+ strlen(db_error_mess)+3);
		sprintf ( error_str, "%s  %s", time_stamp, db_error_mess );
		/*return (DS_NOTOK);*/
		return (error_str);
	}

/*
 * If the variable ret_str is still NULL, no resource value was found
 * in the database, but the function was executed without error.
 * In this case return the value DS_WARNING.
 */
	if ( ret_str == NULL )
	{
		error_str = (char*)malloc(strlen(time_stamp)+strlen(no_error_mess)+3);
                sprintf ( error_str, "%s  %s", time_stamp, no_error_mess );
		/*sprintf ( error_str, 
		    "%s No error message stored in the database for error (%d/%d/%d)", 
		    time_stamp, team, server, error_ident );*/
		return (error_str);
	}

/*
 * Found the error string in the database. Return the string with 
 * a time stamp.
 */
	error_str = (char*)malloc(strlen(time_stamp)+strlen(ret_str)+3);
	sprintf ( error_str, "%s  %s", time_stamp, ret_str );
	free (ret_str);
	return (error_str);
}

/*+**********************************************************************
 Function   :	long dev_error_push()

 Description:   Push a dynamically generated error string onto the error 
		string stack so that it can be transferred back to the client 
		at the end of execution.

 Arg(s) In  :   char *error_string    - error string

 Arg(s) Out :   none

 Return(s)  :   DS_OK or DS_NOTOK or DS_WARNING
***********************************************************************-*/
long dev_error_push (char *error_string)
{
	if (dev_error_stack == NULL)
	{
		dev_error_stack = (char*)malloc(strlen(error_string)+1);
		dev_error_stack[0] = 0;
	}
	else
	{
		dev_error_stack = (char*)realloc(dev_error_stack, strlen(dev_error_stack)+strlen(error_string)+1);
	}
	sprintf(dev_error_stack+strlen(dev_error_stack),"%s",error_string);
	dev_error_stack[strlen(dev_error_stack)] = 0;

	return(DS_OK);
}

/*+**********************************************************************
 Function   :	long dev_error_clear()

 Description:   Clear the dynmaic error stack. This can be useful for servers
 		who want to remove error messages from the stack and return
		their own error only.

 Arg(s) In  :   none

 Arg(s) Out :   none

 Return(s)  :   DS_OK or DS_NOTOK or DS_WARNING
***********************************************************************-*/
long dev_error_clear ()
{
	if (dev_error_stack != NULL)
	{
		free(dev_error_stack);
		dev_error_stack = NULL;
	}

	return(DS_OK);
}

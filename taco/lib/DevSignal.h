
/*+*******************************************************************

 File:		DevSignal.h

 Project:	Device Servers with SUN-RPC

 Description:	Include file for a standard signal handling under
		HPUX, SUN and OS9.
		The definition of additional signal codes for OS9
		allows the use of the same makros on every system. 
		 
 Author(s):	Jens Meyer 
		$Author: jkrueger1 $

 Original:	June, 1991

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-03-14 12:22:07 $

 Copyleft (c) 1990 by European Synchrotron Radiation Facility, 
                      Grenoble, France

********************************************************************-*/

#ifndef _DevSignal_h
#define _DevSignal_h

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevSignalh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevSignal.h,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";
#endif /* _IDENT */


#ifdef OSK
/*
 * Get OS9 signal definition file
 */

#include <signal.h> 

/*
 * Define additional makros for OS9 
 */

#define SIGALRM 5
#define SIGTERM 6
#define SIGUSR1 7
#define SIGUSR2 8

/*
 * private OS9 signal definitions
 */
#define TIMEOUT_PLC	256
#define RECEIVE_PLC 	257
/*
 * allow also to use signal numbers up
 * to 512 without signal definition in this file.
 */
#define NUSIG	513   /* NSIG is 1 greater than the highest defined signal */
#else

#ifdef _OSK

/*
 * Ultra-C compatible code 
 *
 * in this case the signals SIGALRM, SIGTERM, SIGUSR1 and SIGUSR2 are defined 
 * as part of the POSIX set of signals and do/must not need to be defined
 * separately
 *
 * andy 8sep94
 */

#include <signal.h>

#define NUSIG	513   /* NUSIG is 1 greater than the highest defined signal */
                      /* use the same definition as used for Unibridge OSK  */

#endif /* _OSK */

/*
 * Get normal UNIX system signal definition file.
 * Set the number of available signals for UNIX systems
 */

#ifdef __hpux
#include <sys/signal.h>

#define NUSIG	_NSIG
#endif /*__hpux */

#if defined (solaris) || (sun) || (defined irix)
#include <signal.h>
#define NUSIG	NSIG
#endif /* solaris */

#ifdef lynx
#include <signal.h>

#define SIG_ERR -1 /* SIG_ERR is NOT defined in signal.h, */
		   /* contrary to what is said in the doc */

#define NUSIG	_NSIG
#endif /* lynx */
#endif /* OSK */


#if defined(_NT)
#include <signal.h>
#define NUSIG   NSIG
#endif  /* _NT */

#ifdef linux
#include <signal.h>
#define NUSIG   _NSIG
#endif /* linux */

#ifdef vxworks
#include <signal.h>
#define NUSIG	_NSIGS
#endif /* vxworks */

#endif /* _DevSignal_h */


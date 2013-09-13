/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:	DevSignal.h
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	Include file for a standard signal handling under
 *		HPUX, SUN and OS9.
 *		The definition of additional signal codes for OS9
 *		allows the use of the same makros on every system. 
 *		 
 * Author(s):	Jens Meyer 
 *		$Author: jkrueger1 $
 *
 * Original:	June, 1991
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2005-10-20 06:51:52 $
 *
 ********************************************************************-*/

#ifndef _DevSignal_h
#define _DevSignal_h

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevSignalh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevSignal.h,v 1.9 2005-10-20 06:51:52 jkrueger1 Exp $";
#endif /* _IDENT */

#if HAVE_SIGNAL_H || WIN32
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error could not find signal.h
#endif

#ifdef OSK
/*
 * Get OS9 signal definition file
 */

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

#define NUSIG	513   /* NUSIG is 1 greater than the highest defined signal */
                      /* use the same definition as used for Unibridge OSK  */

#endif /* _OSK */

/*
 * Get normal UNIX system signal definition file.
 * Set the number of available signals for UNIX systems
 */

#ifdef __hpux

#define NUSIG	_NSIG
#endif /*__hpux */

#if defined (_solaris) || (sun) || (defined irix)
#define NUSIG	NSIG
#endif /* _solaris */

#ifdef lynx

#define SIG_ERR -1 /* SIG_ERR is NOT defined in signal.h, */
		   /* contrary to what is said in the doc */

#define NUSIG	_NSIG
#endif /* lynx */
#endif /* OSK */


#if defined(WIN32)
#define NUSIG   NSIG
#endif  /* WIN32 */

#ifdef linux
#define NUSIG   _NSIG
#endif /* linux */

#ifdef FreeBSD
#define NUSIG   NSIG
#endif /* linux */

#ifdef vxworks
#define NUSIG	_NSIGS
#endif /* vxworks */

#endif /* _DevSignal_h */


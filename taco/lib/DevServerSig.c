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
 * File:        DevServerSignal.c
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: Device server signal handling function ds__signal
 *		gives a unique signal handling interface in UNIX
 *		style for servers running under HPUX, SUN and OS9.
 *
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	June 1991
 *
 * Version:	$Revision: 1.14 $
 *
 * Date:	$Date: 2008-10-23 05:28:17 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#if HAVE_SIGNAL_H
#	include <signal.h>
#elif HAVE_SYS_SIGNAL_H
#	include <sys/signal.h>
#else
#error could not find signal.h
#endif
#include <API.h>

#include <private/ApiP.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevSignal.h>

#if HAVE_RPC_PMAP_CLNT_H
#	include <rpc/pmap_clnt.h>
#else
#	include <rpc/pmap_cln.h>
#endif /* !WIN32 */

/**
 *  table of signal handler pointers
 */

#ifdef __cplusplus
static void 	(*sig_tab [NUSIG])(int) = {(void (*)(int))SIG_DFL};
#else
static void 	(*sig_tab [NUSIG])() = {SIG_DFL};
#endif

/**@ingroup dsAPI
 *
 * A general device server signal handling interface for
 * HPUX, SUN and OS9. Based on the UNIX way of treating
 * signals.
 *
 * Activated signal handling function pointers are stored 
 * in the global pointer table sig_tab.
 *
 * Actions like SIG_DFL and SIG_IGN are available for
 * all systems.
 *
 * @param sig    	signal
 * @param action 	pointer to function that will be
 *			called upon the receipt of signal sig.
 *
 * @param error 	Will contain an appropriate error
 *			code if the corresponding call
 *			returns a non-zero value.
 *
 * @retval DS_OK 
 * @retval DS_NOTOK
 */
long ds__signal (int sig, void (*action)(int), long *error)
{
	*error = 0;

/*
 *  check limits for valid signals
 */

#if defined (unix)
	if ( sig<1 || sig>=NUSIG)
#elif defined(WIN32)
	if ( sig<SIGINT || sig>=NUSIG)
#else
	if ( sig<0 || sig>=NUSIG)
#endif /* unix */
	{
		*error = DevErr_SignalOutOfRange;
		return (DS_NOTOK);
	}

/*
 *  store action in global table and acitvate
 *  the handler.
 */
	sig_tab[sig] 	= action;

/*
 *  check that the main quit signals will not be deactivated
 */
#ifdef __cplusplus
	if ( action == (void (*)(int)) SIG_DFL || 
	     action == (void (*)(int))SIG_IGN)
#else
	if ( action == SIG_DFL || action == SIG_IGN)
#endif
	{
#ifndef WIN32
		if ( sig!=SIGHUP && sig!=SIGINT && sig!=SIGQUIT && sig!=SIGTERM )
#else
		if ( sig!=SIGINT && sig!=SIGTERM )
#endif /* WIN32 */
		{
#ifdef __cplusplus
			if ( (void (*) (int)) signal (sig, action) == (void (*)(int)) SIG_ERR )
#else
			if ( (void (*) (int)) signal (sig, action) == SIG_ERR )
#endif
			{
				*error = DevErr_CannotSetSignalHandler;
				return (DS_NOTOK);
			}
	  	}
	}
	else
	{
/*
 *  activate signal
 */
#ifdef __cplusplus
		if ( (void (*) (int)) signal (sig, main_signal_handler) == (void (*)(int)) SIG_ERR )
#else
		if ( (void (*) (int)) signal (sig, main_signal_handler) == SIG_ERR )
#endif
		{
			*error = DevErr_CannotSetSignalHandler;
			return (DS_NOTOK);
		}
	}
	return (DS_OK);
}


/**@ingroup dsAPI
 *
 * A global device server signal handler which catches
 * in any case all main quit/kill signals for a proper
 * server exit and executes all other signal actions
 * activated by  ds__signal() calls.
 *
 * @param signo signal
 */
void main_signal_handler (int signo)
{
/*
 *  check limits for valid signals
 */
#if defined (unix)
	if ( signo < 1 || signo >= NUSIG)
#elif defined(WIN32)
	if ( signo< SIGINT || signo>=NUSIG)
#else
	if ( signo < 0 || signo >= NUSIG)
#endif /* unix */
		return;
	
/*
 *  call of internal device server signal handling function
 *  related to signo.
 */
#ifdef __cplusplus
	if ( sig_tab[signo] != (void (*) (int)) SIG_DFL && 
	     sig_tab[signo] != (void (*) (int)) SIG_IGN )
#else
	if ( sig_tab[signo] != (void (*) ()) SIG_DFL && 
	     sig_tab[signo] != (void (*) ()) SIG_IGN )
#endif
		sig_tab[signo] (signo);

/*
 *  filter signals for quitting the server
 */

#if defined (WIN32)
	if (signo==SIGINT || signo==SIGTERM || signo==SIGABRT || signo==SIGBREAK)
#else
	if (signo==SIGQUIT || signo==SIGINT || signo==SIGHUP || signo==SIGTERM || signo==SIGABRT)
#endif /* WIN32 */
     	{
		unregister_server ();
/*
 * now we exit after returning from unregister_server()
 * this flawless change allows to use unregister_server() at
 * any instant without unwanted server exit - what a deal
 */
      	    	exit(1);
	}

/* 
 *  Set up signal handler again
 */
	(void) signal (signo, main_signal_handler);
}


/**
 * Unregisters the device server from the
 * static database and the portmapper and
 * closes open handles to database and messages
 * services.
 */
void unregister_server (void)
{
	DevLong error = 0;
	LOCK(async_mutex);
/*
 * if this is a bona fida device server and it is using the database
 * then unregister server from database device table
 */
	if (config_flags->device_server == True)
	{
		if (!config_flags->no_database  
			&& (db_svc_unreg (config_flags->server_name, &error) != DS_OK))
	   			dev_printerror_no (SEND,"db_svc_unreg failed",error);
/*
 *  destroy open client handles to message and database servers
	 	clnt_destroy (db_info.conf->clnt);
	 	clnt_destroy (msg_info.conf->clnt);
 */
	}
/*
 * unregister synchronous version (4) of server from portmapper
 */
	pmap_unset (config_flags->prog_number, API_VERSION);
/*
 * unregister the asynchronous version (5) of the server from portmapper
 */
	pmap_unset (config_flags->prog_number, ASYNCH_API_VERSION);
/*
 *  finally unregister version (1) used by gettransient_ut()
 */
	pmap_unset (config_flags->prog_number, DEVSERVER_VERS);
/* 
 * the server has been unregistred, so set flag to false! 
 * otherwise, there may be more than one attempt to unregister the server
 * in multithreaded apps.
 */
	config_flags->device_server = False;    
	UNLOCK(async_mutex);
/*
 * returning here and calling exit() later from main_signal_handler() will
 * permit us to call unregister_server() from a different signal handler
 * and continue to do something useful afterwards
 */
	return;
}


/*+*******************************************************************

 File:          DevServerSignal.c

 Project:       Device Servers with SUN-RPC

 Description:   Device server signal handling function ds__signal
		gives a unique signal handling interface in UNIX
		style for servers running under HPUX, SUN and OS9.

 Author(s):	Jens Meyer

 Original:	June 1991


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:27 $
 $Author: jkrueger1 $

 Copyright (c) 1990 by  European Synchrotron Radiation Facility,
			Grenoble, France

*******************************************************************-*/

#include <DevServer.h>
#include <DevErrors.h>
#include <DevSignal.h>

/*
 *  table of signal handler pointers
 */

void 	(*sig_tab [NUSIG])() = {SIG_DFL};



#ifdef __STDC__
long ds__signal (int sig, void (*action)(), long *error)
#else
/**/
long ds__signal (sig, action, error)
/*+**********************************************************************
 Function   :  	extern long ds__signal()

 Description: 	A general device server signal handling interface for
		HPUX, SUN and OS9. Based on the UNIX way of treating
		signals.
		Activated signal handling funcion pointers are stored 
		in the global pointer table sig_tab.
		Actions like SIG_DFL and SIG_IGN are available for
		all systems.
		The related include file : DevSignal.h

 Arg(s) In  :	int sig          - signal
            :	void (*action)() - pointer to function that will be
				   called upon the receipt of signal sig.

 Arg(s) Out :   long *error - Will contain an appropriate error
			      code if the corresponding call
			      returns a non-zero value.

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
	int	sig;		/* */
	void	(*action)();	/* */
	long	*error;		/* */
#endif /* __STDC__ */



{
	*error = 0;

	/*
	 *  check limits for valid signals
	 */

#ifdef unix
	if ( sig<1 || sig>=NUSIG)
#endif /* unix */
#ifndef unix
	if ( sig<0 || sig>=NUSIG)
#endif /* unix */
	   {
	   *error = DevErr_SignalOutOfRange;
	   return (-1);
	   }

	/*
	 *  store action in global table and acitvate
	 *  the handler.
	 */

	sig_tab[sig] 	= action;

#ifdef unix
	/*
	 *  check that the main quit signals will not be deactivated
	 */

	if ( action == SIG_DFL || action == SIG_IGN)
	   {
	   if ( sig!=SIGHUP && sig!=SIGINT && sig!=SIGQUIT && sig!=SIGTERM )
	      {
	      if ( signal (sig, action) == SIG_ERR )
		 {
	         *error = DevErr_CannotSetSignalHandler;
	         return (-1);
		 }
	      }
	   }
	else
	   {
	   /*
	    *  activate signal
	    */

	   if ( signal (sig, main_signal_handler) == SIG_ERR )
	      {
	      *error = DevErr_CannotSetSignalHandler;
	      return (-1);
	      }
	   }
#endif /* unix */

	return (0);
}




#ifdef __STDC__
void main_signal_handler (int signo)
#else
/**/
void main_signal_handler (signo)
/*+**********************************************************************
 Function   : 	extern void main_signal_handler ()

 Description:	A global device server signal handler which catches
		in any case all main quit/kill signals for a proper
		server exit and executes all other signal actions
		activated by  ds__signal() calls.

 Arg(s) In  :	int signo - signal

 Arg(s) Out : 	none

 Return(s)  :   DS_OK or DS_NOTOK
***********************************************************************-*/
	int     signo;		/* */
#endif /* __STDC__ */

{
	/*
	 *  check limits for valid signals
	 */

#ifdef unix
	if ( signo<1 || signo>=NUSIG)
#endif /* unix */
#ifndef unix
	if ( signo<0 || signo>=NUSIG)
#endif /* unix */
	   {
	   return;
	   }
	
 	/*
	 *  call of internal device server signal handling function
	 *  related to signo.
	 */

	if ( sig_tab[signo] != (void (*) ()) SIG_DFL && 
	     sig_tab[signo] != (void (*) ()) SIG_IGN )
	   {
	   sig_tab[signo] (signo);
	   }

	/*
	 *  filter signals for quitting the server
	 */

	if (signo==SIGQUIT || signo==SIGINT || signo==SIGHUP ||
	    signo==SIGTERM)
     	   {
      	   unregister_server ();
	   }

#ifdef unix
	/* 
	 *  Set up signal handler again
	 */

	(void) signal (signo, main_signal_handler);
#endif /* unix */

}


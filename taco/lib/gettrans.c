/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File:        gettrans.c
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: get transient program number  
 *		gettransient    -> Old version. Reserves port only for one
 *				   protocol.
 *		gettransient_ut -> New version. Reserves port for UDP and
 *				   TCP protocol.
 * Author(s):   Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:    09.01.1991
 *
 * Version:	$Revision: 1.9 $
 *
 * Date:	$Date: 2006-09-18 22:31:44 $
 *
 ********************************************************************-*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 
#include <API.h>
#include <private/ApiP.h>
#include <DevServer.h>
#include <Admin.h>

#ifdef unix
#	include <rpc/pmap_clnt.h>
#	include <rpc/pmap_prot.h>
#	if HAVE_SYS_SOCKET_H
#		include <sys/socket.h>
#	else
#		include <socket.h>
#	endif
#elif defined WIN32
#	include <rpc/pmap_pro.h>
#	include <rpc/pmap_cln.h>
#endif

#if ( OSK | _OSK )
#	ifdef __cplusplus
extern "C" {
#	endif
	extern struct pmaplist *pmap_getmaps(struct sockaddr_in *address); 
	extern unsigned int tsleep(unsigned int);
#	ifdef __cplusplus
}
#	endif
#	include <rpc/pmap_clnt.h>
#	include <rpc/pmap_prot.h>
#	include <inet/socket.h>
#endif /* OSK | _OSK */

#ifdef vxworks
#	include <taskLib.h>
#endif /* vxworks */

#ifdef _XOPEN_SOURCE_EXTENDED
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif /* XOPEN_SOURCE_EXTENDED */

#if defined (__hpux) || (WIN32)
#ifdef __cplusplus
extern "C" {
#endif
	extern void get_myaddress (struct sockaddr_in *);
#ifdef __cplusplus
}
#endif
#endif /* hpux || WIN32 */

#include <stdlib.h>

/*+=======================================================================

A. Gotz,  27.04.2000

Problems with M.Diehl's gettransient() call :

* On OS9 if multiple programs (e.g. device servers or even rpcinfo) call 
  pmap_getmaps() simultaneously the portmapper dies ! 
* No protection against multiple programs getting the same program number
  and then trying to register it. 

In order to solve this I have added the following patch :

* A random sleep (using rand()) every time time gettransient is called 
  will reduce the chances of device servers calling pmap_getmap() at
  the same time thereby reducing the probability of the portmapper
  crashing on OS9** and device servers getting the same pmap list.

* The svc_register routine will make multiple attempts to get a
  free program number i.e. multiple calls to gettransient.

** The OS9 problem is still there but by introducing sleeps in
   the startup procedures of the device servers the problem can be avoided.
   This solution has been preferred to using semaphores to solve
   the problem because semaphores can block forever if a process
   crashes and because the problem is limited to OS9 (sigh).
   
M. Diehl, 15.11.99

Complete reimplementation of the transient RPC prognum allocation scheme :

* No need to bind sockets and set/unset prognums (and for the corresponding
  patches) anymore
* identical transient number hashing for real device servers, asynch.
  clients and the message/database server - i.e. only one function
* no collisions between database and message server during program number
  allocation. This was catching us in a race condition leading to the
  portmapper fork problem - the portmapper-patch for SuSE was apperently
  helpful only due to timing side-effects!
* selecting only prognums for which absolutely no version/proto combination
  has been registered before. This means, that exactly one call to
  gettransient() is required for every Taco service user.
* Fixing some inconsistencies in prognum/version handling by the asynch api,
  if called from a real device server, which has already registered the
  normal sync api services.
* BUT: still some (extremely small, however) chance for a race condition,
  if a number of servers is simultaneously started by some starting program
  and any 2 of them come up with the same hashvalue and for those a
  scheduler switch happens between the calls to gettransient() and the first
  svc_register(). This is extremely unlikely and the chance for such a
  collision could be reduced beyond any reasonable limit, if the starting
  program sleeps for, say some 100msec, between starting of any two
  programs. To make this absolutely failsafe one has to use a host-wide
  atomic locking of the critical section (using SYSV-IPC semaphores e.g.).
  Using the version 1/udp portmapper registration as a lock is not a good
  solution not only because the portmapper forks, but also because
  version 1/udp is needed for database and message server forcing us
  to unregister it again for some short moment - allowing the race
  condition between those 2 programs. This is a real issue since the
  old implementation started trying at the same prognum base. How critical
  this is was demonstrated by the influence of the portmap daemon compiling
  options (read: timing due to additional security checks!).
=========================================================================-*/

int gettransient(const char *ds_name)
{
/*
#ifdef WIN32
	LPpmaplist      plist, 
			p;
#else
 */
        struct pmaplist *plist, 
						*p;
/*
#endif
 */
	u_long		base = 0x50000000, 
			key = 131, 
			maxoffset = 0x10000000;
	u_long  	offset, 
			prognum, 
			i, 
			found;
	struct sockaddr_in addr;
#if defined (_UCC) || (WIN32)
	long backoff;
#else
	struct timespec backoff;
#endif
	long 		pid, 
			first = 1;

	if (first)
	{
#if defined (WIN32)
        	pid = _getpid ();
#else
#if !defined (vxworks)
        	pid = getpid ();
#else  /* !vxworks */
        	pid = taskIdSelf ();
#endif /* !vxworks */
#endif
		srand(pid);
		first = 0;
	}
/*
 * before doing anything first backoff a random fraction of a second
 * this will help reduce the probability that multiple servers interrogate
 * the portmapper at exactly the same time cf. above
 */
#ifdef _UCC
	backoff = 256. * (float)rand() / (float)RAND_MAX;
	tsleep(0x80000000|backoff);
#else
#ifdef WIN32
	backoff = (long)(1000. * (float)rand() / (float)RAND_MAX);
	Sleep(backoff);
#else
	backoff.tv_sec = 0;
	backoff.tv_nsec = 1000000000. * (float)rand() / (float)RAND_MAX;
	nanosleep(&backoff, NULL);
#endif /* WIN32 */
#endif /* _UCC */

/*
 * determine the offset from base transient program number using a hashing
 * function to calculate an (almost) unique code based on the device
 * server full name (server/personal name). Because the personal
 * name (server/personal name) is unique in a TACO control system
 * the offset will be unique. The hashing function used is very similar
 * to the so-called "coalesced hashing" function also used by E.Taurel
 * in the data collector. It has been simplified here to the following
 *
 * hashing code = sum ( ds_name[i] * 131 ^ i ) modulo 0x10000000
 *
 * The modulo is necessary because transient program numbers must lie 
 * between 40000000 and 5fffffff and to avoid confusion with old servers
 * a base of 0x50000000 has been used. 
 *
 * andy 12jun97
 */
	offset = 0;
	for (i = 0; i < strlen(ds_name); i++)
		offset = (offset * key + (u_long)ds_name[i]) % maxoffset;

	prognum = base + offset;

/*
 * Because the hashing function is not unique a check must be made for
 * collisions. This is done by looping round testing to see if the program 
 * number is already bound until an unbound program number is found or
 * until all program numbers have been checked.
 */
        get_myaddress(&addr);
	plist = pmap_getmaps(&addr);
	if(NULL == plist)
	{
/*
 * if plist is NULL assume simply that no programs are registered
 * and return prognum as is - andy 2/11/2001
 */
		dev_printdebug(DBG_ERROR | DBG_STARTUP, "gettransient(): pmap_getmaps() failed , aborting !\n");
		return prognum;

	}
  	found = 0;
        do
        {
		if (prognum >= (base + maxoffset - 1))
			prognum = base;

		for (p = plist; p != NULL; p = p->pml_next)
            		if( prognum == p->pml_map.pm_prog )
				break;
		if (p == NULL)
		{
			found = 1;
			break;
		}
		prognum++;
 	} while(prognum != (base + offset));

	if (found == 0)
	{
		printf("gettransient(): failed to find free prognum = %d\n", prognum);
		return 0;
	}
 	
/*	xdr_free((xdrproc_t)xdr_pmap,(char *)plist); */
        return prognum;
}

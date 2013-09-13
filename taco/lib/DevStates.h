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
 * File:	DevStates.h
 * 
 * Project:	Device Servers
 *
 * Description:	Public include device server state definitions
 *
 * Author(s):	Andy Goetz
 *		Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	March 1992
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:08:22 $
 *
 ********************************************************************-*/

#ifndef _DevStates_h
#define _DevStates_h


/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevStatesh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevStates.h,v 1.3 2005-07-25 13:08:22 jkrueger1 Exp $";
#endif /* _IDENT */


/*
 *  Device states   
 *
 *  Should be only defined in the general include file
 *  if they are used as return values to the client.
 */

#define DEVUNKNOWN	 0

#define DEVOFF		 1
#define DEVON		 2

#define DEVCLOSE	 3
#define DEVOPEN		 4

#define DEVLOW		 5
#define DEVHIGH		 6

#define DEVINSERTED	 7
#define DEVEXTRACTED	 8
#define DEVMOVING	 9

#define DEVWARMUP 	10
#define DEVINIT		11

#define DEVSTANDBY	12
#define DEVSERVICE	13
#define DEVRUN		14

#define DEVLOCAL	15
#define DEVREMOTE	16
#define DEVAUTOMATIC	17

#define DEVRAMP		18
#define DEVTRIPPED	19
#define DEVHV_ENABLE	20
#define DEVBEAM_ENABLE	21

#define DEVBLOCKED	22
#define DEVFAULT	23

#define DEVSTARTING		24	
#define DEVSTOPPING		25	
#define DEVSTARTREQUESTED	26  	/* startup sequence requested but
					   not initialised */
#define DEVSTOPREQUESTED	27	/* stop sequence requested but
					   not initialised */

#define DEVPOSITIVEENDSTOP	28
#define DEVNEGATIVEENDSTOP	29


#define DEVBAKEREQUESTED   	30   /* bake requested but not yet started */
#define DEVBAKEING         	31   /* channel is bakeing */
#define DEVSTOPBAKE        	32   /*	channel bake is stopping */

#define DEVFORCEDOPEN           33
#define DEVFORCEDCLOSE          34

#define DEVOFFUNAUTHORISED      35    /* off, restarting not authorised */
#define DEVONNOTREGULAR         36    /* on, temp not regulated  */
#define DEVRESETTING          	37    /* resetting */
#define DEVFORBIDDEN          	38
#define DEVOPENING          	39
#define DEVCLOSING          	40
#define DEVUNDEFINED          	41

#define DEVCOUNTING             42      /* Device counting */
#define STOPPED                 43      /* Device stopped  */
#define RUNNING                 44      /* Device running */
#define DEVALARM                45      /* Powersupply in alarm */
#define DEVDISABLED		46

#define DEVSTANDBY_NOT_REACHED	47
#define DEVON_NOT_REACHED	48

#if !defined(FORTRAN)
extern _DLLFunc const char* DEVSTATES[];
#endif /* FORTRAN */

#endif /* _DevStates_h */

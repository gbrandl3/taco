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
 * File:        MachNumbers.h
 *
 * Project:     Device Servers with SUN-RPC
 *
 * Description: Include file with the definitions for all 
 *		software of the machine theory group 
 *
 * Author(s):   J. Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:    June 1994
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:08:23 $
 *
 ********************************************************************-*/
#include <DserverTeams.h>
#include <macros.h>

#ifndef _MachDsNumbers_h
#define _MachDsNumbers_h


/*------- Device server identification ---------*/

#define	AutoCorBase	(MachTeamNumber + DS_LSHIFT(1,DS_IDENT_SHIFT))
#define	AppliBase	(MachTeamNumber + DS_LSHIFT(2,DS_IDENT_SHIFT))
#define	MessageBase	(MachTeamNumber + DS_LSHIFT(3,DS_IDENT_SHIFT))
#define	FbpsBase	(MachTeamNumber + DS_LSHIFT(4,DS_IDENT_SHIFT))
/* base for Unidos dosemeter (J.M. Chaize) */
#define	UnidosBase	(MachTeamNumber + DS_LSHIFT(5,DS_IDENT_SHIFT))

#endif	/* _MachDsNumbers_h */
